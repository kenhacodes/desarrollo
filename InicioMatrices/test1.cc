#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <math.h>

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

const int knSquare = 4;
const int knSixagon = 4;

esat::Vec3 g_square[knSquare];
esat::Vec3 g_Sixagon[knSixagon];
esat::Vec3 g_diamond[knSquare];


void initFigures(){

	float angle = 6.28f / (float) knSquare;

	for (int i = 0; i < knSquare; i++)
	{
		g_square[i] = {
				cosf(angle * (float) i + 3.14/2),
				sinf(angle * (float) i + 3.14/2),
				1.0f
			};
	}

	angle = 6.28f / (float) knSixagon;
	for (int i = 0; i < knSixagon; i++)
	{
		g_Sixagon[i] = {
				cosf(angle * (float) i),
				sinf(angle * (float) i),
				1.0f
			};
	}

	g_diamond[0] = {1.0f, 0.0f, 1.0f};
	g_diamond[1] = {-0.33f, 0.4f, 1.0f};
	g_diamond[2] = {-0.6f, 0.0f, 1.0f};
	g_diamond[3] = {-0.33f, -0.4f, 1.0f};

}	

esat::Mat3 TransformBase(){
	esat::Vec2 mouse = { (float) esat::MousePositionX(),
												(float) esat::MousePositionY()};
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Scale(	50.0f, 50.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(0), m);
	m = esat::Mat3Multiply(esat::Mat3Translate(mouse.x, mouse.y), m);
	return m;
}

void DrawShip(esat::Mat3 m){
	esat::Vec2 dr_points[knSquare];
	for (int i = 0; i < knSquare; i++)
	{
		esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_diamond[i]);
		dr_points[i] = {tmp.x, tmp.y};
	}
	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(200,85,50,25);
	esat::DrawSolidPath(&dr_points[0].x, knSquare);
}

void DrawSixagon(esat::Mat3 m){
	esat::Vec2 dr_points[knSixagon];
	for (int i = 0; i < knSixagon; i++)
	{
		esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_Sixagon[i]);
		dr_points[i] = {tmp.x, tmp.y};
	}
	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(200,85,50,25);
	esat::DrawSolidPath(&dr_points[0].x, knSixagon);
}
void DrawSquare(esat::Mat3 m){
	esat::Vec2 dr_points[knSquare];
	for (int i = 0; i < knSquare; i++)
	{
		esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_square[i]);
		dr_points[i] = {tmp.x, tmp.y};
	}
	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(200,85,50,25);
	esat::DrawSolidPath(&dr_points[0].x, knSquare);
}







int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);

	initFigures();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

		esat::Mat3 base = TransformBase();
		DrawShip(base);
		esat::Mat3 base1 = TransformBase();
		DrawSixagon(base1);
		esat::Mat3 base2 = TransformBase();
		DrawSquare(base2);
		
		

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
