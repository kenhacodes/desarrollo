#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <math.h>

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;


const int knCircle = 33;
esat::Vec3 g_star[knCircle];



void initCircle(){

	float angle = 6.28f / (float) knCircle;

	for (int i = 0; i < knCircle; i++)
	{
		g_star[i] = {
				cosf(angle * (float) i),
				sinf(angle * (float) i),
				1.0f
			};
	}
}

// Traslacion, Rotacion, Scalado
esat::Mat3 InitTransform(float scale, float angle, esat::Vec2 pos){
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
	m = esat::Mat3Multiply(esat::Mat3Scale(scale, scale), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
	
	return m;
}

void DrawFigure(esat::Mat3 m){
	esat::Vec2 dr_points[knCircle];
	for (int i = 0; i < knCircle; i++)
	{
		esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_star[i]);
		dr_points[i] = {tmp.x, tmp.y};
	}
	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(200,85,50,25);
	esat::DrawSolidPath(&dr_points[0].x, knCircle);
}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
	initCircle();
	
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

		esat::Mat3 base = InitTransform(150.0f, 0.0f, {400.0f,400.0f});
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
