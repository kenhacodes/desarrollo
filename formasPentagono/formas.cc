#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <math.h>

#include <stdio.h>

struct Tforma
{
	const unsigned int knPoints;
	esat::Vec2 position;
	float radio;
	bool isStar = false;
	esat::Vec2 g_forma[25];
};

const unsigned int knPointsSquare = 4;
const unsigned int knPointsTriangle = 3;
const unsigned int knPointsStar = 10;
const unsigned int knPointsPenthagon = 5;

esat::Vec2 g_square[knPointsSquare];
esat::Vec2 g_triangle[knPointsTriangle];
esat::Vec2 g_star[knPointsStar];
esat::Vec2 g_penthagon[knPointsPenthagon];

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;

void show(unsigned int knPoints, esat::Vec2 g_form[25], float radio,
					esat::Vec2 position, bool star){

	float angle = 6.28f / (float) knPoints;
	float offset = 0;
	if (knPoints%2!=0)
	{
		offset = -3.14f/2;
	}else{
		offset = +3.14f/knPoints;
	}
	
	for (int i = 0; i < knPoints; ++i) {
		if (star && i%2!=0)
		{
			g_form[i].x = (float) cos(angle+offset+angle * i) * (radio/2) + position.x;
  		g_form[i].y = (float) sin(angle+offset+angle * i) * (radio/2) + position.y;
		}else{
			g_form[i].x = (float) cos(offset+angle+angle * i) * radio + position.x;
  		g_form[i].y = (float) sin(offset+angle+angle * i) * radio + position.y;
		}
	}
}

int esat::main(int argc, char **argv) {

	float angle = 6.28f / (float) knPointsPenthagon;
	float radio = 150.0f;
	


	
	//PENTHAGON STAR
	for (int i = 0; i < knPointsPenthagon; ++i) {
  	g_penthagon[i].x = (float) cos(angle-3.14f/2+angle*2 * i) * radio + 400.0f + cos(angle);
  	g_penthagon[i].y = (float) sin(angle-3.14f/2+angle*2 * i) * radio + 300.0f + sin(angle);
	}
	
	
	show(knPointsPenthagon,g_penthagon,150.0f,{100.0f,300.0f},false);
	show(knPointsSquare,g_square,100.0f,{500.0f,200.0f},false);
	show(knPointsTriangle,g_triangle,100.0f,{300.0f,400.0f},false);
	show(knPointsStar,g_star,100.0f,{700.0f,400.0f},true);

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

			esat::DrawSetStrokeColor(255,0,0,255);
			esat::DrawSetFillColor(255, 255, 255, 255);
    	esat::DrawSolidPath(&g_penthagon[0].x, knPointsPenthagon);
			esat::DrawSolidPath(&g_square[0].x, knPointsSquare);
			esat::DrawSolidPath(&g_triangle[0].x, knPointsTriangle);
			esat::DrawSolidPath(&g_star[0].x, knPointsStar);
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
