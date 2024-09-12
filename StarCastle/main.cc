#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "zorolib.h"

enum WindowState
{
  MENU = 0,
  LOGIN,
  SCOREBOARD,
  CREDITS,
  SIGNUP,
  INGAME,
  ADMIN,
  GAMEOVER
};

struct TShip
{
  zoro::Vec2 pos = {400.0f, 400.0f};
  zoro::Vec2 speed = {0.0f, 0.0f};
  zoro::Vec2 acceleration = {0.0f, 0.0f};

  // Shape
  zoro::Vec3 *g_points;
  zoro::Vec2 *dr_points;

  zoro::Vec3 *g_Fuelpoints;
  zoro::Vec2 *dr_Fuelpoints;

  int score = 0;
  int lives = 3;
  int kNPoints = 10;
  float scale = 350.0f;
  float angle = 0.0f;
  bool IsPressing = false;
  bool isPaintingFuel = false;
  bool isInvincible = false;
  bool isDying = false;
  zoro::Mat3 M;
};

struct TUser
{
  int id;
  char *nick;
  char *pass;
  char *email;
  char *name;
  char *lastname;
  char *prov;
  char *country;
  char *birthday;
  char *highscoreDate;
  int credits;
  int highscore;
  bool isAdmin;
};

struct TShot
{
  TShot *next;
  TShot *prev;
  bool isEnemy;
  zoro::Vec2 pos;
  zoro::Vec2 dir;
  float speed;
  float travelDistance;
  bool active;
};

struct TButton
{
  bool isInput;
  bool focused;
  int idFunction;
  int fontSize;
  zoro::Vec2 pos;
  zoro::Vec2 dimensions;
  WindowState windowContext;
  char *text;
  char *label;
  bool hasLabel;
  TButton *next;
};

struct TUserList
{
  TUser *user;
  TUserList *next;
  TUserList *prev;
};

struct TPopup
{
  bool Activate;
  bool isActive;
  bool isEntering;
  bool isOutering;
  char *text;
  float activeTime;
  float maxActiveTime;
};

struct TScoreboard
{

  int score;
  char *date;
  char *username;
  TScoreboard *next;
  TScoreboard *prev;
};

enum WindowState GAMESTATE = MENU;
bool isLogged = false;
bool admin_change = false;
int level = 1;

bool isNewTop10 = false;

TUser *user1 = nullptr;
TUser *tempUser = nullptr;
TUserList *UserList = nullptr;

TShip ship;
TShip enemyShip;

TButton *InterfaceList;
TShot *shotlist;

TScoreboard *scoreList = nullptr;

zoro::Vec2 *sqPoints; // Square points

char *TextBuffer = nullptr;

TPopup popup;

TButton *adminButton = nullptr;

TButton *currentFocus = nullptr;

TButton *username = nullptr;
TButton *password = nullptr;
TButton *username_Signup = nullptr;
TButton *password_Signup = nullptr;
TButton *email = nullptr;
TButton *name = nullptr;
TButton *lastname = nullptr;
TButton *prov = nullptr;
TButton *country = nullptr;
TButton *birthday = nullptr;
TButton *highscoreDate = nullptr;

TButton *admin_username = nullptr;
TButton *admin_password = nullptr;
TButton *admin_email = nullptr;
TButton *admin_name = nullptr;
TButton *admin_lastname = nullptr;
TButton *admin_prov = nullptr;
TButton *admin_country = nullptr;
TButton *admin_birthday = nullptr;
TButton *admin_highscoreDate = nullptr;
TButton *admin_credits = nullptr;
TButton *admin_highscore = nullptr;

bool lookingNextButton = false;

bool mouseIsDown = false;
zoro::Vec2 mouseDownPos;

float scrollOffset = 0.0f;


//    TIME VARIABLES
unsigned char fps = 60;
double current_time, last_time;

const float sprFuelTimeRef = 100.0f;
double sprFuelTime = 0.0f;

const float shotCooldownTimeRef = 100.0f;
double shotCooldownTime = 0.0f;

const float InvincibleTimeRef = 4500.0f;
const float PaintTimeRef = 200.0f;

double InvincibleTime = 0.0f;
double PaintTime = 0.0f;

const float LastShotTimeRef = 5000.0f;
double LastShotTime = 0.0f;

const float UFORepositionTimeRef = 1000.0f;
double UFORepositiontTime = 0.0f;

float UFOAttackCooldownTimeRef = 2500.0f;
double UFOAttackCooldownTime = 0.0f;

const float UFOCooldownTimeRef = 15000.0f;
double UFOCooldownTime = 0.0f;

const float MenuCooldownTimeRef = 300.0f;
double MenuCooldownTime = 0.0f;

const float BackspaceCooldownTimeRef = 80.0f;
double BackspaceCooldownTime = 0.0f;

const float ShipDyingAnimTimeRef = 1000.0f;
double ShipDyingAnimTime = 0.0f;

//  CONSTANTS
FILE *f;

const float WINDOW_X = 800.0f, WINDOW_Y = 800.0f;

const int newUserCredit = 10;

const float shotMaxDistance = 700.0f;

const float shotSpeed = 10.0f;

const float shipScale = 50.0f;

void startNewGame();
void startNewLevel();
void deleteShot(TShot *s);
bool ufoColisionCalculation(zoro::Vec2 pos);
void ufoDeath();
void ScrollbarController(int i, float boxheight, float min_height, float max_height, float width, float xpos);

// -----------------------------------User Data -----------------------------------

void cleanChar(char *c)
{
  for (int i = 0; i < 40; i++)
    *(c + i) = '\0';
}

void EmptyUserData(TUser *user)
{
  cleanChar(user->nick);
  cleanChar(user->email);
  cleanChar(user->country);
  cleanChar(user->birthday);
  cleanChar(user->lastname);
  cleanChar(user->name);
  cleanChar(user->pass);
  cleanChar(user->prov);
  cleanChar(user->highscoreDate);
  user->highscore = 0;
  user->id = 0;
  user->isAdmin = false;
}

void PrintUser(TUser *user)
{
  printf("\nMemory address: %d\nID: %d\nNick: %s\nPass: %s\nEmail: %s\nName: %s\nLastName: %s\nProv: %s\nCountry: %s\nCredits: %d\nHS: %d\n ",
         (int)user, user->id, user->nick, user->pass, user->email, user->name, user->lastname, user->prov, user->country, user->credits, user->highscore);
}

void PrintUserPass(TUser *user)
{
  printf("\nNick: %s\nPass: %s\n ",
         user->nick, user->pass);
}

void PrintUserList()
{
  TUserList *p = UserList;
  printf("----------------------------\n");
  while (p != nullptr)
  {
    PrintUserPass(p->user);
    p = p->next;
  }
  printf("----------------------------\n");
}

void initEmptyUser(TUser **newuser)
{
  *newuser = (TUser *)malloc(sizeof(TUser));
  (*(newuser))->birthday = (char *)malloc(40 * sizeof(char));
  (*(newuser))->highscoreDate = (char *)malloc(40 * sizeof(char));
  (*(newuser))->country = (char *)malloc(40 * sizeof(char));
  (*(newuser))->email = (char *)malloc(40 * sizeof(char));
  (*(newuser))->lastname = (char *)malloc(40 * sizeof(char));
  (*(newuser))->name = (char *)malloc(40 * sizeof(char));
  (*(newuser))->nick = (char *)malloc(40 * sizeof(char));
  (*(newuser))->pass = (char *)malloc(40 * sizeof(char));
  (*(newuser))->prov = (char *)malloc(40 * sizeof(char));
  (*(newuser))->credits = 0;
  (*(newuser))->id = 0;
  (*(newuser))->isAdmin = false;
  (*(newuser))->highscore = 0;
}

int UserListLength()
{
  int length = 0;

  TUserList *p = UserList;

  while (p != nullptr)
  {
    length++;
    p = p->next;
  }

  return length;
}

void insertUser(TUser *newUser, bool isSignUp)
{
  TUserList *ulist = UserList;

  TUserList *newUserInList = (TUserList *)malloc(sizeof(TUserList));

  initEmptyUser(&newUserInList->user);

  newUserInList->user->id = UserListLength() + 1;

  if (isSignUp)
  {
    newUserInList->user->credits = newUserCredit;
  }
  else
  {
    newUserInList->user->credits = newUser->credits;
  }

  newUserInList->user->highscore = newUser->highscore;
  newUserInList->user->isAdmin = newUser->isAdmin;

  newUserInList->user->email = strdup(newUser->email);
  newUserInList->user->lastname = strdup(newUser->lastname);
  newUserInList->user->name = strdup(newUser->name);

  newUserInList->user->nick = strdup(newUser->nick);
  newUserInList->user->pass = strdup(newUser->pass);
  newUserInList->user->prov = strdup(newUser->prov);
  newUserInList->user->country = strdup(newUser->country);
  newUserInList->user->birthday = strdup(newUser->birthday);
  newUserInList->user->highscoreDate = strdup(newUser->highscoreDate);

  newUserInList->next = ulist;
  newUserInList->prev = nullptr;

  if (ulist != nullptr)
  {
    ulist->prev = newUserInList;
  }

  UserList = newUserInList;
  // printf("newuser: %d\ntempuser: %d", (int)newUserInList->user, (int)newUser);
  // printf("\nUSER ADDED TO LIST\n");
}

