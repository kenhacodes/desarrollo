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
const int knPoints = 10;
const int knPointsCircle = 30;
const int knPointsHex = 6;
esat::Vec3 g_points[knPoints];
esat::Vec3 g_pointsCircle[knPointsCircle];  
esat::Vec3 g_pointsHex[knPointsHex];
void InitFigures()
{
  float points[20] = {
      0.0f, 0.0f, 0.45f, 0.0f,
      0.8f, 0.35f, 0.9f, 0.35f,
      1.0f, 0.25f, 1.0f, 0.35f,
      0.9f, 0.45f, 0.8f, 0.45f,
      0.45f, 0.1f, 0.0f, 0.1f};

  for (int i = 0; i < knPoints * 2; i += 2)
  {
    g_points[i / 2] = esat::Vec3{points[i], points[i + 1], 1.0f};
  }

  for (int i = 0; i < knPointsCircle; i++)
  {
    g_pointsCircle[i] = {cosf((6.28f / knPointsCircle) * i), sinf((6.28f / knPointsCircle) * i), 1.0f};
  }

  for (int i = 0; i < knPointsHex; i++)
  {
     g_pointsHex[i] = {cosf((6.28f / 6) * i), sinf((6.28f / 6) * i), 1.0f};
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

void DrawFigure(const int n_points, esat::Vec3 *points, esat::Mat3 m, bool solidPath)
{
  esat::Vec2 dr_points[knMaxPoints];

  for (int i = 0; i < n_points; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_points[i] = {tmp.x, tmp.y};
  }
  esat::DrawSetFillColor(255, 255, 255, 20);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  if (solidPath)
  {
   esat::DrawSolidPath(&dr_points[0].x, n_points);
  }else{
    esat::DrawPath(&dr_points[0].x, n_points);
  }
  
  
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
    
    esat::Mat3 m = BasicTransform({2.0f, 2.0f}, 0, {(float)esat::MousePositionX(), (float)esat::MousePositionY()});

    esat::Mat3 circle = esat::Mat3Identity();
    circle = esat::Mat3Multiply(esat::Mat3Scale(50, 50), circle);
    circle = esat::Mat3Multiply(m, circle);

    DrawFigure(knPointsCircle, g_pointsCircle, circle, true);
    
    for (int i = 0; i < 7; i++)
    {
      esat::Mat3 c = esat::Mat3Identity();

      c = esat::Mat3Multiply(esat::Mat3Rotate(3.14f/2), c);
      c = esat::Mat3Multiply(esat::Mat3Translate(0, 1), c);
      c = esat::Mat3Multiply(esat::Mat3Scale(50, 50), c);
      c = esat::Mat3Multiply(esat::Mat3Rotate((6.28f / 7) * i), c);

      c = esat::Mat3Multiply(m, c);

      esat::Mat3 h = esat::Mat3Identity();
      h = esat::Mat3Multiply(esat::Mat3Rotate(-3.14f/3), h);
      h = esat::Mat3Multiply(esat::Mat3Translate(8, 0), h);
      //h = esat::Mat3Multiply(esat::Mat3Rotate(3.14f/2), h);
      h = esat::Mat3Multiply(esat::Mat3Scale(0.1, 0.1), h);
      h = esat::Mat3Multiply(c, h);

      DrawFigure(knPointsHex, g_pointsHex, h, false);


      DrawFigure(knPoints, g_points, c, true);
    }
    for (int i = 0; i < 7; i++)
    {
      esat::Mat3 c = esat::Mat3Identity();
      
      c = esat::Mat3Multiply(esat::Mat3Rotate(3.14f/2), c);
      c = esat::Mat3Multiply(esat::Mat3Translate(0, 1), c);
      c = esat::Mat3Multiply(esat::Mat3Scale(50, -50), c);
      c = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f / 7) * i) + 3.14/7), c);
      
      c = esat::Mat3Multiply(m, c);

      DrawFigure(knPoints, g_points, c, true);
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
