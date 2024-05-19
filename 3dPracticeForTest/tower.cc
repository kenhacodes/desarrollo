#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "esat/window.h"
#include "esat/draw.h"
#include "esat/input.h"
#include "esat/sprite.h"
#include "esat/time.h"

const unsigned int kWindowWidth = 800;
const unsigned int kWindowHeight = 800;

const int kNPoints = 4;
esat::Vec4 g_points[kNPoints * 2];

void InitPoints()
{
  // Front face:
  g_points[0] = {-1.0f, 1.0f, 1.0f, 1.0f};
  g_points[1] = {1.0f, 1.0f, 1.0f, 1.0f};
  g_points[2] = {1.0f, -1.0f, 1.0f, 1.0f};
  g_points[3] = {-1.0f, -1.0f, 1.0f, 1.0f};

  // Front face:
  g_points[4] = {-1.0f, 1.0f, -1.0f, 1.0f};
  g_points[5] = {1.0f, 1.0f, -1.0f, 1.0f};
  g_points[6] = {1.0f, -1.0f, -1.0f, 1.0f};
  g_points[7] = {-1.0f, -1.0f, -1.0f, 1.0f};
}

esat::Mat4 BasicTransform(int i, int j)
{
  esat::Mat4 m = esat::Mat4Identity();
  //m = esat::Mat4Multiply(esat::Mat4Scale(1.0f, 1.0f, 3.0f), m);


  m = esat::Mat4Multiply(esat::Mat4Translate(7.0f, j*5.0f, 0.0f), m);
  m = esat::Mat4Multiply(esat::Mat4Scale(0.25f, 0.25f, 0.5f), m);
  if (j % 2 == 0)
  {
     m = esat::Mat4Multiply(esat::Mat4RotateY(((6.28f / 6) * i) + 3.14f/6), m);
  }else{
     m = esat::Mat4Multiply(esat::Mat4RotateY((6.28f / 6) * i), m);
  }
  
 
  

  m = esat::Mat4Multiply(esat::Mat4RotateY(esat::Time() * 0.001f), m);
  m = esat::Mat4Multiply(esat::Mat4Translate(0.0f, 0.0f, 2.5f), m);


  m = esat::Mat4Multiply(esat::Mat4Projection(), m);
  m = esat::Mat4Multiply(esat::Mat4Scale(120.0f, 120.0f, 1.0f), m);
  m = esat::Mat4Multiply(esat::Mat4Translate(400.0f, 400.0f, 0.0f), m);
  return m;
}

esat::Mat4 UpdateFigure()
{

  esat::Vec2 mouse = {(float)esat::MousePositionX(),
                      (float)esat::MousePositionY()};
  esat::Mat4 m = esat::Mat4Identity();
  // P = T * S * Proj * T(z) * P
  m = esat::Mat4Multiply(esat::Mat4RotateY(esat::Time() * 0.001f), m);
  m = esat::Mat4Multiply(esat::Mat4Translate(0.0f, 0.0f, 5.0f), m);
  m = esat::Mat4Multiply(esat::Mat4Projection(), m);
  //m = esat::Mat4Multiply(esat::Mat4Scale(100.0f, 100.0f, 1.0f), m);
  //m = esat::Mat4Multiply(esat::Mat4Translate(400.0f, 400.0f, 0.0f), m);
  return m;
}

void DrawFigure(esat::Mat4 m)
{

  esat::Vec2 tr_points[kNPoints * 2];

  for (int i = 0; i < kNPoints * 2; ++i)
  {
    esat::Vec4 tmp = esat::Mat4TransformVec4(m, g_points[i]);
    float r = 1.0f / tmp.w;
    tmp = {tmp.x * r, tmp.y * r, tmp.z * r, tmp.w * r};
    tr_points[i] = {tmp.x, tmp.y};
  }

  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 0);
  for (int i = 0; i < kNPoints; ++i)
  {

    // i | (i + 1) % kNPoints
    esat::DrawLine(tr_points[i].x, tr_points[i].y, tr_points[(i + 1) % kNPoints].x, tr_points[(i + 1) % kNPoints].y);
    // i | (i + kNPoints)
    esat::DrawLine(tr_points[i].x, tr_points[i].y, tr_points[i + kNPoints].x, tr_points[i + kNPoints].y);
    // i + kNPoints | (i + 1) % kNPoints + kNPoints
    esat::DrawLine(tr_points[i + kNPoints].x, tr_points[i + kNPoints].y, tr_points[(i + 1) % kNPoints + kNPoints].x, tr_points[(i + 1) % kNPoints + kNPoints].y);
  }
}

int esat::main(int argc, char **argv)
{
  srand(time(nullptr));
  double current_time = 0.0;
  double last_time = 0.0;
  double fps = 60.0;

  esat::WindowInit(kWindowWidth, kWindowHeight);
  esat::WindowSetMouseVisibility(true);
  InitPoints();

  while (!esat::IsSpecialKeyDown(esat::kSpecialKey_Escape) &&
         esat::WindowIsOpened())
  {
    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(120, 120, 120);

    // ---------------------------------------- //
    for (int i = 0; i < 6; i++)
    {
      for (int j = -10; j < 10; j++)
      {
        esat::Mat4 m = BasicTransform(i,j);

    DrawFigure(m);
      }
      
      
    }
    
    

    // ---------------------------------------- //

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