void freeUser(TUser *user)
{
  if (user->email)
    free(user->email);
  if (user->lastname)
    free(user->lastname);
  if (user->name)
    free(user->name);
  if (user->nick)
    free(user->nick);
  if (user->pass)
    free(user->pass);
  if (user->prov)
    free(user->prov);
  if (user->country)
    free(user->country);
  if (user->birthday)
    free(user->birthday);
  if (user->highscoreDate)
    free(user->highscoreDate);
  free(user);
}

bool deleteUser(int uid)
{

  TUserList *ulist = UserList;
  bool isDeleted = false;

  while (ulist != nullptr && !isDeleted)
  {
    if (ulist->user->id == uid)
    {
      if (ulist->prev != nullptr)
      {
        ulist->prev->next = ulist->next;
      }
      if (ulist->next != nullptr)
      {
        ulist->next->prev = ulist->prev;
      }
      freeUser(ulist->user);
      free(ulist);
      isDeleted = true;
    }
    ulist = ulist->next;
  }
  return isDeleted;
}

void SaveAdminEdit()
{
  tempUser->nick = strdup(admin_username->text);
  tempUser->pass = strdup(admin_password->text);
  tempUser->email = strdup(admin_email->text);
  tempUser->name = strdup(admin_name->text);
  tempUser->lastname = strdup(admin_lastname->text);
  tempUser->prov = strdup(admin_prov->text);
  tempUser->country = strdup(admin_country->text);
  tempUser->birthday = strdup(admin_birthday->text);
  tempUser->highscoreDate = strdup(admin_highscoreDate->text);

  tempUser->credits = atoi(admin_credits->text);
  tempUser->highscore = atoi(admin_highscore->text);

  tempUser->isAdmin = admin_change;
}

void SaveDataUser()
{

  if (fopen("./resources/users.zoro", "r+b") == NULL)
  {
    f = fopen("./resources/users.zoro", "w+b");
    fclose(f);
  }

  f = fopen("./resources/users.zoro", "w+b");

  TUserList *p = UserList;

  while (p != nullptr)
  {

    tempUser = p->user;

    fwrite(&tempUser->id, sizeof(int), 1, f);
    fwrite(tempUser->nick, sizeof(char), 40, f);
    fwrite(tempUser->pass, sizeof(char), 40, f);
    fwrite(tempUser->email, sizeof(char), 40, f);
    fwrite(tempUser->name, sizeof(char), 40, f);
    fwrite(tempUser->lastname, sizeof(char), 40, f);
    fwrite(tempUser->prov, sizeof(char), 40, f);
    fwrite(tempUser->country, sizeof(char), 40, f);
    fwrite(tempUser->birthday, sizeof(char), 40, f);
    fwrite(tempUser->highscoreDate, sizeof(char), 40, f);
    fwrite(&tempUser->credits, sizeof(int), 1, f);
    fwrite(&tempUser->highscore, sizeof(int), 1, f);
    fwrite(&tempUser->isAdmin, sizeof(bool), 1, f);
    p = p->next;
  }

  fclose(f);
}

void LoadUserList()
{

  if (fopen("./resources/users.zoro", "r+b") != NULL)
  {

    f = fopen("./resources/users.zoro", "r+b");

    while (fread(&tempUser->id, sizeof(int), 1, f) == 1)
    {

      fread(tempUser->nick, sizeof(char), 40, f);
      fread(tempUser->pass, sizeof(char), 40, f);
      fread(tempUser->email, sizeof(char), 40, f);
      fread(tempUser->name, sizeof(char), 40, f);
      fread(tempUser->lastname, sizeof(char), 40, f);
      fread(tempUser->prov, sizeof(char), 40, f);
      fread(tempUser->country, sizeof(char), 40, f);
      fread(tempUser->birthday, sizeof(char), 40, f);
      fread(tempUser->highscoreDate, sizeof(char), 40, f);
      fread(&tempUser->credits, sizeof(int), 1, f);
      fread(&tempUser->highscore, sizeof(int), 1, f);
      fread(&tempUser->isAdmin, sizeof(bool), 1, f);

      insertUser(tempUser, false);
    }

    fclose(f);
  }
  else
  {
    printf("SAVE DATA DOES NOT EXIST.\n");
  }
}

bool CheckLogin(TUser *usercheck)
{
  TUserList *ulist = UserList;
  bool match = false;

  while (ulist != nullptr && !match)
  {
    if (strcmp(ulist->user->nick, usercheck->nick) == 0 && strcmp(ulist->user->pass, usercheck->pass) == 0)
    {
      user1 = ulist->user;
      printf("\n Correct Login \n");
      match = true;
    }

    ulist = ulist->next;
  }

  return match;
}

bool checkIfUserDataIsFull(TUser *userdata)
{

  if (*(userdata->country + 0) == '\0')
    return false;

  if (*(userdata->email + 0) == '\0')
    return false;

  if (*(userdata->lastname + 0) == '\0')
    return false;

  if (*(userdata->name + 0) == '\0')
    return false;

  if (*(userdata->nick + 0) == '\0')
    return false;

  if (*(userdata->pass + 0) == '\0')
    return false;

  if (*(userdata->prov + 0) == '\0')
    return false;

  if (*(userdata->birthday + 0) == '\0')
    return false;

  return true;
}

bool checkUsernameValid(TUser *usercheck)
{
  TUserList *ulist = UserList;

  PrintUserList();
  printf("\n new user name: %s", usercheck->nick);
  while (ulist != nullptr)
  {
    if (strcmp(ulist->user->nick, usercheck->nick) == 0)
    {
      printf("\n%s = %s\n", ulist->user->nick, usercheck->nick);
      return false;
    }

    ulist = ulist->next;
  }

  return true;
}

bool checkDate(char *date)
{

  if (*(date + 0) < 48 || *(date + 0) > 51)
    return false;
  if (*(date + 1) < 48 || *(date + 1) > 57)
    return false;

  if (*(date + 2) != '/')
    return false;

  if (*(date + 3) < 48 || *(date + 3) > 57)
    return false;
  if (*(date + 4) < 48 || *(date + 4) > 57)
    return false;

  if (*(date + 5) != '/')
    return false;

  if (*(date + 6) < 48 || *(date + 6) > 57)
    return false;
  if (*(date + 7) < 48 || *(date + 7) > 57)
    return false;
  if (*(date + 8) < 48 || *(date + 8) > 57)
    return false;
  if (*(date + 9) < 48 || *(date + 9) > 57)
    return false;
  if (*(date + 10) != '\0')
    return false;

  return true;
}

void updateUser()
{
  TUserList *p = UserList;

  while (p != nullptr)
  {
    if (p->user->id == user1->id)
    {
      p->user->name = strdup(user1->name);
      p->user->country = strdup(user1->country);
      p->user->email = strdup(user1->email);
      p->user->highscoreDate = strdup(user1->highscoreDate);
      p->user->lastname = strdup(user1->lastname);
      p->user->nick = strdup(user1->nick);
      p->user->pass = strdup(user1->pass);
      p->user->prov = strdup(user1->prov);
      p->user->credits = user1->credits;
      p->user->highscore = user1->highscore;
      p->user->isAdmin = user1->isAdmin;
      return;
    }
    p = p->next;
  }
}

// ----------------------------------- Scoreboard -----------------------------------

void freeScoreboard()
{
  while (scoreList != nullptr)
  {
    TScoreboard *p = scoreList;
    scoreList = scoreList->next;
    free(p);
  }
}

void insertScore(TUserList *user)
{
  TScoreboard *newScore = (TScoreboard *)malloc(sizeof(TScoreboard));

  newScore->score = user->user->highscore;
  newScore->username = user->user->nick;
  newScore->date = user->user->highscoreDate;

  newScore->next = scoreList;
  newScore->prev = nullptr;

  if (scoreList != nullptr)
  {
    scoreList->prev = newScore;
  }

  scoreList = newScore;
}

