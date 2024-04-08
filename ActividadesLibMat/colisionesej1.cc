#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <math.h>
#include "zorolib.h"

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

const int kNCircle = 5;
esat::Vec3 g_circle[kNCircle];
bool isColiding = false;



void initStar(){

	float angle = 6.28f / (float) kNCircle;

	for (int i = 0; i < kNCircle; i++)
	{
		
			g_circle[i] = {
				cosf(angle * (float) i),
				sinf(angle * (float) i),
				1.0f
			};
		
	}
}

void checkCol(esat::Vec2 points[kNCircle]){
  
  isColiding = false;
  zoro::Vec2 mouse = {(float)esat::MousePositionX(), (float)esat::MousePositionY()};
  zoro::Vec2 a,b;
  for (int i = 0; i < kNCircle; i++)
  {
    a = {points[i].x, points[i].y};
    if (i<kNCircle-1) b = {points[i+1].x, points[i+1].y};
    else b = {points[0].x, points[0].y};

    printf("\n%d - %f",i,zoro::DotVec2(zoro::NormalizeVec2(zoro::LeftPerpendicularVec2(zoro::SubtractVec2(a,b))), zoro::NormalizeVec2(mouse)));
    
    if ( zoro::DotVec2(zoro::NormalizeVec2(zoro::RightPerpendicularVec2(zoro::SubtractVec2(a,b))), zoro::NormalizeVec2(mouse))>0)
    {
      isColiding = true;
    }

  }
}

esat::Mat3 TransformBase(){
	
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Scale(	150.0f, 150.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Translate(00.0f, 00.0f), m);
	return m;
}

void DrawFigure(esat::Mat3 m){
	esat::Vec2 dr_points[kNCircle];
	for (int i = 0; i < kNCircle; i++)
	{
		esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_circle[i]);
		dr_points[i] = {tmp.x, tmp.y};
	}

  checkCol(dr_points);

	esat::DrawSetStrokeColor(0,0,0,0);
	esat::DrawSetFillColor(200,85,50,250);
  if (isColiding) esat::DrawSetFillColor(50,85,200,250);
	esat::DrawSolidPath(&dr_points[0].x, kNCircle);
}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
	initStar();
	

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

		esat::Mat3 base = TransformBase();
		

    
    DrawFigure(base);
		


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
