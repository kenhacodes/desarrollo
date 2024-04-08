#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

float kPI = 3.1415f;
float h, angle;
esat::Vec2 center = {400,400};
esat::Vec2 pos;
esat::Vec2 endLine;
esat::Vec2 triangle[3];

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

void paintArrow(){
	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(0, 0, 0, 0);
	
	h = sqrt(pow(pos.x-center.x,2) + pow(pos.y-center.x,2)); // Size of vector
	
	angle = atan2(pos.y-center.y,pos.x-center.y);
	
	endLine.x = pos.x - ( (h*0.2f) * cos(angle) );
	endLine.y = pos.y - ( (h*0.2f) * sin(angle) );

	esat::DrawLine(center.x,center.y, endLine.x, endLine.y);

	triangle[0] = pos;

	triangle[1].x = endLine.x + ( (h*0.2f) * cos(angle + (kPI/2)) );
	triangle[1].y = endLine.y + ( (h*0.2f) * sin(angle + (kPI/2)) );

	triangle[2].x = endLine.x + ( (h*0.2f) * cos(angle - (kPI/2)) );
	triangle[2].y = endLine.y + ( (h*0.2f) * sin(angle - (kPI/2)) );

	esat::DrawSolidPath(&triangle[0].x, 3);
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