void orderScoreboard()
{
  bool found;
  TScoreboard *p;
  int listLength = UserListLength();

  for (int i = 0; i < listLength - 1; i++)
  {
    found = false;
    p = scoreList;

    for (int j = 0; j < listLength - 1 - i; j++)
    {
      if (p->next != nullptr && p->score < p->next->score)
      {

        int tempScore = p->score;
        char *tempDate = p->date;
        char *tempUsername = p->username;

        p->score = p->next->score;
        p->date = p->next->date;
        p->username = p->next->username;

        p->next->score = tempScore;
        p->next->date = tempDate;
        p->next->username = tempUsername;

        found = true;
      }
      p = p->next;
    }

    if (!found)
      break;
  }
}

void initScoreboard()
{
  freeScoreboard();

  TUserList *p = UserList;

  while (p != nullptr)
  {
    insertScore(p);
    p = p->next;
  }
  orderScoreboard();
  printf("Scoreboard initialized");
}

void paintScore(float y, int score, int order, char *user, char *date)
{

  *(sqPoints + 0) = {50, y};
  *(sqPoints + 1) = {740, y};
  *(sqPoints + 2) = {740, y + 50};
  *(sqPoints + 3) = {50, y + 50};

  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 15);
  esat::DrawSolidPath(&sqPoints[0].x, 4);

  esat::DrawSetFillColor(255, 255, 255, 255);
  esat::DrawSetTextSize(30);
  esat::DrawText(110, y + 35, user);

  itoa(order + 1, TextBuffer, 10);
  esat::DrawText(60, y + 35, TextBuffer);
  itoa(score, TextBuffer, 10);
  esat::DrawText(580, y + 35, TextBuffer);

  esat::DrawSetFillColor(255, 255, 255, 150);
  esat::DrawSetTextSize(20);
  esat::DrawText(443, y + 32, date);
}

void paintScoreboard()
{
  TScoreboard *score = scoreList;
  int i = 0;
  float y;
  while (score != nullptr /* && score->score > 0 */)
  {
    y = 80 + (50 * i) + scrollOffset;
    paintScore(y, score->score, i, score->username, score->date);
    i++;
    score = score->next;
  }
  // Scroll
  *(sqPoints + 0) = {741, 80};
  *(sqPoints + 1) = {760, 80};
  *(sqPoints + 2) = {760, 580};
  *(sqPoints + 3) = {741, 580};
  esat::DrawSetStrokeColor(250, 250, 250, 255);
  esat::DrawSetFillColor(250, 255, 255, 20);
  esat::DrawSolidPath(&sqPoints[0].x, 4);

  if (i > 10)
  {
    ScrollbarController(i, 50, 80, 580, 19, 741);
  }

  // Background
  *(sqPoints + 0) = {0, 0};
  *(sqPoints + 1) = {800, 0};
  *(sqPoints + 2) = {800, 79};
  *(sqPoints + 3) = {0, 79};
  esat::DrawSetStrokeColor(0, 0, 0, 255);
  esat::DrawSetFillColor(0, 0, 0, 255);
  esat::DrawSolidPath(&sqPoints[0].x, 4);
  *(sqPoints + 0) = {0, 581};
  *(sqPoints + 1) = {800, 581};
  *(sqPoints + 2) = {800, 800};
  *(sqPoints + 3) = {0, 800};
  esat::DrawSolidPath(&sqPoints[0].x, 4);
}

bool isTop10HS(int score)
{
  initScoreboard();
  orderScoreboard();
  TScoreboard *p = scoreList;
  int i = 0;

  while (i <= 10)
  {
    if (score > p->score)
    {
      return true;
    }
    p = p->next;
  }
  return false;
}

// ----------------------------------- Inits -----------------------------------

void initAdminButton(TButton **button, zoro::Vec2 pos, zoro::Vec2 dim, char *label)
{
  *button = (TButton *)malloc(1 * sizeof(TButton));

  (*button)->fontSize = 28;
  (*button)->focused = false;
  (*button)->hasLabel = true;
  (*button)->isInput = true;
  (*button)->idFunction = -1;
  (*button)->windowContext = ADMIN;

  (*button)->pos = pos;
  (*button)->dimensions = dim;
  (*button)->label = label;
  (*button)->text = (char *)malloc(40 * sizeof(char));
}

void initAdminInterface()
{
  adminButton = (TButton *)malloc(1 * sizeof(TButton));

  adminButton->fontSize = 40;
  adminButton->focused = false;
  adminButton->hasLabel = false;
  adminButton->isInput = false;
  adminButton->idFunction = 4;
  adminButton->pos = {400, 640};
  adminButton->windowContext = MENU;
  adminButton->dimensions = {400, 40};
  adminButton->text = "admin";

  initAdminButton(&admin_username, {545, 50}, {500, 40}, "USERNAME");
  initAdminButton(&admin_password, {545, 120}, {500, 40}, "PASSWORD");
  initAdminButton(&admin_credits, {545, 190}, {500, 40}, "CREDITS");
  initAdminButton(&admin_email, {545, 260}, {500, 40}, "EMAIL");
  initAdminButton(&admin_name, {545, 330}, {500, 40}, "NAME");
  initAdminButton(&admin_lastname, {545, 400}, {500, 40}, "LAST NAME");
  initAdminButton(&admin_prov, {545, 470}, {500, 40}, "PROVINCE");
  initAdminButton(&admin_country, {545, 540}, {500, 40}, "COUNTRY");
  initAdminButton(&admin_birthday, {545, 610}, {500, 40}, "BIRTHDAY dd/mm/yyyy");
  initAdminButton(&admin_highscoreDate, {545, 680}, {500, 40}, "HIGHSCORE DATE dd/mm/yyyy");
  initAdminButton(&admin_highscore, {545, 750}, {500, 40}, "HIGHSCORE");
}

void init()
{

  esat::DrawSetTextFont("./resources/fonts/Hyperspace.otf");
  shotlist = nullptr;
  InterfaceList = nullptr;

  initAdminInterface();

  // Pointers
  TextBuffer = (char *)malloc(30 * sizeof(char));

  popup.text = nullptr;

  sqPoints = (zoro::Vec2 *)malloc(4 * sizeof(zoro::Vec2));
}

void GoToSignUp()
{

  GAMESTATE = SIGNUP;
  EmptyUserData(tempUser);

  // Cleans inputs
  cleanChar(username_Signup->text);
  cleanChar(password_Signup->text);
  cleanChar(email->text);
  cleanChar(name->text);
  cleanChar(lastname->text);
  cleanChar(prov->text);
  cleanChar(country->text);
  cleanChar(birthday->text);
  cleanChar(highscoreDate->text);
}

void ChangeUserInAdmin(TUser *newtemp)
{
  tempUser = nullptr;
  initEmptyUser(&tempUser);
  tempUser = newtemp;

  // Cleans inputs
  cleanChar(admin_username->text);
  cleanChar(admin_password->text);
  cleanChar(admin_email->text);
  cleanChar(admin_name->text);
  cleanChar(admin_lastname->text);
  cleanChar(admin_prov->text);
  cleanChar(admin_country->text);
  cleanChar(admin_birthday->text);
  cleanChar(admin_highscoreDate->text);
  cleanChar(admin_credits->text);
  cleanChar(admin_highscore->text);

  strcpy(admin_username->text, tempUser->nick);
  strcpy(admin_password->text, tempUser->pass);
  strcpy(admin_email->text, tempUser->email);
  strcpy(admin_name->text, tempUser->name);
  strcpy(admin_prov->text, tempUser->prov);
  strcpy(admin_lastname->text, tempUser->lastname);
  strcpy(admin_country->text, tempUser->country);
  strcpy(admin_birthday->text, tempUser->birthday);
  strcpy(admin_highscoreDate->text, tempUser->highscoreDate);

  itoa(tempUser->credits, TextBuffer, 10);
  strcpy(admin_credits->text, TextBuffer);
  itoa(tempUser->highscore, TextBuffer, 10);
  strcpy(admin_highscore->text, TextBuffer);

  admin_change = tempUser->isAdmin;
}

// ----------------------------------- Ship -----------------------------------

