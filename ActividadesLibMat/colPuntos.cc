#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "zorolib.h"

struct rgb
{
  float r, g, b;
};

struct TShip
{
  zoro::Vec2 pos = {400.0f, 400.0f};
  zoro::Vec2 speed = {0.0f, 0.0f};
  zoro::Vec2 g_points[3];
  float radius = 150.0f;
  float angle = 0.0f;
  zoro::Vec2 acceleration = {0.0f, 0.0f};
  rgb color = {255, 5, 5};
};

TShip ship;

void paintShip(TShip ship)
{

  ship.g_points[0].x = cos(0 + ship.angle) * ship.radius + ship.pos.x;
  ship.g_points[0].y = sin(0 + ship.angle) * ship.radius + ship.pos.y;

  ship.g_points[1].x = cos(2.7 + ship.angle) * ship.radius + ship.pos.x;
  ship.g_points[1].y = sin(2.7 + ship.angle) * ship.radius + ship.pos.y;

  ship.g_points[2].x = cos(-2.7 + ship.angle) * ship.radius + ship.pos.x;
  ship.g_points[2].y = sin(-2.7 + ship.angle) * ship.radius + ship.pos.y;

  zoro::Vec2 a, b, p;
  p.x = esat::MousePositionX();
  p.y = esat::MousePositionY();

  bool found = false;
  float res;
  for (int i = 0; i < 3; i++)
  {
    a = ship.g_points[i];
    if (i == 2)
      b = ship.g_points[0];
    else
      b = ship.g_points[i + 1];

    // if dot((zoro::SubtractVec2(p,a)) , zoro::RightPerpendicularVec2(zoro::NormalizeVec2( zoro::SubtractVec2(b,a) ) )   );

    if (zoro::DotVec2((zoro::SubtractVec2(p, a)), zoro::RightPerpendicularVec2(zoro::NormalizeVec2(zoro::SubtractVec2(b, a)))) > 1)
      found = true;
  }

  if (found)
  {
    esat::DrawSetStrokeColor(255, 0, 0, 255);
    esat::DrawSetFillColor(255, 0, 0, 255);
  }
  else
  {
    esat::DrawSetStrokeColor(ship.color.r, ship.color.g, ship.color.b, 50);
    esat::DrawSetFillColor(ship.color.r, ship.color.g, ship.color.b, 50);
  }
  esat::DrawSolidPath(&ship.g_points[0].x, 3);
}

unsigned char fps = 60; // Control de frames por segundo
double current_time, last_time;

int esat::main(int argc, char **argv)
{

  esat::WindowInit(800, 800);
  WindowSetMouseVisibility(true);

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0);

    paintShip(ship);

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
