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
#include "asteroidsAPI.h"

enum WindowState
{
  MENU = 0,
  LOGIN,
  SCOREBOARD,
  CREDITS,
  SIGNUP,
  INGAME,
  ADMIN,
  PAUSE,
  GAMEOVER
};

struct TUfo
{
  float speed;
  zoro::Vec2 pos;
  zoro::Vec2 dir;
  bool isBig;
  zoro::Mat3 M;
  float accuracy = 0;
  bool active = false;
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

  ast::rgb color = {255, 255, 255};
  int score = 0;
  int lives = 3;
  int kNPoints = 5;
  float scale = 50.0f;
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
int level = 1;

bool isNewTop10 = false;

TUser *user1 = nullptr;
TUser *tempUser = nullptr;
TUserList *UserList = nullptr;

TShip ship;
TShip ship2; // player 2

ast::TPaintColData ufoDataUp, ufoDataDown;
ast::TPaintColData *astDataTypes;
ast::TAsteroid *asteroidList;

TButton *InterfaceList;
TShot *shotlist;
TUfo ufo;

TScoreboard *scoreList = nullptr;

zoro::Vec2 *sqPoints; // Square points

char *TextBuffer = nullptr;

TPopup popup;
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

bool lookingNextButton = false;

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
const int totalAsteroidTypes = 4;

const float shotSpeed = 10.0f;
const float asteroidSpeed = 1.5f;

const float shipScale = 30.0f;

void startNewGame();
bool checkColP(ast::TColPoints *colP, ast::TAsteroid *p, zoro::Vec2 pos, bool inner);

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

void PrintUserList()
{
  TUserList *p = UserList;
  printf("----------------------------\n");
  while (p != nullptr)
  {
    PrintUser(p->user);
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
  printf("newuser: %d\ntempuser: %d", (int)newUserInList->user, (int)newUser);
  printf("\nUSER ADDED TO LIST\n");
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
  esat::DrawText(100, y + 35, user);
  esat::DrawText(300, y + 35, date);
  itoa(order + 1, TextBuffer, 10);
  esat::DrawText(60, y + 35, TextBuffer);
  itoa(score, TextBuffer, 10);
  esat::DrawText(500, y + 35, TextBuffer);
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
    // i * 50 = altura total scorelist
    // 500 = altura max scroller

    // 500 = 100
    // 1000 = x
    
    // altura = m / (((t*100)/m)/100)

     float scrollerHeight = 500 * (500.0f / (i * 50.0f));

   

    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
    {
      scrollOffset -= 4;
    }else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
    {
      scrollOffset += 4;
    }
    if (scrollOffset < -( (i*50)-500))
    {
      scrollOffset = -( (i*50)-500);
    }
    if (scrollOffset > 0)
    {
      scrollOffset = 0;
    }
    printf("\n%f", scrollOffset);
     *(sqPoints + 0) = {742, 80-scrollOffset};
    *(sqPoints + 1) = {759, 80-scrollOffset};
    *(sqPoints + 2) = {759, 80+scrollerHeight - scrollOffset};
    *(sqPoints + 3) = {742, 80+scrollerHeight - scrollOffset};
    esat::DrawSetStrokeColor(250, 250, 250, 255);
    esat::DrawSetFillColor(250, 250, 250, 180);
    esat::DrawSolidPath(&sqPoints[0].x, 4);
    
    
    
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
  return true;
}

// ----------------------------------- Inits -----------------------------------

ast::TAsteroid *randomAsteroid()
{
  ast::TAsteroid *asteroidNew = (ast::TAsteroid *)malloc(sizeof(ast::TAsteroid));

  asteroidNew->pos.x = 0.0f + rand() % 750;
  asteroidNew->pos.y = 0.0f + rand() % 750;

  if (rand() % 2 == 1)
    asteroidNew->pos.x += 750.0f;
  if (rand() % 2 == 1)
    asteroidNew->pos.y += 750.0f;

  asteroidNew->size = ast::BIG;
  asteroidNew->speed = asteroidSpeed;
  if (level > 6)
  {
    asteroidNew->speed += (level - 6) / 2;
  }

  asteroidNew->type = rand() % totalAsteroidTypes;
  asteroidNew->color = {(float)50 + rand() % 200, (float)50 + rand() % 200, (float)50 + rand() % 200};

  int dir = (rand() % 628) * 0.1;
  asteroidNew->dir = zoro::NormalizeVec2({cosf(dir), sinf(dir)});

  return asteroidNew;
}

void init()
{
  ast::InitList(&asteroidList);
  esat::DrawSetTextFont("./resources/fonts/Hyperspace.otf");
  shotlist = nullptr;
  InterfaceList = nullptr;

  for (int i = 0; i < totalAsteroidTypes; i++)
  {
    ast::GenerateAsteroidColPoints((astDataTypes + i));
  }

  // Pointers
  TextBuffer = (char *)malloc(30 * sizeof(char));

  popup.text = nullptr;

  sqPoints = (zoro::Vec2 *)malloc(4 * sizeof(zoro::Vec2));
  ufo.pos = {400.0f, 410.0f};
  ufo.dir = {0.0f, 0.0f};
  ufo.isBig = false;
  ufo.speed = 1.33f;
  ufo.active = false;
}

void initAstData()
{
  astDataTypes = (ast::TPaintColData *)malloc(4 * sizeof(ast::TPaintColData));

  (astDataTypes + 0)->kNPoints = 12;
  (astDataTypes + 1)->kNPoints = 11;
  (astDataTypes + 2)->kNPoints = 10;
  (astDataTypes + 3)->kNPoints = 12;

  for (int i = 0; i < 4; i++)
  {
    (astDataTypes + i)->dr_points = (zoro::Vec2 *)malloc((astDataTypes + i)->kNPoints * sizeof(zoro::Vec2));
    (astDataTypes + i)->g_points = (zoro::Vec3 *)malloc((astDataTypes + i)->kNPoints * sizeof(zoro::Vec3));
    (astDataTypes + i)->col = (ast::TColPoints *)malloc(1 * sizeof(ast::TColPoints));
    (astDataTypes + i)->col->points = (zoro::Vec2 *)malloc((astDataTypes + i)->kNPoints * sizeof(zoro::Vec2));
    (astDataTypes + i)->col->next = nullptr;
    (astDataTypes + i)->col->NumColPoints = 0;
  }

  ufoDataUp.kNPoints = 4;
  ufoDataUp.col = (ast::TColPoints *)malloc(sizeof(ast::TColPoints));
  ufoDataUp.col->points = (zoro::Vec2 *)malloc((ufoDataUp.kNPoints * sizeof(zoro::Vec2)));
  ufoDataUp.g_points = (zoro::Vec3 *)malloc(ufoDataUp.kNPoints * sizeof(zoro::Vec3));
  ufoDataUp.dr_points = (zoro::Vec2 *)malloc(ufoDataUp.kNPoints * sizeof(zoro::Vec2));
  ufoDataUp.col->next = nullptr;

  ufoDataDown.kNPoints = 6;
  ufoDataDown.col = (ast::TColPoints *)malloc(sizeof(ast::TColPoints));
  ufoDataDown.col->points = (zoro::Vec2 *)malloc((ufoDataDown.kNPoints * sizeof(zoro::Vec2)));
  ufoDataDown.g_points = (zoro::Vec3 *)malloc(ufoDataDown.kNPoints * sizeof(zoro::Vec3));
  ufoDataDown.dr_points = (zoro::Vec2 *)malloc(ufoDataDown.kNPoints * sizeof(zoro::Vec2));
  ufoDataDown.col->next = nullptr;

  // Asteroid 0

  (*((*(astDataTypes + 0)).g_points + 0)) = {0.4f, -0.22f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 1)) = {0.87f, -0.5f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 2)) = {0.44f, -0.95f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 3)) = {0.0f, -0.72f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 4)) = {-0.38f, -0.95f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 5)) = {-0.90f, -0.4f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 6)) = {-0.77f, 0.0f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 7)) = {-0.9f, 0.48f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 8)) = {-0.5f, 0.95f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 9)) = {-0.3f, 0.75f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 10)) = {0.31f, 0.97f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 11)) = {0.88f, 0.25f, 1.0f};

  // Asteroid 1

  (*((*(astDataTypes + 1)).g_points + 0)) = {-0.1f, 0.1f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 1)) = {-0.2f, 0.95f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 2)) = {0.17f, 0.97f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 3)) = {0.75f, 0.07f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 4)) = {0.75f, -0.25f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 5)) = {0.37f, -0.99f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 6)) = {-0.25f, -0.99f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 7)) = {-0.8f, -0.25f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 8)) = {-0.43f, 0.05f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 9)) = {-0.8f, 0.18f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 10)) = {-0.5f, 0.93f, 1.0f};

  // Asteroid 2

  (*((*(astDataTypes + 2)).g_points + 0)) = {0.55f, -0.1f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 1)) = {0.78f, -0.6f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 2)) = {0.41f, -0.94f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 3)) = {-0.02f, -0.56f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 4)) = {-0.43f, -0.9f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 5)) = {-0.77f, -0.61f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 6)) = {-0.77f, 0.52f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 7)) = {-0.48f, 0.89f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 8)) = {0.2f, 0.89f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 9)) = {0.74f, 0.45f, 1.0f};

  // Asteroid 3
  (*((*(astDataTypes + 3)).g_points + 0)) = {0.14f, 0.0f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 1)) = {0.78f, -0.27f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 2)) = {0.78f, -0.5f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 3)) = {0.24f, -0.9f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 4)) = {-0.31f, -0.9f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 5)) = {-0.12f, -0.5f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 6)) = {-0.70f, -0.5f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 7)) = {-0.70f, 0.16f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 8)) = {-0.40f, 0.85f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 9)) = {0.11f, 0.62f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 10)) = {0.42f, 0.84f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 11)) = {0.78f, 0.41f, 1.0f};

  // UFO
  *(ufoDataUp.g_points + 0) = {-0.20f, -0.40f, 1.0f};
  *(ufoDataUp.g_points + 1) = {0.18f, -0.40f, 1.0f};
  *(ufoDataUp.g_points + 2) = {0.32f, -0.1f, 1.0f};
  *(ufoDataUp.g_points + 3) = {-0.47f, -0.1f, 1.0f};

  *(ufoDataDown.g_points + 0) = {0.32f, -0.1f, 1.0f};
  *(ufoDataDown.g_points + 1) = {-0.47f, -0.1f, 1.0f};
  *(ufoDataDown.g_points + 2) = {-0.85f, 0.1f, 1.0f};
  *(ufoDataDown.g_points + 3) = {-0.4f, 0.38f, 1.0f};
  *(ufoDataDown.g_points + 4) = {0.39f, 0.38f, 1.0f};
  *(ufoDataDown.g_points + 5) = {0.77f, 0.1f, 1.0f};
}

