#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <math.h>

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

const int kNPentagon = 5;
esat::Vec3 g_pentagon[kNPentagon];

void initPentagon(){
	float angle = 6.28f / (float) kNPentagon;
	for (int i = 0; i < kNPentagon; i++)
	{
		g_pentagon[i] = {
			cosf(angle * (float) i),
			sinf(angle * (float) i),
			1.0f
		};
	}
}

esat::Mat3 TransformBase(){
	esat::Vec2 mouse = { (float) esat::MousePositionX(),
												(float) esat::MousePositionY()};
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Scale(	50.0f * cosf(esat::Time() * 0.001f), 
																					50.0f * cosf(esat::Time() * 0.001f)), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(esat::Time() * 0.001f), m);
	m = esat::Mat3Multiply(esat::Mat3Translate(mouse.x, mouse.y), m);
	return m;
}

void DrawFigure(esat::Mat3 m){
	esat::Vec2 dr_points[kNPentagon];
	for (int i = 0; i < kNPentagon; i++)
	{
		esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_pentagon[i]);
		dr_points[i] = {tmp.x, tmp.y};
	}
	esat::DrawSetStrokeColor(0,0,0,0);
	esat::DrawSetFillColor(200,85,50,250);
	esat::DrawSolidPath(&dr_points[0].x, kNPentagon);
}
esat::Mat3 TransformDerived(esat::Mat3 base, float angle){

	esat::Mat3 m = esat::Mat3Identity();

	m = esat::Mat3Multiply(esat::Mat3Scale(0.5f, 0.5f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(3.14f), m); 
	m = esat::Mat3Multiply(esat::Mat3Translate(1.5f, 0.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m); 
	m = esat::Mat3Multiply(base,m);
	return m;
}
void FractalTransform(esat::Mat3 base, int fractal_iteration){
	fractal_iteration--;
	if (fractal_iteration >= 1) {
		for (int i = 0; i < kNPentagon; ++i)
		{
			esat::Mat3 derived = TransformDerived(base,  6.28f / (float) kNPentagon * (float) i);
			DrawFigure(derived);
			FractalTransform(derived, fractal_iteration);
		}
		
	}
}


// Traslacion, Rotacion, Scalado
/*
esat::Mat3 InitTransform(float scale, float angle, esat::Vec2 pos){
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Scale(scale, scale), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
	m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
	return m;
}
*/




int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);

	initPentagon();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

		esat::Mat3 base = TransformBase();
		DrawFigure(base);
		FractalTransform(base, 6);
		for (int i = 0; i < kNPentagon; i++)
		{
			esat::Mat3 derived = TransformDerived(base, 6.28f / (float) kNPentagon * (float) i);
			DrawFigure(derived);
		}
		

		
		

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