void initShip(TShip *ship)
{
  ship->pos = {400.0f, 400.0f};
  ship->speed = {0.0f, 0.0f};
  ship->acceleration = {0.0f, 0.0f};
  ship->scale = shipScale;
  ship->angle = 0.0f;
  ship->dr_points = (zoro::Vec2 *)malloc(ship->kNPoints * sizeof(zoro::Vec2));
  ship->g_points = (zoro::Vec3 *)malloc(ship->kNPoints * sizeof(zoro::Vec3));

  *(ship->g_points + 0) = {+0.6f, -0.15f, 1.0f};
  *(ship->g_points + 1) = {-0.4f, -0.25f, 1.0f};
  *(ship->g_points + 2) = {+0.25f, -0.05f, 1.0f};
  *(ship->g_points + 3) = {-0.45f, -0.65f, 1.0f};
  *(ship->g_points + 4) = {+0.05f, 0.0f, 1.0f};

  *(ship->g_points + 5) = {+0.6f, +0.15f, 1.0f};
  *(ship->g_points + 6) = {-0.4f, +0.25f, 1.0f};
  *(ship->g_points + 7) = {+0.25f, +0.05f, 1.0f};
  *(ship->g_points + 8) = {-0.45f, +0.65f, 1.0f};
  *(ship->g_points + 9) = {+0.05f, 0.0f, 1.0f};

  ship->dr_Fuelpoints = (zoro::Vec2 *)malloc(3 * sizeof(zoro::Vec2));
  ship->g_Fuelpoints = (zoro::Vec3 *)malloc(3 * sizeof(zoro::Vec3));
  *(ship->g_Fuelpoints + 0) = {-0.04f, +0.04f, 1.0f};
  *(ship->g_Fuelpoints + 1) = {-0.04f, -0.04f, 1.0f};
  *(ship->g_Fuelpoints + 2) = {-0.8f, +0.0f, 1.0f};
}

void initEnemyShip(TShip *ship)
{
  ship->pos = {400.0f, 400.0f};
  ship->speed = {0.0f, 0.0f};
  ship->acceleration = {0.0f, 0.0f};
  ship->scale = shipScale;
  ship->angle = 0.0f;
  ship->kNPoints = 15; // important!
  ship->dr_points = (zoro::Vec2 *)malloc(ship->kNPoints * sizeof(zoro::Vec2));
  ship->g_points = (zoro::Vec3 *)malloc(ship->kNPoints * sizeof(zoro::Vec3));

  *(ship->g_points + 0) = {+0.8f, +0.15f, 1.0f};
  *(ship->g_points + 1) = {0.0f, -0.2f, 1.0f};
  *(ship->g_points + 2) = {-0.6f, +0.85f, 1.0f};
  *(ship->g_points + 3) = {-0.45f, -0.65f, 1.0f};
  *(ship->g_points + 4) = {+0.05f, 0.0f, 1.0f};
  *(ship->g_points + 5) = {+0.6f, +0.15f, 1.0f};

  *(ship->g_points + 6) = {-0.4f, +0.25f, 1.0f};
  
  ship->dr_Fuelpoints = (zoro::Vec2 *)malloc(3 * sizeof(zoro::Vec2));
  ship->g_Fuelpoints = (zoro::Vec3 *)malloc(3 * sizeof(zoro::Vec3));
  *(ship->g_Fuelpoints + 0) = {-0.04f, +0.04f, 1.0f};
  *(ship->g_Fuelpoints + 1) = {-0.04f, -0.04f, 1.0f};
  *(ship->g_Fuelpoints + 2) = {-0.8f, +0.0f, 1.0f};
}

void paintShip(TShip p_ship)
{

  p_ship.M = zoro::MatIdentity3();
  p_ship.M = zoro::Mat3Multiply(zoro::Mat3Translate(p_ship.pos), p_ship.M);
  p_ship.M = zoro::Mat3Multiply(zoro::Mat3Scale(p_ship.scale, p_ship.scale), p_ship.M);
  p_ship.M = zoro::Mat3Multiply(zoro::Mat3Rotate(p_ship.angle), p_ship.M);

  for (int i = 0; i < p_ship.kNPoints; i++)
  {
    zoro::Vec3 tmp = zoro::Mat3TransformVec3(p_ship.M, *(p_ship.g_points + i));
    *(p_ship.dr_points + i) = {tmp.x, tmp.y};
  }

  esat::DrawSetStrokeColor(20, 20, 255, 255);
  esat::DrawSetFillColor(20, 20, 20, 20);
  //esat::DrawSolidPath(&p_ship.dr_points[0].x, 2);
  for (int j = 0; j < 2; j++)
  {
    esat::DrawLine(p_ship.dr_points[0+ (j*5)].x, p_ship.dr_points[0 + (j*5)].y, p_ship.dr_points[1+ (j*5)].x, p_ship.dr_points[1+ (j*5)].y);
    esat::DrawLine(p_ship.dr_points[2 + (j*5)].x, p_ship.dr_points[2 + (j*5)].y, p_ship.dr_points[ 3 + (j*5)].x, p_ship.dr_points[3+ (j*5)].y);
    esat::DrawLine(p_ship.dr_points[3 + (j*5)].x, p_ship.dr_points[3 + (j*5)].y, p_ship.dr_points[ 4 + (j*5)].x, p_ship.dr_points[4+ (j*5)].y);
  }

  // esat::DrawSolidPath(&p_ship.dr_points[2].x, 3);
  // esat::DrawSolidPath(&p_ship.dr_points[5].x, 1);
  // esat::DrawSolidPath(&p_ship.dr_points[7].x, 3);

  if (p_ship.IsPressing && esat::Time() - sprFuelTime > sprFuelTimeRef)
  {
    p_ship.isPaintingFuel = true;
  }

  if (p_ship.isPaintingFuel)
  {
    if (esat::Time() - sprFuelTime > sprFuelTimeRef)
    {
      p_ship.isPaintingFuel = false;
      sprFuelTime = esat::Time();
    }

    for (int i = 0; i < 3; i++)
    {
      zoro::Vec3 tmp = zoro::Mat3TransformVec3(p_ship.M, *(p_ship.g_Fuelpoints + i));
      *(p_ship.dr_Fuelpoints + i) = {tmp.x, tmp.y};
    }
    esat::DrawSolidPath(&p_ship.dr_Fuelpoints[0].x, 3);
  }
}

void shipDeath()
{
  ship.lives--;
  if (ship.lives <= 0)
  {
    isNewTop10 = false;
    GAMESTATE = GAMEOVER;

    if (ship.score > user1->highscore)
    {
      isNewTop10 = isTop10HS(ship.score);
      user1->credits += 5;
      user1->highscore = ship.score;
      cleanChar(highscoreDate->text);
      user1->highscoreDate = highscoreDate->text;
    }
  }
  else
  {
    ship.isDying = true;
    ShipDyingAnimTime = esat::Time();
  }
}

bool shipPointColision(zoro::Vec2 pos)
{
  zoro::Vec2 a, b;
  zoro::Vec3 temp;

  int counter = 0;
  for (int i = 0; i < 3; i++)
  {
    switch (i)
    {
    case 0:
      a = {(ship.g_points + 0)->x, (ship.g_points + 0)->y};
      b = {(ship.g_points + 1)->x, (ship.g_points + 1)->y};
      break;
    case 1:
      a = {(ship.g_points + 1)->x, (ship.g_points + 1)->y};
      b = {(ship.g_points + 4)->x, (ship.g_points + 4)->y};
      break;
    case 2:
      a = {(ship.g_points + 4)->x, (ship.g_points + 4)->y};
      b = {(ship.g_points + 0)->x, (ship.g_points + 0)->y};
      break;
    default:
      break;
    }

    temp = zoro::Mat3TransformVec3(ship.M, {a.x, a.y, 1.0f});
    a = {temp.x, temp.y};
    temp = zoro::Mat3TransformVec3(ship.M, {b.x, b.y, 1.0f});
    b = {temp.x, temp.y};

    // esat::DrawLine(a.x, a.y, pos.x, pos.y);
    float value = DotVec2(zoro::SubtractVec2(pos, a), zoro::RightPerpendicularVec2(zoro::NormalizeVec2(zoro::SubtractVec2(b, a))));

    if (value < 0.0f)
    {
      counter++;
    }
  }

  if (counter == 3)
  {
    return true;
  }
  return false;
}

void shipColision()
{

  zoro::Vec2 shipPoint;
  zoro::Vec3 gshipPoint;

  TShot *s = shotlist;
  bool found = false;

  zoro::Vec2 a, b, pos;
  zoro::Vec3 temp;

  ship.M = zoro::MatIdentity3();
  ship.M = zoro::Mat3Multiply(zoro::Mat3Translate(ship.pos), ship.M);
  ship.M = zoro::Mat3Multiply(zoro::Mat3Scale(ship.scale, ship.scale), ship.M);
  ship.M = zoro::Mat3Multiply(zoro::Mat3Rotate(ship.angle), ship.M);

  for (int i = 0; i < ship.kNPoints; i++)
  {
    zoro::Vec3 tmp = zoro::Mat3TransformVec3(ship.M, *(ship.g_points + i));
    *(ship.dr_points + i) = {tmp.x, tmp.y};
  }

  // Bullet colision with ship
  while (s != nullptr)
  {
    if (s->isEnemy)
    {
      if (shipPointColision(s->pos))
      {
        deleteShot(s);
        shipDeath();
        return;
      }
    }
    s = s->next;
  }
}