void GoToSignUp()
{
  printf("\nGoing to Signup - ");
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
  /*
  tempUser->nick = username_Signup->text;
  tempUser->pass = password_Signup->text;
  tempUser->name = name->text;
  tempUser->lastname = lastname->text;
  tempUser->email = email->text;
  tempUser->prov = prov->text;
  tempUser->country = country->text;
  tempUser->birthday = birthday->text;
  */

  printf("end\n");
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
  *(ship->g_points + 0) = {+0.8f, +0.0f, 1.0f};
  *(ship->g_points + 1) = {-0.25f, +0.35f, 1.0f};
  *(ship->g_points + 2) = {-0.1f, +0.2f, 1.0f};
  *(ship->g_points + 3) = {-0.1f, -0.2f, 1.0f};
  *(ship->g_points + 4) = {-0.25f, -0.35f, 1.0f};

  ship->dr_Fuelpoints = (zoro::Vec2 *)malloc(3 * sizeof(zoro::Vec2));
  ship->g_Fuelpoints = (zoro::Vec3 *)malloc(3 * sizeof(zoro::Vec3));
  *(ship->g_Fuelpoints + 0) = {-0.13f, +0.19f, 1.0f};
  *(ship->g_Fuelpoints + 1) = {-0.13f, -0.19f, 1.0f};
  *(ship->g_Fuelpoints + 2) = {-0.55f, +0.0f, 1.0f};
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

  esat::DrawSetStrokeColor(p_ship.color.r, p_ship.color.g, p_ship.color.b, 255);
  esat::DrawSetFillColor(p_ship.color.r, p_ship.color.g, p_ship.color.b, 20);
  esat::DrawSolidPath(&p_ship.dr_points[0].x, p_ship.kNPoints);

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
  printf("\nLives: %d", ship.lives);
  if (ship.lives <= 0)
  {
    isNewTop10 = false;
    GAMESTATE = GAMEOVER;
    PrintUser(user1);
    printf("ship score: %d\n", ship.score);
    if (ship.score > user1->highscore)
    {
      printf("NEW HIGHSCORE");
      isNewTop10 = isTop10HS(ship.score);
      user1->highscore = ship.score;
      SaveDataUser();
    }
  }
  else
  {
    ship.isDying = true;
    ShipDyingAnimTime = esat::Time();
  }
}

