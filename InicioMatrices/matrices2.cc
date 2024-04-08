#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <math.h>

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

esat::Vec2 points[7] = {
	{0.0f, 0.25f},
	{-0.625f, -1.0f},
	{-1.0f, - 1.0f},
	{0.0f, 1.0f},
	{1.0f, -1.0f},
	{0.625f, -1.0f},
	{0.0f, 0.25f}
};

esat::Vec2 Fig[18][7];

float ScaleInside = 33;
float ScaleOutside = 40;
esat::Vec2 FigPos;
float anglePointsInner;
float anglePointsOuter;
esat::Mat3 T;
esat::Mat3 R;
esat::Mat3 S;
esat::Mat3 M;

// Traslacion, Rotacion, Scalado
esat::Mat3 InitTransform(float scale, float angle, esat::Vec2 pos){
	esat::Mat3 m = esat::Mat3Identity();
	m = esat::Mat3Multiply(esat::Mat3Scale(scale, scale), m);
	m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
	m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
	return m;
}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);

	anglePointsInner = 6.28f / 6; 
	anglePointsOuter = 6.28f / 12; 
	

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
		esat::DrawSetStrokeColor(0,0,0,0);
		esat::DrawSetFillColor(200,85,50,250);

		FigPos = {(float)esat::MousePositionX(),(float)esat::MousePositionY()};

		for (int i = 0; i < 18; i++)
		{
			for (int j = 0; j < 7; j++)
			{
				Fig[i][j] = points[j];
			}
		}

		
		// INNER
		for (int i = 0; i < 6; i++)
		{
			M = esat::Mat3Identity();
			M = esat::Mat3Multiply(esat::Mat3Translate(0.0f, -2.0f), M);
			M = esat::Mat3Multiply(esat::Mat3Scale(ScaleInside, ScaleInside), M);
			M = esat::Mat3Multiply(esat::Mat3Rotate((anglePointsInner*i)+3.14/2), M);
			M = esat::Mat3Multiply(esat::Mat3Translate(FigPos.x, FigPos.y), M);
			
			for (int j = 0; j < 7; j++)
			{
				Fig[i][j] = esat::Mat3TransformVec2(M, Fig[i][j]);
			}
		}
		
		/*
		// INNER
		for (int i = 0; i < 6; i++)
		{
			M = InitTransform(ScaleInside, (anglePointsInner*i)+3.14/2, FigPos);
			M = esat::Mat3Multiply(esat::Mat3Translate(cos(anglePointsInner*i)*100, sin(anglePointsInner*i)*100), M);
			for (int j = 0; j < 7; j++)
			{
				
				Fig[i][j] = esat::Mat3TransformVec2(M, Fig[i][j]);
			}
		}
		*/
		/*
		for (int i = 6; i < 18; i++)
		{
			M = InitTransform(ScaleOutside, (anglePointsOuter * i), FigPos);
			M = esat::Mat3Multiply(esat::Mat3Translate( cos(anglePointsInner*i)*230, sin(anglePointsInner*i)*230), M);
			for (int j = 0; j < 7; j++)
			{
				
				
				Fig[i][j] = esat::Mat3TransformVec2(M, Fig[i][j]);
			}
		}
		*/

		for (int i = 6; i < 18; i++)
		{

			M = esat::Mat3Identity();
			M = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 3.9f), M);
			M = esat::Mat3Multiply(esat::Mat3Scale(ScaleOutside, ScaleOutside), M);
			M = esat::Mat3Multiply(esat::Mat3Rotate((anglePointsOuter*i)), M);
			M = esat::Mat3Multiply(esat::Mat3Translate(FigPos.x, FigPos.y), M);

			for (int j = 0; j < 7; j++)
			{
				Fig[i][j] = esat::Mat3TransformVec2(M, Fig[i][j]);
			}
		}

		for (int i = 0; i < 18; i++)
		{
			esat::DrawSolidPath(&Fig[i][0].x, 7);
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