void movementShip()
{
  if (ship.pos.x > 800)
    ship.pos.x = 1;
  if (ship.pos.x < 0)
    ship.pos.x = 799;
  if (ship.pos.y > 800)
    ship.pos.y = 1;
  if (ship.pos.y < 0)
    ship.pos.y = 799;

  ship.speed = zoro::SumVec2(zoro::ScaleVec2(ship.speed, 0.988), ship.acceleration);
  ship.pos = zoro::SumVec2(ship.pos, ship.speed);
}

void shipManager()
{
  if (!ship.isDying)
    movementShip();
  if (!ship.isInvincible && !ship.isDying)
  {
    shipColision();
  }

  if (ship.isDying && esat::Time() - ShipDyingAnimTime > ShipDyingAnimTimeRef)
  {
    ship.isDying = false;
    ship.isInvincible = true;
    InvincibleTime = esat::Time();
    ship.pos = {400.0f, 400.0f};
    ship.speed = {0.0f, 0.0f};
  }

  if (ship.isInvincible)
  {
    if (esat::Time() - InvincibleTime > InvincibleTimeRef)
    {
      ship.isInvincible = false;
      InvincibleTime = esat::Time();
    }

    if (esat::Time() - PaintTime > PaintTimeRef)
    {
      paintShip(ship);
      PaintTime = esat::Time();
    }
  }
  else
  {
    if (!ship.isDying)
    {
      paintShip(ship);
    }
  }
}

// ----------------------------------- Shots -----------------------------------

void addShot(TShot s)
{
  // Allocates memory for shot based on s
  TShot *newShot = (TShot *)malloc(1 * sizeof(TShot));

  newShot->dir = s.dir;
  newShot->pos = s.pos;
  if (s.isEnemy)
  {
    newShot->speed = shotSpeed;
  }
  else
  {
    LastShotTime = esat::Time();
    newShot->speed = shotSpeed + zoro::MagnitudeVec2(ship.speed);
  }

  newShot->isEnemy = s.isEnemy;
  newShot->active = true;
  newShot->next = nullptr;
  newShot->prev = nullptr;
  newShot->travelDistance = 0.0f;

  // Adds it to list
  if (shotlist == nullptr)
    shotlist = newShot;
  else
  {
    newShot->next = shotlist;
    shotlist->prev = newShot;
    shotlist = newShot;
  }
}

void emptyShotList()
{
  TShot *s = shotlist;
  TShot *p = nullptr;
  while (s != nullptr)
  {
    p = s->next;
    deleteShot(s);
    s = p;
  }
}

void deleteShot(TShot *s)
{

  if (shotlist == nullptr || s == nullptr)
    return;

  if (shotlist == s)
    shotlist = s->next;

  if (s->next != nullptr)
    s->next->prev = s->prev;

  if (s->prev != nullptr)
    s->prev->next = s->next;

  free(s);
  s = nullptr;
}

void moveShots()
{
  TShot *p = shotlist;

  while (p != nullptr)
  {
    if (p->travelDistance > shotMaxDistance)
    {
      // printf("Ok ");
      p->active = false;
      deleteShot(p);
    }
    else
    {
      p->pos = zoro::SumVec2(p->pos, zoro::ScaleVec2(p->dir, p->speed));
      p->travelDistance += zoro::MagnitudeVec2(zoro::ScaleVec2(p->dir, p->speed));

      if (p->pos.x > 800)
      {
        p->pos.x = 1;
      }
      if (p->pos.x < 0)
      {
        p->pos.x = 800;
      }
      if (p->pos.y > 800)
      {
        p->pos.y = 1;
      }
      if (p->pos.y < 0)
      {
        p->pos.y = 800;
      }
    }
    p = p->next;
  }
}

void paintShots()
{
  TShot *p = shotlist;

  while (p != nullptr)
  {

    // DRAW
    *(sqPoints + 0) = p->pos;
    *(sqPoints + 1) = zoro::SumVec2(*(sqPoints + 0), zoro::ScaleVec2(zoro::RightPerpendicularVec2(zoro::NormalizeVec2(p->dir)), 3));
    *(sqPoints + 2) = zoro::SumVec2(*(sqPoints + 1), zoro::ScaleVec2(zoro::NormalizeVec2(p->dir), -4));
    *(sqPoints + 3) = zoro::SumVec2(*(sqPoints + 0), zoro::ScaleVec2(zoro::NormalizeVec2(p->dir), -4));
    esat::DrawSetStrokeColor(0, 0, 0, 0);
    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSolidPath(&sqPoints[0].x, 4);

    if (!p->isEnemy)
    {
      for (int i = 1; i < 20; i++)
      {

        // DRAW
        *(sqPoints + 0) = zoro::SubtractVec2(p->pos, zoro::ScaleVec2(zoro::NormalizeVec2(p->dir), 3 * i));
        *(sqPoints + 1) = zoro::SumVec2(*(sqPoints + 0), zoro::ScaleVec2(zoro::RightPerpendicularVec2(zoro::NormalizeVec2(p->dir)), 3));
        *(sqPoints + 2) = zoro::SumVec2(*(sqPoints + 1), zoro::ScaleVec2(zoro::NormalizeVec2(p->dir), -3));
        *(sqPoints + 3) = zoro::SumVec2(*(sqPoints + 0), zoro::ScaleVec2(zoro::NormalizeVec2(p->dir), -3));

        esat::DrawSetStrokeColor(0, 0, 0, 0);
        esat::DrawSetFillColor(255, 255, 255, 80 - ((80 * i) / 30));
        esat::DrawSolidPath(&sqPoints[0].x, 4);
        if (zoro::MagnitudeVec2(zoro::SubtractVec2(p->pos, ship.pos)) < i + 50)
          i = 21;
      }
    }

    p = p->next;
  }
}

void shotsManager()
{

  moveShots();

  paintShots();
}

// ----------------------------------- Game UI -----------------------------------

void paintGUI()
{
  /*
  esat::DrawSetTextSize(40);
    esat::DrawSetFillColor(230, 230, 230, 255);
    itoa(ship.score, TextBuffer, 10);
    esat::DrawText(150, 50, TextBuffer);

    for (int i = 0; i < ship.lives; i++)
    {
      zoro::Mat3 m = zoro::MatIdentity3();
      m = zoro::Mat3Multiply(zoro::Mat3Translate({(i * 20) + 40.0f, 100.0f}), m);
      m = zoro::Mat3Multiply(zoro::Mat3Scale(20, 20), m);
      m = zoro::Mat3Multiply(zoro::Mat3Rotate(-3.14f / 2), m);

      for (int i = 0; i < ship.kNPoints; i++)
      {
        zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *(ship.g_points + i));
        *(ship.dr_points + i) = {tmp.x, tmp.y};
      }
      esat::DrawSetStrokeColor(255,255,255, 255);
      esat::DrawSetFillColor(20,20,20, 20);
      esat::DrawSolidPath(&ship.dr_points[0].x, ship.kNPoints);
    }
  */
}

// ----------------------------------- Interface -----------------------------------

bool checkMouseClick(float minx, float maxx, float miny, float maxy, int mouseType)
{
  float mx, my;
  mx = esat::MousePositionX();
  my = esat::MousePositionY();
  if (mx > minx && mx < maxx && my > miny && my < maxy)
  {
    if (mouseType == -1)
      return true;

    if (esat::MouseButtonDown(mouseType))
      return true;
  }

  return false;
}

void resetPopup()
{
  popup.Activate = false;
  popup.activeTime = 0.0f;
  popup.isActive = false;
  popup.isEntering = false;
  popup.isOutering = false;
  popup.maxActiveTime = 0.0f;
}

