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

esat::Mat3 TransformBase(){
	esat::Vec2 mouse = { (float) esat::MousePositionX(),
												(float) esat::MousePositionY()};
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Scale(	20.0f, 20.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(esat::Time() * 0.001f), m);
	m = esat::Mat3Multiply(esat::Mat3Translate(mouse.x, mouse.y), m);
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

esat::Mat3 TransformDerived(esat::Mat3 base, float angle){

	esat::Mat3 m = esat::Mat3Identity();

	m = esat::Mat3Multiply(esat::Mat3Scale(2.0f, 8.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(0), m); 
	m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
	m = esat::Mat3Multiply(base,m);
	return m;
}

esat::Mat3 TransformDerivedParticle(esat::Mat3 base, float angle, float speed){

	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
	m = esat::Mat3Multiply(esat::Mat3Scale(0.5f, 0.125f), m);
	
	m = esat::Mat3Multiply(esat::Mat3Translate(cosf(esat::Time() * 0.001 * speed),
																						 sinf(esat::Time() * 0.001 * speed)), m);
	m = esat::Mat3Multiply(base,m);
	return m;
}

/*
void FractalTransform(esat::Mat3 base, int fractal_iteration){
	fractal_iteration--;
	if (fractal_iteration >= 1) {
		for (int i = 0; i < knCircle; ++i)
		{
			esat::Mat3 derived = TransformDerived(base,  6.28f / (float) knCircle * (float) i);
			DrawFigure(derived);
			FractalTransform(derived, fractal_iteration);
		}
	}
}
*/


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

	initCircle();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

		esat::Mat3 base = TransformBase();
		DrawFigure(base);

		for (int i = 0; i < 3; i++)
		{
			esat::Mat3 derived = TransformDerived(base, 6.28f / (float) 3 * (float) i);
			DrawFigure(derived);
			esat::Mat3 particle = TransformDerivedParticle(derived, 6.28f / (float) 3 * (float) i, (float)i+1);
			DrawFigure(particle);
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
