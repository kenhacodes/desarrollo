#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "esat/window.h"
#include "esat/input.h"
#include "esat/draw.h"
#include "esat/math.h"
#include "esat/time.h"

const unsigned int kWindow_X = 800;
const unsigned int kWindow_Y = 600;

// Some global variables?

const int knMaxPoints = 50;
const int knArrowPoints = 7;
const int knTrianglePoints = 3;
const int knStarPoints = 10;

esat::Vec3 g_arrow[knArrowPoints];
esat::Vec3 g_Triangle[knTrianglePoints];
esat::Vec3 g_circle[knMaxPoints];
esat::Vec4 g_star[knStarPoints * 2];

// Create other functions?

void InitFigures()
{
  // 1
  for (int i = 0; i < knTrianglePoints; i++)
  {
    g_Triangle[i] = {cosf((6.28f / knTrianglePoints) * i), sinf((6.28f / knTrianglePoints) * i), 1.0f};
  }
  g_arrow[0] = {1.0f, 0.0f, 1.0f};
  g_arrow[1] = {0.0f, -1.0f, 1.0f};
  g_arrow[2] = {0.0f, -0.5f, 1.0f};
  g_arrow[3] = {-1.0f, -0.5f, 1.0f};
  g_arrow[4] = {-1.0f, +0.5f, 1.0f};
  g_arrow[5] = {0.0f, +0.5f, 1.0f};
  g_arrow[6] = {0.0f, 1.0f, 1.0f};

  // 2
  for (int i = 0; i < knMaxPoints; i++)
  {
    g_circle[i] = {cosf((6.28f / knMaxPoints) * i), sinf((6.28f / knMaxPoints) * i), 1.0f};
  }

  // 3

  for (int i = 0; i < knStarPoints; i++)
  {
    if (i % 2 == 0)
    {
      g_star[i] = {cosf((6.28f / knStarPoints) * i),
                   sinf((6.28f / knStarPoints) * i),
                   0.5f, 1.0f};

      g_star[i + knStarPoints] = {cosf((6.28f / knStarPoints) * i),
                                  sinf((6.28f / knStarPoints) * i),
                                  -0.5f, 1.0f};
    }
    else
    {
      g_star[i] = {cosf((6.28f / knStarPoints) * i) / 2, sinf((6.28f / knStarPoints) * i) / 2, 0.5f, 1.0f};
      g_star[i + knStarPoints] = {cosf((6.28f / knStarPoints) * i) / 2, sinf((6.28f / knStarPoints) * i) / 2, -0.5f, 1.0f};
    }
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

void DrawFigure2D(const int n_points, esat::Vec3 *points, esat::Mat3 m)
{

  esat::Vec2 dr_points[knMaxPoints];
  for (int i = 0; i < n_points; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_points[i] = {tmp.x, tmp.y};
  }
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 0);
  esat::DrawSolidPath(&dr_points[0].x, n_points);
}

void DrawFigure3D(esat::Mat4 m)
{
  esat::Vec2 tr_points[knStarPoints * 2];

  for (int i = 0; i < knStarPoints * 2; i++)
  {
    esat::Vec4 tmp = esat::Mat4TransformVec4(m, g_star[i]);
    float r = 1.0f / tmp.w;
    tmp = {tmp.x * r, tmp.y * r, tmp.z * r, tmp.w * r};
    tr_points[i] = {tmp.x, tmp.y};
  }
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 0);
  for (int i = 0; i < knStarPoints; i++)
  {
    // i | i+1 % knPoints
    esat::DrawLine(tr_points[i].x, tr_points[i].y, tr_points[(i + 1) % knStarPoints].x, tr_points[(i + 1) % knStarPoints].y);
    // i | i+knpoints
    esat::DrawLine(tr_points[i].x, tr_points[i].y, tr_points[i + knStarPoints].x, tr_points[i + knStarPoints].y);
    // i + knpoints | i+1 % knPoints + knpoints
    esat::DrawLine(tr_points[i + knStarPoints].x, tr_points[i + knStarPoints].y, tr_points[((i + 1) % knStarPoints) + knStarPoints].x, tr_points[((i + 1) % knStarPoints) + knStarPoints].y);
  }
}

void FirstTransformation()
{
  esat::Mat3 m = BasicTransform({1.0f, 1.0f}, -3.14 / 2, {400.0f, 300.0f});

  for (int i = 0; i < 3; i++)
  {
    esat::Mat3 a = esat::Mat3Identity();
    a = esat::Mat3Multiply(esat::Mat3Translate(-4, 0), a);

    a = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 3) * i) + 3.14f / 3), a);

    a = esat::Mat3Multiply(esat::Mat3Scale(20, 20), a);

    a = esat::Mat3Multiply(m, a);
    DrawFigure2D(knArrowPoints, g_arrow, a);
  }

  esat::Mat3 triangle = esat::Mat3Identity();

  triangle = esat::Mat3Multiply(esat::Mat3Scale(60, 60), triangle);

  triangle = esat::Mat3Multiply(m, triangle);

  DrawFigure2D(knTrianglePoints, g_Triangle, triangle);
}