void callButtonFunction(int id)
{
  switch (id)
  {
  case 0:
    isLogged = CheckLogin(user1);
    if (isLogged)
    {
      GAMESTATE = MENU;
    }
    else
    {
      popup.Activate = true;
      popup.maxActiveTime = 2000;
      popup.text = "Credentials are incorect:(";
    }
    break;
  case 1:
    scrollOffset = 0;
    GAMESTATE = SCOREBOARD;
    initScoreboard();
    break;
  case 2:
    GAMESTATE = CREDITS;
    break;
  case 3:
    GAMESTATE = MENU;
    break;
  case 4:
    GAMESTATE = ADMIN;
    ChangeUserInAdmin(user1);
    scrollOffset = 0;
    break;
  case 5:

    if (isLogged)
    {
      if (user1->credits > 0)
      {
        startNewGame();
      }
      else
      {
        popup.Activate = true;
        popup.maxActiveTime = 2000;
        popup.text = "Not enough credits";
      }
    }
    else
    {
      GAMESTATE = LOGIN;
    }

    break;
  case 6:
    GoToSignUp();

    break;
  case 7:
    // SignUp

    tempUser->nick = strdup(username_Signup->text);
    tempUser->pass = strdup(password_Signup->text);
    tempUser->name = strdup(name->text);
    tempUser->lastname = strdup(lastname->text);
    tempUser->email = strdup(email->text);
    tempUser->prov = strdup(prov->text);
    tempUser->country = strdup(country->text);
    tempUser->birthday = strdup(birthday->text);

    printf("tempuser:\n");
    PrintUser(tempUser);
    printf("......................................\n");

    if (!checkUsernameValid(tempUser))
    {
      popup.Activate = true;
      popup.maxActiveTime = 3000;
      popup.text = "Username already exists";
      return;
    }
    if (!checkDate(tempUser->birthday))
    {
      popup.Activate = true;
      popup.maxActiveTime = 2000;
      popup.text = "Date format incorrect";
      return;
    }

    if (!checkIfUserDataIsFull(tempUser))
    {
      popup.Activate = true;
      popup.maxActiveTime = 3000;
      popup.text = "Fill all the data";
      return;
    }

    if (strcmp(tempUser->nick, "ADMIN") == 0)
    {
      tempUser->isAdmin = true;
    }

    insertUser(tempUser, true);
    SaveDataUser();
    tempUser = nullptr;
    initEmptyUser(&tempUser);
    GAMESTATE = MENU;
    break;
  case 8:
    if (checkDate(highscoreDate->text))
    {
      GAMESTATE = SCOREBOARD;
      initScoreboard();
      SaveDataUser();
    }
    else
    {
      popup.Activate = true;
      popup.maxActiveTime = 2000;
      popup.text = "Date not correct :(";
    }

    break;
  case 9:
    if (checkDate(highscoreDate->text))
    {
      GAMESTATE = MENU;
      SaveDataUser();
    }
    else
    {
      popup.Activate = true;
      popup.maxActiveTime = 2000;
      popup.text = "Date not correct :(";
    }

    break;
  case 10:

    SaveAdminEdit();
    SaveDataUser();
    popup.Activate = true;
    popup.maxActiveTime = 1000;
    popup.text = "Saved :)";
    break;
  default:
    break;
  }
}

TButton *addButtonToList(TButton b)
{
  // Creates new button
  TButton *newbutton = (TButton *)malloc(1 * sizeof(TButton));

  newbutton->pos = b.pos;
  newbutton->dimensions = b.dimensions;
  newbutton->idFunction = b.idFunction;
  newbutton->windowContext = b.windowContext;
  newbutton->fontSize = b.fontSize;
  newbutton->isInput = b.isInput;
  newbutton->label = b.label;
  newbutton->hasLabel = b.hasLabel;

  if (b.isInput) // If is input allocates memory for the text
  {
    newbutton->text = (char *)malloc(40 * sizeof(char));
  }
  else
  {
    newbutton->text = b.text;
  }

  newbutton->focused = false;
  // Adds button to list.
  newbutton->next = InterfaceList;
  InterfaceList = newbutton;
  return newbutton;
}

void initInterfaceData()
{

  TButton newButton;
  // newButton.text = (char *)malloc(40 * sizeof(char)); // 40 chars maximum

  newButton.fontSize = 40;
  newButton.focused = false;
  newButton.hasLabel = false;

  // Buttons
  newButton.isInput = false;

  newButton.text = "start";
  newButton.idFunction = 5;
  newButton.pos = {400, 400};
  newButton.windowContext = MENU;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "scoreboard";
  newButton.idFunction = 1;
  newButton.pos = {400, 480};
  newButton.windowContext = MENU;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "credits";
  newButton.idFunction = 2;
  newButton.pos = {400, 560};
  newButton.windowContext = MENU;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "MENU";
  newButton.idFunction = 3;
  newButton.pos = {400, 640};
  newButton.windowContext = SCOREBOARD;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "go back";
  newButton.idFunction = 3;
  newButton.pos = {400, 640};
  newButton.windowContext = CREDITS;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "SAVE";
  newButton.idFunction = 10;
  newButton.pos = {145, 720};
  newButton.windowContext = ADMIN;
  newButton.dimensions = {280, 40};

  addButtonToList(newButton);

  newButton.text = "go back";
  newButton.idFunction = 3;
  newButton.pos = {145, 775};
  newButton.windowContext = ADMIN;
  newButton.dimensions = {280, 40};

  addButtonToList(newButton);

  newButton.text = "go back";
  newButton.idFunction = 3;
  newButton.pos = {400, 640};
  newButton.windowContext = LOGIN;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "Sign Up";
  newButton.idFunction = 7;
  newButton.pos = {400, 720};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "go back";
  newButton.idFunction = 5;
  newButton.pos = {400, 760};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  /*
  newButton.text = "admin";
    newButton.idFunction = 4;
    newButton.pos = {400, 640};
    newButton.windowContext = MENU;
    newButton.dimensions = {400, 40};

    addButtonToList(newButton);
  */

  newButton.text = "login";
  newButton.idFunction = 0;
  newButton.pos = {400, 480};
  newButton.windowContext = LOGIN;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "sign up";
  newButton.idFunction = 6;
  newButton.pos = {400, 560};
  newButton.windowContext = LOGIN;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "SCOREBOARD";
  newButton.idFunction = 8;
  newButton.pos = {400, 720};
  newButton.windowContext = GAMEOVER;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  newButton.text = "MENU";
  newButton.idFunction = 3;
  newButton.pos = {400, 760};
  newButton.windowContext = GAMEOVER;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

  // Text Inputs

  newButton.isInput = true;
  newButton.idFunction = -1;
  newButton.hasLabel = true;

  // Login Username
  newButton.pos = {400, 240};
  newButton.windowContext = LOGIN;
  newButton.dimensions = {610, 40};
  newButton.label = "Username";
  username = addButtonToList(newButton);
  user1->nick = username->text;
  cleanChar(user1->nick);

  // Login Pass
  newButton.pos = {400, 360};
  newButton.windowContext = LOGIN;
  newButton.dimensions = {610, 40};
  newButton.label = "Password";
  password = addButtonToList(newButton);
  user1->pass = password->text;
  cleanChar(user1->pass);

  // Sign Up Username
  newButton.pos = {400, 80};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "Username";
  username_Signup = addButtonToList(newButton);

  // Sign Up Pass
  newButton.pos = {400, 160};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "Password";
  password_Signup = addButtonToList(newButton);

  // Name
  newButton.pos = {400, 240};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "Name";
  name = addButtonToList(newButton);

  // Lastname
  newButton.pos = {400, 320};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "LastName";
  lastname = addButtonToList(newButton);

  // Email
  newButton.pos = {400, 400};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "Email";
  email = addButtonToList(newButton);

  // Prov
  newButton.pos = {400, 480};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "Province";
  prov = addButtonToList(newButton);

  // Country
  newButton.pos = {400, 560};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "Country";
  country = addButtonToList(newButton);

  // Birthday
  newButton.pos = {400, 640};
  newButton.windowContext = SIGNUP;
  newButton.dimensions = {610, 40};
  newButton.label = "birthday dd/mm/yyyy";
  birthday = addButtonToList(newButton);

  // Highscore Date
  newButton.pos = {400, 640};
  newButton.windowContext = GAMEOVER;
  newButton.dimensions = {410, 40};
  newButton.label = "date dd/mm/yyyy";
  highscoreDate = addButtonToList(newButton);
}

