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

const int knPoints = 5;
const int knMaxPoints = 50;

esat::Vec3 g_points[knPoints];

void InitFigures(){

  for (int i = 0; i < knPoints; i++)
  {
    g_points[i] = {cosf((6.28f / knPoints) * i), sinf(((6.28f / knPoints) * i)), 1.0f};
  }
  

}

esat::Mat3 BasicTransform(esat::Vec2 scale, float angle ,esat::Vec2 pos){
  esat::Mat3 m = esat::Mat3Identity();

  m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
  return m;
}

void DrawFigure(const int n_points, esat::Vec3* points, esat::Mat3 m){
  esat::Vec2 dr_points[knMaxPoints];

  for (int i = 0; i < n_points; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_points[i] = {tmp.x,  tmp.y};
  }
  esat::DrawSetFillColor(255,255,255,20);
  esat::DrawSetStrokeColor(255,255,255,255);
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

    esat::Mat3 m = BasicTransform({1.0f,1.0f}, 0, {(float) esat::MousePositionX(), (float) esat::MousePositionY()});
    
    esat::Mat3 b = esat::Mat3Identity();

    b = esat::Mat3Multiply(esat::Mat3Scale(50,50), b);
    b = esat::Mat3Multiply(m, b);

    DrawFigure(knPoints, g_points, b);

    for (int i = 0; i < knPoints; i++)
    {
      esat::Mat3 c = esat::Mat3Identity();
      
      
      c = esat::Mat3Multiply(esat::Mat3Scale(0.4,0.4), c);
      c = esat::Mat3Multiply(esat::Mat3Rotate( ((6.28f / knPoints) * i ) + 3.14f), c);
      c = esat::Mat3Multiply(esat::Mat3Translate(1.4,0),c);
      c = esat::Mat3Multiply(esat::Mat3Rotate( (6.28f / knPoints) * i ), c);
      
      
      c = esat::Mat3Multiply(b, c);
      DrawFigure(knPoints, g_points, c);
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
