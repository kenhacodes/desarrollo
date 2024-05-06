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
  SIGNUP,
  INGAME
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
  zoro::Mat3 M;
};

struct TUser
{
  char *name;     // 20
  char *email;    // 32
  char *lastname; // 20
  char *pass;     // 24
  char *prov;     // 16
  char *country;  // 20
  int credits;    // 4
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

struct TButton{
  int idFunction;
  int fontSize;
  zoro::Vec2 pos;
  zoro::Vec2 dimensions;
  WindowState windowContext;
  char *text;
  TButton *next;
};

enum WindowState GAMESTATE = MENU;
int level = 1;

TShip ship;
TShip ship2; // player 2

ast::TPaintColData ufoDataUp, ufoDataDown;
ast::TPaintColData *astDataTypes;
ast::TAsteroid *asteroidList;

TButton *InterfaceList;
TShot *shotlist;
TUfo ufo;

zoro::Vec2 *sqPoints; // Square points

char *TextBuffer;

//    TIME VARIABLES
unsigned char fps = 60;
double current_time, last_time;

const float sprFuelTimeRef = 100.0f;
double sprFuelTime = 0.0f;

const float shotCooldownTimeRef = 100.0f;
double shotCooldownTime = 0.0f;

const float InvincibleTimeRef = 3500.0f;
const float PaintTimeRef = 200.0f;

double InvincibleTime = 0.0f;
double PaintTime = 0.0f;

const float LastShotTimeRef = 5000.0f;
double LastShotTime = 0.0f;

const float UFORepositionTimeRef = 1000.0f;
double UFORepositiontTime = 0.0f;

const float UFOCooldownTimeRef = 15000.0f;
double UFOCooldownTime = 0.0f;

//  CONSTANTS
const float WINDOW_X = 800.0f, WINDOW_Y = 800.0f;

const float shotMaxDistance = 700.0f;
const int totalAsteroidTypes = 4;

const float shotSpeed = 10.0f;
const float asteroidSpeed = 1.5f;

const float shipScale = 30.0f;

void startNewGame();

// ----------------------------------- Inits -----------------------------------

ast::TAsteroid *randomAsteroid()
{
  ast::TAsteroid *asteroidNew = (ast::TAsteroid *)malloc(sizeof(ast::TAsteroid));

  asteroidNew->pos.x = 0.0f + rand() % 750;
  asteroidNew->pos.y = 0.0f + rand() % 750;
  /*
  if (rand() % 2 == 1)
    asteroidNew->pos.x += 750.0f;
  if (rand() % 2 == 1)
    asteroidNew->pos.y += 750.0f;
  */

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
  TextBuffer = (char*) malloc(30 * sizeof(char));
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

  zoro::Mat3 m = zoro::MatIdentity3();
  m = zoro::Mat3Multiply(zoro::Mat3Translate(p_ship.pos), m);
  m = zoro::Mat3Multiply(zoro::Mat3Scale(p_ship.scale, p_ship.scale), m);
  m = zoro::Mat3Multiply(zoro::Mat3Rotate(p_ship.angle), m);

  for (int i = 0; i < p_ship.kNPoints; i++)
  {
    zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *(p_ship.g_points + i));
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
      zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *(p_ship.g_Fuelpoints + i));
      *(p_ship.dr_Fuelpoints + i) = {tmp.x, tmp.y};
    }
    esat::DrawSolidPath(&p_ship.dr_Fuelpoints[0].x, 3);
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
  movementShip();
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
    paintShip(ship);
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

  if (s == shotlist)
    shotlist = s->next;

  if (s->prev != nullptr)
    s->prev->next = s->next;

  if (s->next != nullptr)
    s->next->prev = s->prev;

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

