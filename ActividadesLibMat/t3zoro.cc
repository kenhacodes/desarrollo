#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "zorolib.h"


struct rgb
{
	float r,g,b;
};

struct Tcircle{
	zoro::Vec2D pos = {400.0f, 400.0f};
	float radius = 100.0f;
	rgb color = {180, 5, 5};
};

Tcircle c1, c2;
zoro::Vec2D g_points[50];

float h;

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

void paintCircle(Tcircle circle){

	float normalAngle = 6.28f / 50;
	for (int i = 0; i < 50; i++)
	{
		g_points[i].x = cos(normalAngle * i) * circle.radius + circle.pos.x;
  	g_points[i].y = sin(normalAngle * i) * circle.radius + circle.pos.y;
	}

	esat::DrawSetStrokeColor(circle.color.r,circle.color.g,circle.color.b,0);
	esat::DrawSetFillColor(circle.color.r,circle.color.g,circle.color.b,100);
	esat::DrawSolidPath(&g_points[0].x, 50);
}

void checkColision(){

	//h = sqrt(pow((c1.pos.x-c2.pos.x),2) + pow(c1.pos.y-c2.pos.y,2));
	h = zoro::MagnitudeVec2D( zoro::SubtractVec2D(c1.pos, c2.pos) );
	if (h < (c1.radius + c2.radius))
	{
		c1.color = {200,200,50};
	}else{
		c1.color = c2.color;
	}
	
	
}

void updateCircles(){
	c2.pos.x = esat::MousePositionX();
	c2.pos.y = esat::MousePositionY();

	checkColision();

	paintCircle(c1);
	paintCircle(c2);
}

int esat::main(int argc, char **argv) {


	c2.radius = 33.0f;

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

			updateCircles();


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
