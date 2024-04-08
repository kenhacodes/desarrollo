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

struct Tsquare
{
	zoro::Vec2D pos = {0.0f,0.0f};
	zoro::Vec2D dir = {0.5f,0.5f};
	float speed = 5.0f;
	float size = 40.0f;
};

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

Tsquare sqr;
zoro::Square2D g_sqr;
zoro::Vec2D mouse;
float angle, h;
float threshold = 5.0f;

void moveSqr(){
	mouse.x = esat::MousePositionX();
	mouse.y = esat::MousePositionY();
	//Check distance
	//h = sqrt(pow(sqr.pos.x-esat::MousePositionX(),2) + pow(sqr.pos.y-esat::MousePositionY(),2));
	h = zoro::MagnitudeVec2D( zoro::SubtractVec2D(sqr.pos, mouse) );
	if (h>threshold)
	{
		// Create normalized vec2 of dir

		//angle = atan2(sqr.pos.y-esat::MousePositionY(), sqr.pos.x- esat::MousePositionX());

		// Move it
		//angle = atan2(sqr.dir.y, sqr.dir.x);
		//sqr.pos.x -= cos(angle) *	sqr.speed;
		//sqr.pos.y -= sin(angle) * sqr.speed;

		sqr.pos = zoro::SubtractVec2D( sqr.pos, zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(sqr.pos, mouse)), sqr.speed));

	}

}

void paintSqr(){
	
	float normalAngle = 6.28f / 4;

	float offset = +3.14f/4;

	for (int i = 0; i < 4; i++)
	{
		g_sqr.p[i].x = cos(offset + normalAngle * i) * sqr.size + sqr.pos.x;
  	g_sqr.p[i].y = sin(offset + normalAngle * i) * sqr.size + sqr.pos.y;
	}

	esat::DrawSetStrokeColor(180,10,10,255);
	esat::DrawSetFillColor(120, 10, 10, 250);
	esat::DrawSolidPath(&g_sqr.p[0].x, 4);
}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
	sqr.pos = {400.0f,400.0f};
	
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

  	last_time = esat::Time(); 
  	esat::DrawBegin();
  	esat::DrawClear(0,0,0);

		if (esat::MouseButtonPressed(0)){
			moveSqr();
		}
			
		paintSqr();


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
