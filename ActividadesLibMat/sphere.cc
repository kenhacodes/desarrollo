#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include "math.h"

float kwidth = 800.0f;
float kheight = 800.0f;

int height = 10;
int revolutions = 10;

const int kNPoints = 4;
esat::Vec4 g_points[kNPoints * 2];

void initCube() {
  g_points[0] = {-1.0f, 1.0f, 1.0f, 1.0f}; 
  g_points[1] = {1.0f, 1.0f, 1.0f, 1.0f};
  g_points[2] = {1.0f, -1.0f, 1.0f, 1.0f};
  g_points[3] = {-1.0f, -1.0f, 1.0f, 1.0f};

  g_points[4] = {-1.0f, 1.0f, -1.0f, 1.0f};
  g_points[5] = {1.0f, 1.0f, -1.0f, 1.0f};
  g_points[6] = {1.0f, -1.0f, -1.0f, 1.0f};
  g_points[7] = {-1.0f, -1.0f, -1.0f, 1.0f};
}

esat::Mat4 basic(int y, int j, int offset){
  
  esat::Mat4 m = esat::Mat4Identity();
  
  m = esat::Mat4Multiply(esat::Mat4Scale(1.0f, 1.0f, 3.0f), m);

  m = esat::Mat4Multiply(esat::Mat4Translate(20.0f, j* 6, 0.0f), m);

  m = esat::Mat4Multiply(esat::Mat4RotateY( esat::Time() * 0.001f + (6.28f / 6 * y) + offset),m);

  m = esat::Mat4Multiply(esat::Mat4Translate(0.0f, 0.0f, 33.0f), m);


  m = esat::Mat4Multiply(esat::Mat4Projection(), m);


  m = esat::Mat4Multiply(esat::Mat4Scale(150.0f, 150.0f, 1.0f), m);

  m = esat::Mat4Multiply(esat::Mat4Translate(400.0f, 400.0f + height, 0.0f), m);
  
  
  return m;
}

void DrawFigure(esat::Mat4 m) {
  
  esat::Vec2 dr_points[kNPoints * 2];

  for (int i = 0; i < kNPoints * 2; i++)
  {
    esat::Vec4 tmp = esat::Mat4TransformVec4(m, g_points[i]);
    float r = 1.0f / tmp.w;
    tmp = {tmp.x * r, tmp.y * r, tmp.z * r, tmp.w * r};
    dr_points[i] = { tmp.x, tmp.y};
  }
  
  esat::DrawSetStrokeColor(255,255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  for (int i = 0; i < kNPoints ; i++)
  {
    // Current to next
    esat::DrawLine(dr_points[i].x, dr_points[i].y,
                  dr_points[(i + 1) % kNPoints].x, dr_points[(i + 1) % kNPoints].y);
    // Current to back
    esat::DrawLine(dr_points[i].x, dr_points[i].y,
                  dr_points[(i + kNPoints)].x, dr_points[(i + kNPoints)].y);
    // Back to next back
    esat::DrawLine(dr_points[(i + kNPoints)].x, dr_points[(i + kNPoints)].y,
                  dr_points[(i + 1) % kNPoints + kNPoints].x, dr_points[(i + 1) % kNPoints + kNPoints].y);
  }
}

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

int esat::main(int argc, char **argv) {
	esat::WindowInit(kwidth,kheight);
	WindowSetMouseVisibility(true);
  
  initCube();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
		
		// --------------
    for (int i = 0; i < 6; i++)
    {
      for (int j = -50; j < 50; j++)
      {
        if (j % 2 == 0)
        {
          esat::Mat4 b = basic(i, j , 10);
          DrawFigure(b);
        }else{
          esat::Mat4 b = basic(i, j , 0);
          DrawFigure(b);
        }
      }  
    }

    

    //DrawFigure(inner);
    //DrawFigure(innerininner);




		// --------------
    esat::DrawEnd();
    //Control fps 
    do{
    	current_time = esat::Time();
    }while((current_time-last_time)<=1000.0/fps);
    esat::WindowFrame();
}
  
  
esat::WindowDestroy();
return 0;
    
}