void addScore(enum ast::AsteroidSize type){
  
  switch(type){
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

void asteroidsManager() // UWU
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
    }else{
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


void paintGUI(){

  esat::DrawSetTextSize(40);
  esat::DrawSetFillColor(230,230,230,255); 
  itoa(ship.score, TextBuffer, 10);
  esat::DrawText(150,50, TextBuffer);
  itoa(ship2.score, TextBuffer, 10);
  esat::DrawText(650,50, TextBuffer);

  for (int i = 0; i < ship.lives; i++)
  {
    zoro::Mat3 m = zoro::MatIdentity3();
    m = zoro::Mat3Multiply(zoro::Mat3Translate({(i*20) + 40.0f, 100.0f}), m);
    m = zoro::Mat3Multiply(zoro::Mat3Scale(20, 20), m);
    m = zoro::Mat3Multiply(zoro::Mat3Rotate(-3.14f/2), m);

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
    m = zoro::Mat3Multiply(zoro::Mat3Translate({(-i*20) + 740.0f, 100.0f}), m);
    m = zoro::Mat3Multiply(zoro::Mat3Scale(20, 20), m);
    m = zoro::Mat3Multiply(zoro::Mat3Rotate(-3.14f/2), m);

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

void callButtonFunction(int id){
  switch (id)
  {
  case 0:
    startNewGame();
    printf("\n -> NEW GAME <-");
    break;
  
  default:
    break;
  }
}

void addButtonToList(TButton b){
  TButton *newbutton = (TButton*) malloc(1 * sizeof(TButton));

  newbutton->pos = b.pos;
  newbutton->dimensions = b.dimensions;
  newbutton->idFunction = b.idFunction;
  newbutton->windowContext = b.windowContext;
  newbutton->fontSize = b.fontSize;
  newbutton->text = b.text;

  newbutton->next = InterfaceList;
  InterfaceList = newbutton;
}

void initInterfaceData(){

  TButton newButton;
  newButton.text = (char*) malloc(40 * sizeof(char)); // 40 chars maximum

  newButton.fontSize = 40;
  
  newButton.text = "start";
  newButton.idFunction = 0;
  newButton.pos = {400,400};
  newButton.windowContext = MENU;
  newButton.dimensions = { 400,40}; 

  addButtonToList(newButton);

  newButton.text = "scoreboard";
  newButton.idFunction = 1;
  newButton.pos = {400,480};
  newButton.windowContext = MENU;
  newButton.dimensions = { 400,40}; 

  addButtonToList(newButton);

  newButton.text = "credits";
  newButton.idFunction = 1;
  newButton.pos = {400,560};
  newButton.windowContext = MENU;
  newButton.dimensions = { 400,40};

  addButtonToList(newButton);
}

void paintMenu(){

  TButton *p = InterfaceList;
  while (p != nullptr)
  {
    
    if (GAMESTATE == p->windowContext)
    {
      zoro::Vec2 finalTextPos;
      
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
      
      float mx,my;
      mx = esat::MousePositionX();
      my = esat::MousePositionY();
      if ((mx <= (*(sqPoints + 1)).x && mx >= (*(sqPoints + 0)).x) &&
          (my <= (*(sqPoints + 2)).y && my >= (*(sqPoints + 0)).y))
      {
        if(esat::MouseButtonDown(0)){
          callButtonFunction(p->idFunction);
        }

        esat::DrawSetFillColor(255,255,255,255);
        esat::DrawSetStrokeColor(255,255,255,255);
        esat::DrawSolidPath(&sqPoints[0].x, 4);

        esat::DrawSetFillColor(0,0,0,255);
      }else{
        esat::DrawSetFillColor(255,255,255,10);
        esat::DrawSetStrokeColor(255,255,255,255);
        esat::DrawSolidPath(&sqPoints[0].x, 4);

        esat::DrawSetFillColor(255,255,255,255);
      }
      
      

      

      
      esat::DrawSetTextSize(p->fontSize);
    
      finalTextPos.y = p->pos.y + (p->dimensions.y / 2);  // 1 fontsize = 0.75pixels
      finalTextPos.y -= (p->dimensions.y -  p->fontSize * 0.75)/2;

      finalTextPos.x = p->pos.x - (p->dimensions.x / 2);
      finalTextPos.x +=  (p->dimensions.x - strlen(p->text)*((p->fontSize/2) + 4))/2;   // widthLetter = (fontsize / 2) + 4

      esat::DrawText(finalTextPos.x , finalTextPos.y, p->text);
    }
  
    p = p->next;
  }
  
  


}

// ----------------------------------- Input -----------------------------------

void input()
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
  for (int i = 0; i < numAsteroidsToGenerate; i++)
  {
    ast::InsertList(&asteroidList, randomAsteroid());
  }
}

void CEO()
{

  
  switch (GAMESTATE)
  {
  case MENU:
    
    esat::DrawSetFillColor(255,255,255,255);
    esat::DrawSetTextSize(80);
    esat::DrawText(190,200,"asteroids");
    break;

  case LOGIN:

    break;

  case SIGNUP:

    break;

  case INGAME:

    input();

    shotsManager();

    asteroidsManager();

    ufoManager();

    shipManager();

    paintGUI();
    break;

  default:
    break;
  }
  paintMenu();
}

int esat::main(int argc, char **argv)
{

  srand(time(NULL)); // RANDOMNESS

  esat::WindowInit(800, 800);
  WindowSetMouseVisibility(true);

  initAstData();
  initShip(&ship);
  init();
  initInterfaceData();

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