void shipColision()
{
  zoro::Vec2 shipPoint;
  zoro::Vec3 gshipPoint;
  ast::TPaintColData *data = nullptr;
  ast::TColPoints *colP = nullptr;
  ast::TAsteroid *p = asteroidList;
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

  for (int i = 0; i < 5; i++)
  {
    if (i != 2 && i != 3)
    {
      shipPoint = *(ship.dr_points + i);

      while (p != nullptr)
      {
        data = (astDataTypes + p->type);
        colP = data->col;
        if (checkColP(colP, p, shipPoint, false))
        {
          bool colisionWithEmpty = false;
          colP = colP->next;

          if (colP == nullptr)
          {
            // Destroy ship
            ship.lives--;
            shipDeath();
            return;
          }
          else
          {
            while (colP != nullptr)
            {
              if (checkColP(colP, p, shipPoint, true))
              {
                colisionWithEmpty = true;
              }
              colP = colP->next;
            }
            if (!colisionWithEmpty)
            {
              // Destroy ship
              ship.lives--;
              shipDeath();
              return;
            }
          }
        }
        p = p->next;
      }
    }
  }

  p = asteroidList;
  int counter = 0;

  while (p != nullptr)
  {
    data = (astDataTypes + p->type);
    colP = data->col;

    for (int j = 0; j < colP->NumColPoints - 1; j++)
    {
      counter = 0;
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

        temp = zoro::Mat3TransformVec3(p->M, {(colP->points + j)->x, (colP->points + j)->y, 1.0f});
        pos = {temp.x, temp.y};
        // esat::DrawLine(a.x, a.y, pos.x, pos.y);
        float value = DotVec2(zoro::SubtractVec2(pos, a), zoro::RightPerpendicularVec2(zoro::NormalizeVec2(zoro::SubtractVec2(b, a))));

        if (value < 0.0f)
        {
          counter++;
        }
      }

      if (counter == 3)
      {
        // Destroy ship
        ship.lives--;
        shipDeath();
        return;
      }
    }

    p = p->next;
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
    else
    {
    }
  }
}

