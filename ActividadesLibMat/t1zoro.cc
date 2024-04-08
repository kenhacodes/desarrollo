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

float h, angle;
zoro::Vec2D center = {400,400};
zoro::Vec2D pos; // Mouse
zoro::Vec2D endLine; // End Line before arrow
zoro::Triangle2D triangle;

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

void paintArrow(){
	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(0, 0, 0, 0);
	
	//h = sqrt(pow(pos.x-center.x,2) + pow(pos.y-center.x,2)); // Size of vector
	h = zoro::MagnitudeVec2D(zoro::SubtractVec2D(pos, center));
	//angle = atan2(pos.y-center.y,pos.x-center.y);
	
	//endLine.x = pos.x - ( (h*0.2f) * cos(angle) );
	//endLine.y = pos.y - ( (h*0.2f) * sin(angle) );

	endLine = zoro::SumVec2D(zoro::ScaleVec2D(zoro::SubtractVec2D(pos, center), 0.8), center);
	esat::DrawLine(center.x,center.y, endLine.x, endLine.y);

	triangle.p[0] = pos;
	/*
	triangle.p[1].x = endLine.x + ( (h*0.2f) * cos(angle + (zoro::PI/2)) );
	triangle.p[1].y = endLine.y + ( (h*0.2f) * sin(angle + (zoro::PI/2)) );

	triangle.p[2].x = endLine.x + ( (h*0.2f) * cos(angle - (zoro::PI/2)) );
	triangle.p[2].y = endLine.y + ( (h*0.2f) * sin(angle - (zoro::PI/2)) );
	*/
		
	triangle.p[1] = zoro::SumVec2D(endLine, zoro::ScaleVec2D( zoro::NormalizeVec2D( zoro::LeftPerpendicularVec2D(zoro::SubtractVec2D(pos, center)) ), h * 0.2));
	triangle.p[2] = zoro::SumVec2D(endLine, zoro::ScaleVec2D( zoro::NormalizeVec2D( zoro::RightPerpendicularVec2D(zoro::SubtractVec2D(pos, center)) ), h * 0.2));
	
	zoro::PrintVec2D(triangle.p[1] );

	esat::DrawSolidPath(&triangle.p[0].x, 3);
}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

			pos.x = esat::MousePositionX();
			pos.y = esat::MousePositionY();

			paintArrow();

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