void paintButton(TButton *p)
{
  zoro::Vec2 finalTextPos;
  float mx, my;
  *(sqPoints + 0) = p->pos;
  (*(sqPoints + 0)).x -= p->dimensions.x / 2;
  (*(sqPoints + 0)).y -= p->dimensions.y / 2;

  *(sqPoints + 1) = p->pos;
  (*(sqPoints + 1)).x += p->dimensions.x / 2;
  (*(sqPoints + 1)).y -= p->dimensions.y / 2;

  *(sqPoints + 2) = p->pos;
  (*(sqPoints + 2)).x += p->dimensions.x / 2;
  (*(sqPoints + 2)).y += p->dimensions.y / 2;

  *(sqPoints + 3) = p->pos;
  (*(sqPoints + 3)).x -= p->dimensions.x / 2;
  (*(sqPoints + 3)).y += p->dimensions.y / 2;

  mx = esat::MousePositionX();
  my = esat::MousePositionY();

  if (lookingNextButton)
  {
    p->focused = true;
    currentFocus = p;
    lookingNextButton = false;
  }

  if ((mx <= (*(sqPoints + 1)).x && mx >= (*(sqPoints + 0)).x) &&
      (my <= (*(sqPoints + 2)).y && my >= (*(sqPoints + 0)).y))
  {
    if (esat::MouseButtonUp(0))
    {
      esat::ResetBufferdKeyInput();
      if (p->isInput)
      {
        p->focused = true;
        currentFocus = p;
      }
      else if (esat::Time() - MenuCooldownTime > MenuCooldownTimeRef)
      {
        callButtonFunction(p->idFunction);
        MenuCooldownTime = esat::Time();
      }
    }

    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);
    esat::DrawSolidPath(&sqPoints[0].x, 4);

    esat::DrawSetFillColor(0, 0, 0, 255);
  }
  else
  {

    if (p->focused)
    {
      esat::DrawSetFillColor(255, 255, 255, 200);
      esat::DrawSetStrokeColor(255, 255, 255, 255);
      esat::DrawSolidPath(&sqPoints[0].x, 4);

      esat::DrawSetFillColor(0, 0, 0, 255);
    }
    else
    {
      esat::DrawSetFillColor(255, 255, 255, 10);
      esat::DrawSetStrokeColor(255, 255, 255, 255);
      esat::DrawSolidPath(&sqPoints[0].x, 4);

      esat::DrawSetFillColor(255, 255, 255, 255);
    }
  }
  if (p->text != nullptr)
  {
    esat::DrawSetTextSize(p->fontSize);

    finalTextPos.y = p->pos.y + (p->dimensions.y / 2); // 1 fontsize = 0.75pixels
    finalTextPos.y -= (p->dimensions.y - p->fontSize * 0.75) / 2;

    finalTextPos.x = p->pos.x - (p->dimensions.x / 2);
    finalTextPos.x += (p->dimensions.x - strlen(p->text) * ((p->fontSize / 2) + 4)) / 2; // widthLetter = (fontsize / 2) + 4

    esat::DrawText(finalTextPos.x, finalTextPos.y, p->text);

    if (p->hasLabel)
    {
      esat::DrawSetFillColor(255, 255, 255, 255);
      esat::DrawSetTextSize(p->fontSize / 2);
      finalTextPos.y = p->pos.y - (p->dimensions.y / 2) - 4;
      finalTextPos.x = p->pos.x - (p->dimensions.x / 2);
      esat::DrawText(finalTextPos.x, finalTextPos.y, p->label);
    }
  }
  else
  {
    printf("Button has no text memory");
  }
}

void ScrollbarController(int i, float boxheight, float min_height, float max_height, float width, float xpos)
{
  // i * 50 = altura total scorelist
  // 500 = altura max scroller
  float scrollRange = max_height - min_height;
  float contentSize = (i * boxheight);

  float scrollerHeight = scrollRange * (scrollRange / contentSize);

  if (esat::MousePositionX() < xpos + width && esat::MousePositionX() > xpos && esat::MousePositionY() > min_height && esat::MousePositionY() < max_height && esat::MouseButtonDown(0))
  {
    mouseIsDown = true;
    mouseDownPos = {(float)esat::MousePositionX(), (float)esat::MousePositionY()};
  }

  if (esat::MouseButtonUp(0))
    mouseIsDown = false;

  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
  {
    scrollOffset -= 4;
  }
  else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
  {
    scrollOffset += 4;
  }

  if (mouseIsDown)
  {
    scrollOffset += mouseDownPos.y - esat::MousePositionY();
    mouseDownPos.y = esat::MousePositionY();
  }

  if (scrollOffset < -(contentSize - scrollRange))
  {
    scrollOffset = -(contentSize - scrollRange);
  }
  if (scrollOffset > 0)
  {
    scrollOffset = 0;
  }

  float scrollerOffsethandle = (scrollOffset * scrollRange) / contentSize;

  *(sqPoints + 0) = {xpos, min_height - scrollerOffsethandle};
  *(sqPoints + 1) = {xpos + width, min_height - scrollerOffsethandle};
  *(sqPoints + 2) = {xpos + width, min_height + scrollerHeight - scrollerOffsethandle};
  *(sqPoints + 3) = {xpos, min_height + scrollerHeight - scrollerOffsethandle};

  esat::DrawSetStrokeColor(250, 250, 250, 255);
  esat::DrawSetFillColor(250, 250, 250, 180);
  if (mouseIsDown)
    esat::DrawSetFillColor(210, 210, 250, 180);
  esat::DrawSolidPath(&sqPoints[0].x, 4);
}

void PaintUser(float y, TUserList *pUser, bool selected)
{
  *(sqPoints + 0) = {5, y};
  *(sqPoints + 1) = {270, y};
  *(sqPoints + 2) = {270, y + 40};
  *(sqPoints + 3) = {5, y + 40};

  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 15);
  esat::DrawSetTextSize(20);

  if (esat::MousePositionX() < 270 && esat::MousePositionY() > y && esat::MousePositionY() < y + 40 && esat::MousePositionY() < 620 && esat::MouseButtonDown(0))
  {
    ChangeUserInAdmin(pUser->user);
  }

  if (selected)
    esat::DrawSetFillColor(255, 255, 255, 250);

  esat::DrawSolidPath(&sqPoints[0].x, 4);

  esat::DrawSetFillColor(255, 255, 255, 255);

  if (selected)
    esat::DrawSetFillColor(0, 0, 0, 255);

  esat::DrawText(50, y + 28, pUser->user->nick);

  itoa(pUser->user->id, TextBuffer, 10);
  esat::DrawText(10, y + 28, TextBuffer);
}

void AdminUserScrollManager()
{

  TUserList *ulist = UserList;
  int i = 0;

  while (ulist != nullptr)
  {
    if (tempUser->id == ulist->user->id)
      PaintUser(10 + (i * 40) + scrollOffset, ulist, true);
    else
      PaintUser(10 + (i * 40) + scrollOffset, ulist, false);

    ulist = ulist->next;
    i++;
  }

  // Scrollbar square
  (*(sqPoints + 0)) = {270, 10};
  (*(sqPoints + 1)) = {285, 10};
  (*(sqPoints + 2)) = {285, 620};
  (*(sqPoints + 3)) = {270, 620};

  esat::DrawSetFillColor(255, 255, 255, 0);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSolidPath(&sqPoints[0].x, 4);

  if (i > 15)
  {
    // Scrollbar
    ScrollbarController(i, 40, 10, 620, 15, 270);

    // Black background to limit what its shown.

    esat::DrawSetFillColor(0, 0, 0, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 0);

    (*(sqPoints + 0)) = {0, 621};
    (*(sqPoints + 1)) = {290, 621};
    (*(sqPoints + 2)) = {290, 800};
    (*(sqPoints + 3)) = {0, 800};

    esat::DrawSolidPath(&sqPoints[0].x, 4);

    (*(sqPoints + 0)) = {0, 0};
    (*(sqPoints + 1)) = {290, 0};
    (*(sqPoints + 2)) = {290, 9};
    (*(sqPoints + 3)) = {0, 9};

    esat::DrawSolidPath(&sqPoints[0].x, 4);
  }
}

void paintAdmin()
{

  paintButton(admin_username);
  paintButton(admin_password);
  paintButton(admin_email);
  paintButton(admin_name);
  paintButton(admin_prov);
  paintButton(admin_lastname);
  paintButton(admin_country);
  paintButton(admin_birthday);
  paintButton(admin_highscoreDate);
  paintButton(admin_credits);
  paintButton(admin_highscore);

  esat::DrawSetStrokeColor(255, 255, 255, 100);
  esat::DrawLine(290, 0, 290, 800);

  // Scroll of users
  AdminUserScrollManager();

  esat::DrawSetFillColor(255, 255, 255, 0);
  esat::DrawSetStrokeColor(255, 255, 255, 255);

  // IS ADMIN CHECKBOX
  (*(sqPoints + 0)) = {5, 630};
  (*(sqPoints + 1)) = {285, 630};
  (*(sqPoints + 2)) = {285, 670};
  (*(sqPoints + 3)) = {5, 670};

  esat::DrawSolidPath(&sqPoints[0].x, 4);

  (*(sqPoints + 0)) = {245, 635};
  (*(sqPoints + 1)) = {280, 635};
  (*(sqPoints + 2)) = {280, 665};
  (*(sqPoints + 3)) = {245, 665};

  if (checkMouseClick(245, 280, 635, 665, -1))
  {
    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);
    if (esat::MouseButtonDown(0))
    {
      tempUser->isAdmin = !tempUser->isAdmin;
      admin_change = tempUser->isAdmin;
    }
  }

  esat::DrawSolidPath(&sqPoints[0].x, 4);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  if (tempUser->isAdmin)
  {
    if (checkMouseClick(245, 280, 635, 665, -1))
      esat::DrawSetStrokeColor(0, 0, 0, 255);

    esat::DrawLine(248, 655, 255, 663);
    esat::DrawLine(255, 663, 273, 638);
  }

  esat::DrawSetTextSize(35);
  esat::DrawSetFillColor(255, 255, 255, 255);
  esat::DrawText(10, 664, "IS ADMIN");

  if (esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
  {
    int id = tempUser->id;
    popup.Activate = true;
    popup.maxActiveTime = 2000;
    if (id != user1->id)
    {
      deleteUser(id);
      ChangeUserInAdmin(user1);
      popup.text = "User deleted succesfully";
      SaveDataUser();
    }
    else
    {
      popup.text = "ERROR CANNOT DELETE USER WHILE LOGGED IN";
    }
  }
}