// ----------------------------------- Shots -----------------------------------

void addShot(TShot s)
{
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

  if (shotlist == nullptr)
    shotlist = newShot;
  else
  {
    newShot->next = shotlist;
    shotlist->prev = newShot;
    shotlist = newShot;
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

  s = nullptr;
  free(s);
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

// ----------------------------------- Asteroids -----------------------------------

void moveAsteroid(ast::TAsteroid *p_asteroid)
{
  p_asteroid->pos = zoro::SumVec2(p_asteroid->pos, zoro::ScaleVec2(p_asteroid->dir, p_asteroid->speed));
  p_asteroid->angle += 0.002;
  if (p_asteroid->pos.x > 840)
    p_asteroid->pos.x = -39;
  if (p_asteroid->pos.x < -40)
    p_asteroid->pos.x = 839;
  if (p_asteroid->pos.y > 840)
    p_asteroid->pos.y = -39;
  if (p_asteroid->pos.y < -40)
    p_asteroid->pos.y = 839;

  p_asteroid->M = zoro::MatIdentity3();

  p_asteroid->M = zoro::Mat3Multiply(zoro::Mat3Translate(p_asteroid->pos), p_asteroid->M);
  p_asteroid->M = zoro::Mat3Multiply(zoro::Mat3Scale((p_asteroid->size + 1) * 20, (p_asteroid->size + 1) * 20), p_asteroid->M);
  if (p_asteroid->angle > 0.0f)
  {
    p_asteroid->M = zoro::Mat3Multiply(zoro::Mat3Rotate(p_asteroid->angle), p_asteroid->M);
  }
}

void debugPaintAsteroidColisions(ast::TAsteroid *p_asteroid)
{

  zoro::Mat3 m = zoro::MatIdentity3();

  m = zoro::Mat3Multiply(zoro::Mat3Translate(p_asteroid->pos), m);
  m = zoro::Mat3Multiply(zoro::Mat3Scale(50, 50), m);
  m = zoro::Mat3Multiply(zoro::Mat3Rotate(p_asteroid->angle), m);

  ast::TPaintColData temp = *(astDataTypes + p_asteroid->type);
  ast::TColPoints *colP = temp.col;

  // printf("\nType: %d, count: %d", p_asteroid->type, asteroidData->col->NumColPoints);

  if (colP != nullptr)
  {
    for (int i = 0; i < colP->NumColPoints; i++)
    {
      zoro::Vec2 f = *(colP->points + i);
      zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, {f.x, f.y, 1.0f});
      *(temp.dr_points + i) = {tmp.x, tmp.y};
    }

    esat::DrawSetStrokeColor(255, 0, 0, 50);
    esat::DrawSetFillColor(255, 0, 0, 120);
    // esat::DrawSolidPath(&temp.dr_points[0].x, colP->NumColPoints);
  }

  do
  {
    if (colP->next != nullptr)
      colP = colP->next;

    for (int i = 0; i < colP->NumColPoints; i++)
    {
      zoro::Vec2 f = *(colP->points + i);
      zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, {f.x, f.y, 1.0f});
      *(temp.dr_points + i) = {tmp.x, tmp.y};
    }

    esat::DrawSetStrokeColor(150, 150, 0, 50);
    esat::DrawSetFillColor(255, 150, 0, 120);
    esat::DrawSolidPath(&temp.dr_points[0].x, colP->NumColPoints);

  } while (colP->next != nullptr);
}

