#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include "math.h"

float kwidth = 800.0f;
float kheight = 800.0f;

unsigned char fps = 60; // Control de frames por segundo
double current_time, last_time;
const int knMaxPoints = 50;

const int knArrow = 4;
const int knSquare = 4;
const int knHex = 6;

esat::Vec3 g_arrow[knArrow];
esat::Vec3 g_square[knSquare];
esat::Vec3 g_hex[knHex];

void InitFigures()
{
  float arrow[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, -3.0f,
      -1.0f, 0.0f};
  float square[] = {
      -1.0f, -1.0f,
      1.0f, -1.0f,
      1.0f, 1.0f,
      -1.0f, 1.0f,
      -1.0f, -1.0f};

  for (int i = 0; i < knArrow * 2; i += 2)
  {
    g_arrow[i / 2] = {arrow[i], arrow[i + 1], 1.0f};
  }
  for (int i = 0; i < knSquare * 2; i += 2)
  {
    g_square[i / 2] = {square[i], square[i + 1], 1.0f};
  }
  for (int i = 0; i < knHex; i++)
  {
    g_hex[i] = {cosf((6.28f / knHex) * i), sinf((6.28f / knHex) * i), 1.0f};
  }
}

esat::Mat3 BasicTransform(esat::Vec2 scale, float rotation, esat::Vec2 pos)
{
  esat::Mat3 m = esat::Mat3Identity();
  m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(rotation), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
  return m;
}

void DrawFigure(const int n_points, esat::Vec3 *points, esat::Mat3 m)
{
  esat::Vec2 dr_points[knMaxPoints];

  for (int i = 0; i < n_points; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_points[i] = {tmp.x, tmp.y};
  }
  esat::DrawSetFillColor(255, 255, 255, 20);
  esat::DrawSetStrokeColor(255, 255, 255, 255);

  esat::DrawSolidPath(&dr_points[0].x, n_points);
}

int esat::main(int argc, char **argv)
{
  esat::WindowInit(kwidth, kheight);
  WindowSetMouseVisibility(true);

  InitFigures();

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0);

    // --------------

    esat::Mat3 m = BasicTransform({1, 1}, 0, {400.0f, 400.0f});

    esat::Mat3 sq = esat::Mat3Identity();
    sq = esat::Mat3Multiply(esat::Mat3Scale(50.0f, 50.0f), sq);
    sq = esat::Mat3Multiply(m, sq);

    DrawFigure(knSquare, g_square, sq);
    DrawFigure(knHex, g_hex, sq);

    for (int j = 0; j < 4; j++)
    {
      esat::Mat3 side = esat::Mat3Identity();
      side = esat::Mat3Multiply(esat::Mat3Rotate((6.28f / 4) * j), side);
      side = esat::Mat3Multiply(m, side);

      for (int i = -2; i < 3; i++)
      {
        esat::Mat3 a = esat::Mat3Identity();
        a = esat::Mat3Multiply(esat::Mat3Translate(i * 2.5, 10), a);
        a = esat::Mat3Multiply(esat::Mat3Scale(8.0f, 8.0f), a);
        a = esat::Mat3Multiply(side, a);
        DrawFigure(knArrow, g_arrow, a);
      }
      for (int i = -2; i < 3; i++)
      {
        esat::Mat3 a = esat::Mat3Identity();
        a = esat::Mat3Multiply(esat::Mat3Scale(0.9f, 2.0f), a);
        a = esat::Mat3Multiply(esat::Mat3Translate(i * 2.5, 14), a);
        a = esat::Mat3Multiply(esat::Mat3Scale(8.0f, 8.0f), a);
        a = esat::Mat3Multiply(side, a);
        DrawFigure(knSquare, g_square, a);
      }

      esat::Mat3 a = esat::Mat3Identity();
      a = esat::Mat3Multiply(esat::Mat3Translate(0, -3.5), a);
      a = esat::Mat3Multiply(esat::Mat3Scale(12.0f, 40.0f), a);
      a = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 4) * j) + 3.14 / 4), a);

      a = esat::Mat3Multiply(m, a);
      DrawFigure(knArrow, g_arrow, a);
    }

    // --------------
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