void paintMenu()
{

  TButton *p = InterfaceList;
  while (p != nullptr)
  {
    if (GAMESTATE == p->windowContext)
    {
      paintButton(p);
    }

    p = p->next;
  }
}

void paintPopup()
{
  if (popup.Activate && !popup.isActive)
  {
    popup.Activate = false;
    popup.activeTime = esat::Time();
    popup.isEntering = true;
    popup.isActive = true;
    popup.isOutering = false;
  }

  if (!popup.isActive)
    return;

  float widthText = strlen(popup.text) * ((25 / 2) + 4);
  float xpos;
  float start;
  float end;
  float t;

  if (popup.isEntering)
  {
    if (esat::Time() - popup.activeTime > 250)
    {
      popup.activeTime = esat::Time();
      popup.isEntering = false;
    }

    // start + (end - start) * t
    start = 801;
    end = 801 - widthText - 10;
    t = (((float)esat::Time() - popup.activeTime) * 0.004f);

    xpos = start + (end - start) * t;

    *(sqPoints + 0) = {xpos, 50};
    *(sqPoints + 1) = {xpos, 100};
    *(sqPoints + 2) = {xpos + widthText + 10, 100};
    *(sqPoints + 3) = {xpos + widthText + 10, 50};
  }
  else if (popup.isOutering)
  {
    if (esat::Time() - popup.activeTime > 250)
    {
      popup.isOutering = false;
      popup.isActive = false;
    }

    // start + (end - start) * t
    start = 801 - widthText - 10;
    end = 801;
    t = (((float)esat::Time() - popup.activeTime) * 0.004f);

    xpos = start + (end - start) * t;

    *(sqPoints + 0) = {xpos, 50};
    *(sqPoints + 1) = {xpos, 100};
    *(sqPoints + 2) = {xpos + widthText + 10, 100};
    *(sqPoints + 3) = {xpos + widthText + 10, 50};
  }
  else
  {

    if (esat::Time() - popup.activeTime > popup.maxActiveTime)
    {
      popup.activeTime = esat::Time();
      popup.isOutering = true;
    }

    xpos = 801 - widthText;

    *(sqPoints + 0) = {xpos, 50};
    *(sqPoints + 1) = {xpos, 100};
    *(sqPoints + 2) = {800, 100};
    *(sqPoints + 3) = {800, 50};
  }

  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 250);
  esat::DrawSolidPath(&sqPoints[0].x, 4);

  esat::DrawSetTextSize(25);
  esat::DrawSetFillColor(0, 0, 0, 255);
  esat::DrawText(xpos + 10, 85, popup.text);
}

void interfaceManager()
{
  paintMenu();
  paintPopup();
}

// ----------------------------------- Input -----------------------------------

void input()
{

  if (GAMESTATE == INGAME)
  {
    ship.acceleration = {0.0f, 0.0f};

    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
    {
      ship.acceleration.x = cos(ship.angle) * 0.19;
      ship.acceleration.y = sin(ship.angle) * 0.19;
      ship.IsPressing = !ship.IsPressing;
    }
    if (esat::IsSpecialKeyUp(esat::kSpecialKey_Up))
    {
      ship.IsPressing = false;
    }

    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left))
    {
      ship.angle -= 0.075;
    }
    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right))
    {
      ship.angle += 0.075;
    }
    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
    {
      ship.acceleration.x = -cos(ship.angle) * 0.05;
      ship.acceleration.y = -sin(ship.angle) * 0.05;
    }

    if (esat::IsKeyDown('H'))
    {
      ship.pos.x = 25.0f + rand() % 750;
      ship.pos.y = 25.0f + rand() % 750;
      ship.isInvincible = true;
      InvincibleTime = esat::Time();
      PaintTime = esat::Time();
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Space) && !ship.isInvincible)
    {

      TShot shot;

      shot.dir.x = cos(ship.angle);
      shot.dir.y = sin(ship.angle);
      shot.pos = zoro::SumVec2(ship.pos, zoro::ScaleVec2(shot.dir, ship.scale * 0.9));
      shot.isEnemy = false;
      shot.next = nullptr;

      addShot(shot);
    }
  }

  if (esat::MouseButtonDown(0) && currentFocus != nullptr)
  {

    currentFocus->focused = false;
    currentFocus = nullptr;
  }

  if (currentFocus != nullptr)
  {
    char c;
    c = esat::GetNextPressedKey();
    // printf("%c", c);

    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Backspace) && *(currentFocus->text + 0) != '\0' &&
        esat::Time() - BackspaceCooldownTime > BackspaceCooldownTimeRef)
    {
      // printf("\n%d ,%s", strlen(TextBuffer), TextBuffer);
      BackspaceCooldownTime = esat::Time();
      *(currentFocus->text + (strlen(currentFocus->text) - 1)) = '\0';
    }
    if (c != '\0' && strlen(currentFocus->text) < 25 && ((c >= 64 && c <= 90) || (c >= 46 && c <= 57) || c == 32))
    {
      if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Alt) && c == 50)
        c = '@';

      (*(currentFocus->text + strlen(currentFocus->text))) = c;
    }
    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab))
    {
      currentFocus->focused = false;
      currentFocus = nullptr;
      lookingNextButton = true;
    }
  }
}

// ----------------------------------- Managment -----------------------------------

void startNewLevel()
{
  emptyShotList();

  ship.pos = {400.0f, 400.0f};
  ship.speed = {0.0f, 0.0f};
  ship.acceleration = {0.0f, 0.0f};
  ship.angle = 0.0f;

  UFOCooldownTime = esat::Time();
  UFORepositiontTime = esat::Time();
}

void startNewGame()
{
  GAMESTATE = INGAME;
  user1->credits--;
  level = 1;
  ship.lives = 3;
  startNewLevel();
}

void CEO()
{
  input();
  switch (GAMESTATE)
  {
  case MENU:

    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetTextSize(80);
    esat::DrawText(190, 200, "star castle");
    esat::DrawSetTextSize(30);
    esat::DrawText(260, 240, "guillermo bosca");
    if (isLogged)
    {
      esat::DrawText(685, 790, "Logged");
      itoa(user1->credits, TextBuffer, 10);
      esat::DrawText(760, 30, TextBuffer);
      if (user1->isAdmin)
      {
        esat::DrawText(10, 790, "ADMIN");
        paintButton(adminButton);
      }
    }
    else
    {
      esat::DrawText(612, 790, "Not logged");
    }

    break;

  case LOGIN:
    // printf("->User:%s\n->Pass: %s\n", user1->nick, user1->pass);
    break;

  case SIGNUP:
    // printf("tempuser: %s\ninput: %s\n", tempUser->nick, username_Signup->text);
    break;

  case INGAME:

    shotsManager();
    shipManager();
    paintGUI();
    break;

  case CREDITS:
    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetTextSize(45);
    esat::DrawText(280, 240, "developer");
    esat::DrawText(205, 350, "guillermo bosca");
    break;
  case GAMEOVER:
    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetTextSize(45);
    esat::DrawText(280, 240, "GAME OVER");
    esat::DrawText(205, 350, "SCORE:");
    itoa(ship.score, TextBuffer, 10);
    esat::DrawText(400, 350, TextBuffer);
    if (isNewTop10)
    {
      esat::DrawSetTextSize(33);
      esat::DrawText(80, 150, "TOP 10 HIGHSCORE 5 EXTRA CREDITS!");
    }
    break;
  case SCOREBOARD:
    paintScoreboard();
    break;
  case ADMIN:
    paintAdmin();
    break;
  default:
    break;
  }
  interfaceManager();
}

int esat::main(int argc, char **argv)
{
  srand(time(NULL));

  esat::WindowInit(800, 800);
  WindowSetMouseVisibility(true);

  printf("---------------------------------------------------");

  initShip(&ship);
  initEnemyShip(&enemyShip);

  init();

  initEmptyUser(&user1);
  initEmptyUser(&tempUser);

  initInterfaceData();

  LoadUserList();
  // PrintUserList();

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0, 255);

    CEO();

    esat::DrawEnd();

    // Control fps
    do
    {
      current_time = esat::Time();
    } while ((current_time - last_time) <= 1000.0 / fps);
    esat::WindowFrame();
  }
  esat::WindowDestroy();
  return 0;
}