void paintAsteroid(ast::TAsteroid *p_asteroid)
{

  ast::TPaintColData temp = *(astDataTypes + p_asteroid->type);

  for (int i = 0; i < temp.kNPoints; i++)
  {
    zoro::Vec3 tmp = zoro::Mat3TransformVec3(p_asteroid->M, *(temp.g_points + i));
    *(temp.dr_points + i) = {tmp.x, tmp.y};
  }

  esat::DrawSetStrokeColor(255, 255, 255, 100);
  esat::DrawSetFillColor(255, 255, 255, 20);
  esat::DrawSolidPath(&temp.dr_points[0].x, temp.kNPoints);
}

void addChildAsteroid(ast::TAsteroid *p)
{
  if (p->size > 0)
  {
    ast::TAsteroid *new1, *new2;

    new1 = (ast::TAsteroid *)malloc(sizeof(ast::TAsteroid));
    new2 = (ast::TAsteroid *)malloc(sizeof(ast::TAsteroid));

    new1->pos = p->pos;
    new2->pos = p->pos;

    new1->size = (enum ast::AsteroidSize)((int)p->size - 1);
    new2->size = (enum ast::AsteroidSize)((int)p->size - 1);

    new1->speed = p->speed;
    new2->speed = p->speed;

    int dir = (rand() % 628) * 0.1;
    new1->dir = zoro::NormalizeVec2({cosf(dir), sinf(dir)});
    dir = (rand() % 628) * 0.1;
    new2->dir = zoro::NormalizeVec2({cosf(dir), sinf(dir)});

    do
    {
      new1->type = rand() % totalAsteroidTypes;
    } while (new1->type == p->type);
    do
    {
      new2->type = rand() % totalAsteroidTypes;
    } while (new2->type == p->type || new2->type == new1->type);

    ast::InsertList(&asteroidList, new1);
    ast::InsertList(&asteroidList, new2);
  }
}

bool checkColP(ast::TColPoints *colP, ast::TAsteroid *p, zoro::Vec2 pos, bool inner)
{
  zoro::Vec2 a, b;
  zoro::Vec3 temp;
  int counter = 0;
  for (int i = 0; i < colP->NumColPoints; i++)
  {
    a = *(colP->points + i);
    if (i == colP->NumColPoints - 1)
    {
      b = *(colP->points + 0);
    }
    else
    {
      b = *(colP->points + i + 1);
    }

    temp = zoro::Mat3TransformVec3(p->M, {a.x, a.y, 1.0f});
    a = {temp.x, temp.y};
    temp = zoro::Mat3TransformVec3(p->M, {b.x, b.y, 1.0f});
    b = {temp.x, temp.y};

    float value = DotVec2(zoro::SubtractVec2(pos, a), zoro::RightPerpendicularVec2(zoro::NormalizeVec2(zoro::SubtractVec2(b, a))));
    if (!inner)
    {
      if (value > 0.0f)
      {
        counter++;
      }
    }
    else
    {
      if (value < 0.0f)
      {
        counter++;
      }
    }
  }
  if (counter == colP->NumColPoints)
  {
    return true;
  }
  return false;
}

void addScore(enum ast::AsteroidSize type)
{

  switch (type)
  {
  case ast::BIG:
    ship.score += 20;
    break;
  case ast::MID:
    ship.score += 50;
    break;
  case ast::SMALL:
    ship.score += 100;
    break;
  default:
    break;
  }
}

void checkColisionAsteroid(ast::TAsteroid *p)
{
  TShot *shots = shotlist;

  ast::TPaintColData *data = nullptr;
  ast::TColPoints *colP = nullptr;

  bool found = false;
  while (shots != nullptr)
  {

    if (p != nullptr)
    {
      data = (astDataTypes + p->type);
      colP = data->col;

      if (checkColP(colP, p, shots->pos, false))
      {
        bool colisionWithEmpty = false;
        colP = colP->next;

        if (colP == nullptr)
        {
          shots->active = false;
        }
        else
        {
          while (colP != nullptr)
          {
            if (checkColP(colP, p, shots->pos, true))
            {
              colisionWithEmpty = true;
            }
            colP = colP->next;
          }
          if (!colisionWithEmpty)
          {
            deleteShot(shots);
            addScore(p->size);

            addChildAsteroid(p);
            ast::Delete(&asteroidList, p);
          }
        }
      }
    }

    if (shots != nullptr)
    {
      shots = shots->next;
    }
  }
}

