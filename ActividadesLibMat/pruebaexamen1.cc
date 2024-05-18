#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include "math.h"

float kwidth = 800.0f;
float kheight = 800.0f;
float height = 0;

esat::Vec3 g_points_Square[4];
esat::Vec3 g_points_Hex[6];
esat::Vec3 g_points_Arrow[4];

esat::Vec2 dr_points_Square[4];
esat::Vec2 dr_points_Hex[6];
esat::Vec2 dr_points_Arrow[4];

esat::Mat3 m;

void initFigs()
{
  g_points_Square[0] = {-1.0f, 1.0f, 1.0f};
  g_points_Square[1] = {-1.0f, -1.0f, 1.0f};
  g_points_Square[2] = {1.0f, -1.0f, 1.0f};
  g_points_Square[3] = {1.0f, 1.0f, 1.0f};

  for (int i = 0; i < 6; i++)
  {

    g_points_Hex[i] = {cosf((6.28f / 6) * i), sinf((6.28f / 6) * i), 1.0f};
  }

  g_points_Arrow[0] = {-1.0f, 0.0f, 1.0f};
  g_points_Arrow[1] = {0.0f, -0.5f, 1.0f};
  g_points_Arrow[2] = {-0.5f, 0.0f, 1.0f};
  g_points_Arrow[3] = {0.0f, +0.5f, 1.0f};
}

esat::Mat3 Base()
{
  esat::Vec2 mouse = {(float)esat::MousePositionX(),
                      (float)esat::MousePositionY()};

  esat::Mat3 m = esat::Mat3Identity();
  m = esat::Mat3Multiply(esat::Mat3Scale(40.0f, 40.0f), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(mouse.x, mouse.y), m);

  return m;
}

void drawSquare()
{

  for (int i = 0; i < 4; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_points_Square[i]);
    dr_points_Square[i] = {tmp.x, tmp.y};
    ;
  }

  esat::DrawSetStrokeColor(255, 0, 0, 255);
  esat::DrawSetFillColor(0, 0, 0, 0);
  esat::DrawSolidPath(&dr_points_Square[0].x, 4);
}

void drawHex()
{
  for (int i = 0; i < 6; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_points_Hex[i]);
    dr_points_Hex[i] = {tmp.x, tmp.y};
  }
  esat::DrawSetStrokeColor(255, 0, 0, 255);
  esat::DrawSetFillColor(50, 0, 0, 0);
  esat::DrawSolidPath(&dr_points_Hex[0].x, 6);
}

void drawEdgeFigure(esat::Mat3 base)
{
  for (int i = 0; i < 4; i++)
  {
    esat::Mat3 nm = esat::Mat3Identity();

    nm = esat::Mat3Multiply(esat::Mat3Rotate((6.28f / 4) * i), nm);
    //nm = esat::Mat3Multiply(esat::Mat3Scale(10.0f, 10.0f), nm);
    nm = esat::Mat3Multiply(esat::Mat3Translate(200, 0), nm);
    nm = (nm,base);
    
    for (int j = 0; j < 4; j++)
    {
      esat::Vec3 tmp = esat::Mat3TransformVec3(nm, g_points_Arrow[j]);
      dr_points_Arrow[j] = {tmp.x, tmp.y};
    }
    
    esat::DrawSetStrokeColor(255, 0, 0, 255);
    esat::DrawSetFillColor(50, 0, 0, 0);
    esat::DrawSolidPath(&dr_points_Arrow[0].x, 4);
  }
}

unsigned char fps = 60; // Control de frames por segundo
double current_time, last_time;

int esat::main(int argc, char **argv)
{
  esat::WindowInit(kwidth, kheight);
  WindowSetMouseVisibility(true);

  initFigs();

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0);
    // --------------

    m = Base();
    drawSquare();
    drawHex();
    drawEdgeFigure(m);

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
