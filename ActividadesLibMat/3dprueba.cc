#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include "math.h"

float kwidth = 800.0f;
float kheight = 800.0f;

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

esat::Mat4 basic(){
  esat::Vec2 mouse = {(float) esat::MousePositionX(),
                      (float) esat::MousePositionY() };

  esat::Mat4 m = esat::Mat4Identity();
  //m = esat::Mat4Multiply(esat::Mat4Translate(0.0f, 0.0f, 1.5f), m);
  //m = esat::Mat4Multiply(esat::Mat4Translate(cos(esat::Time() * 0.001f) * 2, sin(esat::Time() * 0.001f) * 2, 0.0f),m);
  
  //m = esat::Mat4Multiply(esat::Mat4Translate(1.0f, 15.6f, 1.0f), m);
  
  //m = esat::Mat4Multiply(esat::Mat4RotateX(sin(esat::Time() * 0.001f) * 1),m);
  m = esat::Mat4Multiply(esat::Mat4Translate(1.0f, 1.0f, 1.0f), m);
  m = esat::Mat4Multiply(esat::Mat4Projection(), m);
  m = esat::Mat4Multiply(esat::Mat4Translate(mouse.x, mouse.y, 0.0f), m);
  m = esat::Mat4Multiply(esat::Mat4Scale(1.0f, 1.0f, 1.0f), m);
  return m;
}

esat::Mat4 TransformBase(esat::Mat4 basic){
  esat::Vec2 mouse = {(float) esat::MousePositionX(),
                      (float) esat::MousePositionY() };

  
  // . . .
  //m = esat::Mat4Multiply(esat::Mat4Scale(3.0f, 6.0f,3.0f), m);
  
  // Point = T(mx, my, 0.0f) * S(sx, sy, 1.0f) * Proj * T(0,0,z) * Point

  esat::Mat4 m = esat::Mat4Identity();
  // ---
  //m = esat::Mat4Multiply(esat::Mat4RotateY(esat::Time() * 0.001f),m);
  //m = esat::Mat4Multiply(esat::Mat4RotateY(esat::Time() * 0.002f),m);
  //m = esat::Mat4Multiply(esat::Mat4RotateZ(esat::Time() * 0.003f),m);

  m = esat::Mat4Multiply(esat::Mat4Translate(0.0f, 0.0f, 1.0f), m);
  //m = esat::Mat4Multiply(esat::Mat4Projection(), m);
  m = esat::Mat4Multiply(esat::Mat4Scale(150.0f, 150.0f, 1.0f), m);
  
  m = esat::Mat4Multiply(basic, m);
  return m;
  // . . .

}

esat::Mat4 TransformInnerInnerCube(esat::Mat4 base){
  esat::Mat4 m = esat::Mat4Identity();
  m = esat::Mat4Multiply(esat::Mat4Scale(0.25f, 0.25f, 0.25f), m);
  m = esat::Mat4Multiply(esat::Mat4Translate(cos(esat::Time() * 0.001f) * 0.75f, 0.0f, 0.0f), m);
  m = esat::Mat4Multiply(base, m);
  return m;
}

esat::Mat4 TransformInnerCube(esat::Mat4 base) {
  esat::Mat4 m = esat::Mat4Identity();
  //m = esat::Mat4Multiply(esat::Mat4Translate(cos(esat::Time() * 0.001f) * 20, sin(esat::Time() * 0.001f) * 20, 0.0f),m);
  m = esat::Mat4Multiply(esat::Mat4Scale(0.25f, 0.25f, 0.25f), m);
  m = esat::Mat4Multiply(esat::Mat4Translate(cos(esat::Time() * 0.001f) * 3.3f, sin(esat::Time() * 0.001f) * 3.3f, 0.0f),m);
  m = esat::Mat4Multiply(base, m);
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
    esat::Mat4 b = basic();
    esat::Mat4 base = TransformBase(b);
    esat::Mat4 inner = TransformInnerCube(base);
    esat::Mat4 innerininner = TransformInnerInnerCube(inner);
    DrawFigure(base);
    DrawFigure(inner);
    DrawFigure(innerininner);




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