void asteroidsManager()
{
  ast::TAsteroid *p = asteroidList;
  if (asteroidList == nullptr)
  {
    level++;
    startNewGame();
  }

  while (p != nullptr)
  {
    moveAsteroid(p);
    checkColisionAsteroid(p);
    // debugPaintAsteroidColisions(p);
    paintAsteroid(p);

    p = p->next;
  }
}

// ----------------------------------- UFO -----------------------------------

void shootUFO()
{
  if (ufo.isBig)
  {
    ufo.accuracy = rand() % (628) * 0.01;
  }
  else
  {

    if (314 - (level * 20) < 0.0f)
      ufo.accuracy = 0.0f;
    else
    {
      ufo.accuracy = rand() % (314 - (level * 20)) * 0.01;
      ufo.accuracy -= ((314 - (level * 20)) / 2) * 0.01;
    }
  }

  zoro::Vec2 baseDir = zoro::NormalizeVec2(zoro::SubtractVec2(ship.pos, ufo.pos));

  zoro::Vec3 tmp = {baseDir.x, baseDir.y, 1.0f};
  tmp = zoro::Mat3TransformVec3(zoro::Mat3Multiply(zoro::Mat3Rotate(ufo.accuracy), zoro::MatIdentity3()), tmp);

  TShot shot;
  shot.dir = {tmp.x, tmp.y};
  shot.pos = ufo.pos;
  shot.isEnemy = true;
  shot.next = nullptr;

  zoro::PrintVec2(shot.pos);
  addShot(shot);
}

void activateUFO()
{
  ufo.active = true;
  ufo.pos.x = -150.0f;
  ufo.pos.y = 300.0f + rand() % 200;

  if (rand() % 2 == 1)
  {
    printf("Right start\n");
    ufo.pos.x += 950.0f;
    ufo.dir = {-1.0f, 0.0f}; // Move left
  }
  else
  {
    printf("Left start\n");
    ufo.dir = {1.0f, 0.0f}; // Move right
  }
}

void deactivateUFO()
{
  printf("Deactivated\n");
  UFOCooldownTime = esat::Time();
  ufo.active = false;
}

void repositionUFO()
{
  printf("Repositioning\n");

  if (ufo.dir.y != 0)
  {
    ufo.dir.y = 0;
  }
  else
  {
    if (ufo.pos.y > 500)
    {
      ufo.dir.y = -1;
      return;
    }
    if (ufo.pos.y < 300)
    {
      ufo.dir.y = 1;
      return;
    }
    if (rand() % 2 == 1)
    {
      ufo.dir.y = 1;
    }
    else
    {
      ufo.dir.y = -1;
    }
  }
}

void AIufo()
{
  if (!ufo.active)
  {
    if ((esat::Time() - LastShotTime > LastShotTimeRef || ast::ListLength(asteroidList, true) < 8) &&
        esat::Time() - UFOCooldownTime > UFOCooldownTimeRef)
    {
      activateUFO();
    }
  }
  else
  {
    if (ufo.pos.x > 800 + 150 || ufo.pos.x < -250)
    {
      deactivateUFO();
    }

    if (esat::Time() - UFORepositiontTime > UFORepositionTimeRef)
    {
      if (rand() % 100 == 1)
      {
        repositionUFO();
        UFORepositiontTime = esat::Time();
      }
    }
  }
}

void moveUfo()
{
  if (ufo.active)
  {
    ufo.pos = zoro::SumVec2(ufo.pos, zoro::ScaleVec2(zoro::NormalizeVec2(ufo.dir), ufo.speed));

    ufo.M = zoro::MatIdentity3();
    ufo.M = zoro::Mat3Multiply(zoro::Mat3Translate(ufo.pos), ufo.M);

    if (ufo.isBig)
    {
      ufo.M = zoro::Mat3Multiply(zoro::Mat3Scale(60.0f, 60.0f), ufo.M);
    }
    else
    {
      ufo.M = zoro::Mat3Multiply(zoro::Mat3Scale(20.0f, 20.0f), ufo.M);
    }
  }
}

void paintUfo()
{
  zoro::Vec3 tmp;
  for (int i = 0; i < ufoDataUp.kNPoints; i++)
  {
    tmp = zoro::Mat3TransformVec3(ufo.M, *(ufoDataUp.g_points + i));
    *(ufoDataUp.dr_points + i) = {tmp.x, tmp.y};
  }

  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 20);
  esat::DrawSolidPath(&ufoDataUp.dr_points[0].x, ufoDataUp.kNPoints);

  for (int i = 0; i < ufoDataDown.kNPoints; i++)
  {
    tmp = zoro::Mat3TransformVec3(ufo.M, *(ufoDataDown.g_points + i));
    *(ufoDataDown.dr_points + i) = {tmp.x, tmp.y};
  }

  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 20);
  esat::DrawSolidPath(&ufoDataDown.dr_points[0].x, ufoDataDown.kNPoints);
}

