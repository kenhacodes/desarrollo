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

zoro::Circle2D inner;
zoro::Circle2D outer;
zoro::Vec2D g_points[302];
//zoro::Vec2D g_pointsouter[200];
//float normalAngle = (6.28f / 100) + (zoro::PI/2);
float normalAngle = (6.28f / 100);
float offset = (-3.14/4);

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

int percentage = 0;
int counter = 0;

void paintCircle(zoro::Circle2D circle){
	for (int i = 0; i < 100; i++)
	{
		g_points[i].x = cos(normalAngle * i) * circle.radius + circle.c.x;
  	g_points[i].y = sin(normalAngle * i) * circle.radius + circle.c.y;
	}

	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(0,0,0,0);
	esat::DrawSolidPath(&g_points[0].x, 100);
}

void paintDaThing(){

  //paintCircle(inner);
  //paintCircle(outer);
  
  int red = 240 + (-240) * (percentage*0.01);
  int green = 1 + (250- 1) * (percentage*0.01);

  if (percentage<100){
    esat::DrawSetStrokeColor(red,green,0,255);
	  esat::DrawSetFillColor(red,green,0,255);
  }else{
    esat::DrawSetStrokeColor(0,250,20,255);
	  esat::DrawSetFillColor(0,250,20,255);
  }
  
  // Inner area
  for (int i = 0; i < percentage; i++)
  {
    g_points[i].x = cos(offset+normalAngle * (i)) * inner.radius + inner.c.x;
  	g_points[i].y = sin(offset+normalAngle * (i)) * inner.radius + inner.c.y;
  }

  // Middle area
  zoro::Vec2D inicio = g_points[percentage-1];
  zoro::Vec2D final;

  final.x = cos(offset+normalAngle * (percentage)) * outer.radius + outer.c.x;
  final.y = sin(offset+normalAngle * (percentage)) * outer.radius + outer.c.y;
  
  zoro::Vec2D linea = zoro::SubtractVec2D(inicio, final);
  float m = zoro::MagnitudeVec2D(linea);
  zoro::NrmlizeV2(&linea);
  float step = m/100;
  float angle = atan2(inicio.y-final.y, inicio.x-final.x);
  for (int i = 0; i < 100; i++)
  {
    g_points[percentage + i].x = inicio.x + (-linea.x*step*i) ;
    if (percentage<101) g_points[percentage + i].y = inicio.y + (-linea.y*step*i) +(8*cos((esat::Time()/100 + i/5 * 3.14/2) ));
    else g_points[percentage + i].y = inicio.y + (-linea.y*step*i);
  }
  //printf("\nAngle:%f", angle);


  // Outer area
  for (int i = percentage; i > 0; i--)
  {
    g_points[percentage + 100 + percentage-i].x = cos(offset+normalAngle * (i)) * outer.radius + outer.c.x;
    g_points[percentage + 100 + percentage-i].y = sin(offset+normalAngle * (i)) * outer.radius + outer.c.y;
  }
  
	esat::DrawSolidPath(&g_points[0].x, ((percentage*2)+100));
  }
  
  
  


int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
  inner.radius = 100.0f;
  outer.radius = 300.0f;

  inner.c = {400.0f,400.0f};
  outer.c = {400.0f,400.0f};

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

  	last_time = esat::Time(); 
  	esat::DrawBegin();
  	esat::DrawClear(0,0,0);
      
    //inner.c = {(float)esat::MousePositionX(),(float)esat::MousePositionY()};
    //outer.c = {(float)esat::MousePositionX(),(float)esat::MousePositionY()};
      
    if (esat::MouseButtonPressed(0) && percentage<101) percentage++;
    if (esat::MouseButtonPressed(1) && percentage>0) percentage--;
    percentage++;
    if (percentage==101) percentage=1;

    
    if (percentage>0) paintDaThing();
  
    
    

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
