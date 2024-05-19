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
const int kMaxPoints = 50;
const int knPoints = 10;
const int knArrowPoints = 7;
esat::Vec3 g_circlepoints[knPoints];
esat::Vec3 g_arrow[knArrowPoints];

void initCirclePoints()
{
  for (int i = 0; i < knPoints; i++)
  {
    g_circlepoints[i] = {cosf((6.28f / knPoints) * i), sinf((6.28f / knPoints) * i), 1.0f};
  }
  float points[14] = {
      0.0f, 2.0f,
      -5.0f, -8.0f,
      -8.0f, -8.0f,
      0.0f, 8.0f,
      8.0f, -8.0f,
      5.0f, -8.0f,
      0.0f, 2.0f};

for (int i = 0; i < knArrowPoints * 2; i += 2)
{
    g_arrow[i / 2] = esat::Vec3{points[i], points[i + 1], 1.0f}; // Use the correct constructor
}
}

esat::Mat3 BasicTransform(esat::Vec2 scale, float angle, esat::Vec2 pos)
{

  esat::Mat3 m = esat::Mat3Identity();
  m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
  return m;
}

// Funcion generica de dibujado de figura
void DrawFigure(const int n_points, esat::Vec3 *points, esat::Mat3 m)
{

  esat::Vec2 dr_Points[kMaxPoints];
  for (int i = 0; i < n_points; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_Points[i] = {tmp.x, tmp.y};
  }

  esat::DrawSetFillColor(255, 255, 255, 0);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSolidPath(&dr_Points[0].x, n_points);
}

int esat::main(int argc, char **argv)
{
  esat::WindowInit(kwidth, kheight);
  WindowSetMouseVisibility(true);

  initCirclePoints();

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0);

    // --------------

    esat::Mat3 m = BasicTransform({1.0f, 1.0f}, 0.0f, {(float)esat::MousePositionX(), (float)esat::MousePositionY()});
    // DrawFigure(knPoints, g_circlepoints, m);
    for (int i = 0; i < 14; i++)
    {
      esat::Mat3 innerM = Mat3Identity();
      
      innerM = esat::Mat3Multiply(esat::Mat3Translate(0, 25), innerM);
      innerM = esat::Mat3Multiply(esat::Mat3Scale(10, 10), innerM);
      innerM = esat::Mat3Multiply(esat::Mat3Rotate((6.28f / 14) * i), innerM);
      innerM = esat::Mat3Multiply(m, innerM);
      
       DrawFigure(knArrowPoints, g_arrow, innerM);
    }

    for (int i = 0; i < 6; i++)
    {
      esat::Mat3 innerM = Mat3Identity();
      innerM = esat::Mat3Multiply(esat::Mat3Rotate(3.14), innerM);
      innerM = esat::Mat3Multiply(esat::Mat3Translate(0, 25), innerM);
      innerM = esat::Mat3Multiply(esat::Mat3Scale(3.0f, 3.0f), innerM);
      innerM = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 6) * i) ), innerM);
      
      innerM = esat::Mat3Multiply(m, innerM);
      
       DrawFigure(knArrowPoints, g_arrow, innerM);
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