void ufoColision()
{
  TShot *shots = shotlist;
  zoro::Vec2 a, b;
  zoro::Vec3 temp;
  float value;
  int counter;

  while (shots != nullptr)
  {
    if (!shots->isEnemy)
    {

      counter = 0;

      for (int i = 0; i < ufoDataUp.kNPoints; i++)
      {

        a = *(ufoDataUp.dr_points + i);
        if (i == ufoDataUp.kNPoints - 1)
        {
          b = *(ufoDataUp.dr_points + 0);
        }
        else
        {
          b = *(ufoDataUp.dr_points + i + 1);
        }

        value = DotVec2(zoro::SubtractVec2(shots->pos, a), zoro::RightPerpendicularVec2(zoro::NormalizeVec2(zoro::SubtractVec2(b, a))));

        if (value < 0.0f)
        {
          counter++;
        }

        if (counter == ufoDataUp.kNPoints)
        {
          deleteShot(shots);
        }
      }

      // Down UFO
      counter = 0;
      for (int i = 0; i < ufoDataDown.kNPoints; i++)
      {

        a = *(ufoDataDown.dr_points + i);
        if (i == ufoDataDown.kNPoints - 1)
        {
          b = *(ufoDataDown.dr_points + 0);
        }
        else
        {
          b = *(ufoDataDown.dr_points + i + 1);
        }

        value = DotVec2(zoro::SubtractVec2(shots->pos, a), zoro::RightPerpendicularVec2(zoro::NormalizeVec2(zoro::SubtractVec2(b, a))));

        if (value > 0.0f)
        {
          counter++;
        }

        if (counter == ufoDataDown.kNPoints)
        {
          deleteShot(shots);
        }
      }
    }

    shots = shots->next;
  }
}

void ufoManager()
{
  if (esat::IsKeyDown('A'))
  {
    shootUFO();
  }

  AIufo();
  moveUfo();
  ufoColision();
  paintUfo();
}

// ----------------------------------- GUI -----------------------------------

void paintGUI()
{

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
    esat::DrawSetStrokeColor(ship.color.r, ship.color.g, ship.color.b, 255);
    esat::DrawSetFillColor(ship.color.r, ship.color.g, ship.color.b, 20);
    esat::DrawSolidPath(&ship.dr_points[0].x, ship.kNPoints);
  }

  for (int i = 0; i < ship2.lives; i++)
  {
    zoro::Mat3 m = zoro::MatIdentity3();
    m = zoro::Mat3Multiply(zoro::Mat3Translate({(-i * 20) + 740.0f, 100.0f}), m);
    m = zoro::Mat3Multiply(zoro::Mat3Scale(20, 20), m);
    m = zoro::Mat3Multiply(zoro::Mat3Rotate(-3.14f / 2), m);

    for (int i = 0; i < ship.kNPoints; i++)
    {
      zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *(ship.g_points + i));
      *(ship.dr_points + i) = {tmp.x, tmp.y};
    }
    esat::DrawSetStrokeColor(ship.color.r, ship.color.g, ship.color.b, 255);
    esat::DrawSetFillColor(ship.color.r, ship.color.g, ship.color.b, 20);
    esat::DrawSolidPath(&ship.dr_points[0].x, ship.kNPoints);
  }
}

// ----------------------------------- Interface -----------------------------------

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
    break;
  case 5:

    if (isLogged)
    {
      startNewGame();
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
    if (!checkIfUserDataIsFull(tempUser))
    {
      popup.Activate = true;
      popup.maxActiveTime = 3000;
      popup.text = "Fill all the data";
      return;
    }
    printf("\nUSER INSERTED NOT ERROR OK GOOD FINE UWU!!!\n");
    insertUser(tempUser, true);
    SaveDataUser();
    tempUser = nullptr;
    initEmptyUser(&tempUser);
    GAMESTATE = MENU;
    break;

  default:
    break;
  }
}

