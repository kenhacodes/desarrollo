#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <math.h>



#include <stdio.h>

const unsigned int knPoints = 50;
esat::Vec2 g_circle[knPoints];

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;



int esat::main(int argc, char **argv) {


	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
	float a = 0.0f;
	float b = 6.28f / (float) knPoints;

	float sin_a = sinf(a);
	float cos_a = cosf(a);
	float sin_b = sinf(b);
	float cos_b = cosf(b);

	float altura = 150.0f;
	float anchura = 150.0f;

    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

			for (int i = 0; i < knPoints; ++i)
			{
				g_circle[i] = {	cos_a  *cos_b - sin_a * sin_b,
												sin_a * cos_b + cos_a * sin_b};

				cos_a = g_circle[i].x;
				sin_a = g_circle[i].y;

				g_circle[i].x = g_circle[i].x * anchura + 400.0f;
				g_circle[i].y = g_circle[i].y * altura + 300.0f;
			}	

			esat::DrawSetStrokeColor(255,0,0,255);
			esat::DrawSetFillColor(0,0,0,0);
			esat::DrawSolidPath(&g_circle[0].x,knPoints);


			
			altura-=0.5f;
			anchura+=0.5f;
			


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