void SecondTransformation()
{
  // Program me!
  esat::Mat3 m = BasicTransform({1, 1}, 3.14f / 2, {400, 300});

  esat::Mat3 outer = esat::Mat3Identity();
  outer = esat::Mat3Multiply(esat::Mat3Scale(150, 150), outer);
  outer = esat::Mat3Multiply(m, outer);

  DrawFigure2D(knMaxPoints, g_circle, outer);

  esat::Mat3 midlle = esat::Mat3Identity();
  midlle = esat::Mat3Multiply(esat::Mat3Scale(100, 100), midlle);
  midlle = esat::Mat3Multiply(m, midlle);

  DrawFigure2D(knMaxPoints, g_circle, midlle);

  esat::Mat3 inner = esat::Mat3Identity();
  inner = esat::Mat3Multiply(esat::Mat3Scale(50, 50), inner);
  inner = esat::Mat3Multiply(m, inner);

  DrawFigure2D(knMaxPoints, g_circle, inner);

  for (int i = 0; i < 4; i++)
  {
    esat::Mat3 center = esat::Mat3Identity();
    center = esat::Mat3Multiply(esat::Mat3Scale(15, 25), center);
    center = esat::Mat3Multiply(esat::Mat3Translate(0, 25), center);
    center = esat::Mat3Multiply(esat::Mat3Rotate((6.28f / 4) * i), center);
    center = esat::Mat3Multiply(m, center);

    DrawFigure2D(knMaxPoints, g_circle, center);
  }

  for (int i = 0; i < 5; i++)
  {
    esat::Mat3 innerArrow = esat::Mat3Identity();

    innerArrow = esat::Mat3Multiply(esat::Mat3Scale(10, 10), innerArrow);
    innerArrow = esat::Mat3Multiply(esat::Mat3Translate(0, 75), innerArrow);
    innerArrow = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 5) * i) + 3.14 / 2), innerArrow);

    innerArrow = esat::Mat3Multiply(m, innerArrow);

    DrawFigure2D(knArrowPoints, g_arrow, innerArrow);
  }

  for (int i = 0; i < 5; i++)
  {
    esat::Mat3 innerArrow = esat::Mat3Identity();

    innerArrow = esat::Mat3Multiply(esat::Mat3Scale(10, 10), innerArrow);
    innerArrow = esat::Mat3Multiply(esat::Mat3Translate(0, -75), innerArrow);
    innerArrow = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 5) * i) + 3.14 / 2), innerArrow);

    innerArrow = esat::Mat3Multiply(m, innerArrow);

    DrawFigure2D(knArrowPoints, g_arrow, innerArrow);
  }

  for (int i = 0; i < 10; i++)
  {
    esat::Mat3 innerArrow = esat::Mat3Identity();

    innerArrow = esat::Mat3Multiply(esat::Mat3Scale(10, 10), innerArrow);
    innerArrow = esat::Mat3Multiply(esat::Mat3Translate(0, -125), innerArrow);
    innerArrow = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 10) * i) + 3.14f), innerArrow);
    innerArrow = esat::Mat3Multiply(m, innerArrow);

    DrawFigure2D(knArrowPoints, g_arrow, innerArrow);
  }
}

void ThirdTransformation()
{
  for (int i = 0; i < 10; i++)
  {
    esat::Mat4 m = esat::Mat4Identity();
    m = esat::Mat4Multiply(esat::Mat4RotateY(3.14f/2), m);
    m = esat::Mat4Multiply(esat::Mat4RotateX((3.14f / 10) + 3.14f), m);
    m = esat::Mat4Multiply(esat::Mat4Translate(8, 0, 0), m);
    m = esat::Mat4Multiply(esat::Mat4RotateY(((6.28f / 10) * i) ), m);
    m = esat::Mat4Multiply(esat::Mat4RotateY(esat::Time() * 0.001f), m);
    m = esat::Mat4Multiply(esat::Mat4Translate(0, -2, 18), m);
    m = esat::Mat4Multiply(esat::Mat4Projection(), m);
    m = esat::Mat4Multiply(esat::Mat4Scale(500, 500, 1), m);
    m = esat::Mat4Multiply(esat::Mat4Translate(400, 300, 0), m);

    DrawFigure3D(m);
  }
}

int esat::main(int argc, char **argv)
{
  double current_time = 0.0;
  double last_time = 0.0;
  double fps = 30.0;

  esat::WindowInit(kWindow_X, kWindow_Y);
  esat::WindowSetMouseVisibility(true);
  int scene_index = 0;

  // Some data initialization in this point?
  InitFigures();

  // Main loop.
  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {
    last_time = esat::Time();

    esat::DrawBegin();
    esat::DrawClear(0, 0, 0);

    // Scene up!!
    if (esat::MouseButtonUp(0))
    {
      scene_index = scene_index + 1 > 2 ? 0 : scene_index + 1;
    } // Scene down!!
    else if (esat::MouseButtonUp(1))
    {
      scene_index = scene_index - 1 < 0 ? 2 : scene_index - 1;
    }

    // It's allowed to add parameters to any function.
    switch (scene_index)
    {
    case 0:
      FirstTransformation();
      break;
    case 1:
      SecondTransformation();
      break;
    case 2:
      ThirdTransformation();
      break;
    }

    esat::DrawEnd();
    esat::WindowFrame();

    do
    {
      current_time = esat::Time();
    } while ((current_time - last_time) <= 1000.0 / fps);
  }

  esat::WindowDestroy();
  return 0;
}