TButton *addButtonToList(TButton b)
{
  TButton *newbutton = (TButton *)malloc(1 * sizeof(TButton));

  newbutton->pos = b.pos;
  newbutton->dimensions = b.dimensions;
  newbutton->idFunction = b.idFunction;
  newbutton->windowContext = b.windowContext;
  newbutton->fontSize = b.fontSize;
  newbutton->isInput = b.isInput;
  newbutton->label = b.label;
  newbutton->hasLabel = b.hasLabel;

  if (b.isInput)
  {
    newbutton->text = (char *)malloc(40 * sizeof(char));
  }
  else
  {
    newbutton->text = b.text;
  }

  newbutton->focused = false;

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

  newButton.text = "go back";
  newButton.idFunction = 3;
  newButton.pos = {400, 640};
  newButton.windowContext = ADMIN;
  newButton.dimensions = {400, 40};

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

  newButton.text = "admin";
  newButton.idFunction = 4;
  newButton.pos = {400, 640};
  newButton.windowContext = MENU;
  newButton.dimensions = {400, 40};

  addButtonToList(newButton);

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
  newButton.idFunction = 1;
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
  newButton.label = "birthday";
  birthday = addButtonToList(newButton);

  // Highscore Date
  newButton.pos = {400, 640};
  newButton.windowContext = GAMEOVER;
  newButton.dimensions = {410, 40};
  newButton.label = "date";
  highscoreDate = addButtonToList(newButton);
}

void paintMenu()
{
  // printf("\nPaint menu - ");
  zoro::Vec2 finalTextPos;
  float mx, my;
  TButton *p = InterfaceList;
  while (p != nullptr)
  {
    if (GAMESTATE == p->windowContext)
    {
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

    p = p->next;
  }
  // printf("end...");
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
      // ast::Delete(&asteroidList);
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

// ----------------------------------- Extra -----------------------------------

void ClearDrawCoolEffectUwU()
{
  zoro::Vec2 p[4];
  p[0] = {-1.0f, -1.0f};
  p[1] = {801.0f, -1.0f};
  p[2] = {801.0f, 801.0f};
  p[3] = {-1.0f, 801.0f};

  ship.color = {(float)50 + rand() % 200, (float)50 + rand() % 200, (float)50 + rand() % 200};

  esat::DrawSetStrokeColor(0, 0, 0, 255);
  esat::DrawSetFillColor(0, 0, 0, 30);
  esat::DrawSolidPath(&p[0].x, 4);
}

void freeMemory()
{
  //  OwO
}

// ----------------------------------- Managment -----------------------------------

void startNewGame()
{
  GAMESTATE = INGAME;

  int numAsteroidsToGenerate = 2;
  if (level > 6)
  {
    numAsteroidsToGenerate = 12;
  }
  else
  {
    numAsteroidsToGenerate += level * 2;
  }

  // 2 - 3 - 4 - 5  - 6
  // 4 - 6 - 8 - 10 - 12 + velocity
  ast::TAsteroid *current = asteroidList;
  while (current != nullptr)
  {
    ast::TAsteroid *next = current->next;
    ast::Delete(&asteroidList, current);
    current = next;
  }

  for (int i = 0; i < numAsteroidsToGenerate; i++)
  {
    ast::InsertList(&asteroidList, randomAsteroid());
  }

  ship.pos = {400.0f, 400.0f};
  ship.speed = {0.0f, 0.0f};
  ship.acceleration = {0.0f, 0.0f};
  ship.angle = 0.0f;
  ufo.active = false;
  ship.lives = 3;

  UFOCooldownTime = esat::Time();
  UFORepositiontTime = esat::Time();
}

void CEO()
{
  input();
  switch (GAMESTATE)
  {
  case MENU:

    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetTextSize(80);
    esat::DrawText(190, 200, "asteroids");
    esat::DrawSetTextSize(30);
    esat::DrawText(260, 240, "guillermo bosca");

    break;

  case LOGIN:
    // printf("->User:%s\n->Pass: %s\n", user1->nick, user1->pass);
    break;

  case SIGNUP:
    // printf("tempuser: %s\ninput: %s\n", tempUser->nick, username_Signup->text);
    break;

  case INGAME:

    shotsManager();

    asteroidsManager();

    ufoManager();

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
      esat::DrawText(150, 150, "NEW TOP 10 HIGHSCORE");
    }
    break;
  case SCOREBOARD:
    paintScoreboard();
    break;
  default:
    break;
  }
  interfaceManager();
}

int esat::main(int argc, char **argv)
{
  srand(time(NULL)); // RANDOMNESS

  esat::WindowInit(800, 800);
  WindowSetMouseVisibility(true);

  initAstData();
  initShip(&ship);
  init();

  initEmptyUser(&user1);
  initEmptyUser(&tempUser);
  initInterfaceData();

  LoadUserList();
  PrintUserList();

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0, 255);
    // ClearDrawCoolEffectUwU();

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
