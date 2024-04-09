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

struct TShip{
	zoro::Vec2 pos = {400.0f, 400.0f};
	zoro::Vec2 speed = {0.0f,0.0f};
  zoro::Vec2 g_points[3];
  float radius = 50.0f;
  float angle = 0.0f;
  zoro::Vec2 acceleration = {0.0f,0.0f};
	rgb color = {255, 5, 5};
};

TShip ship;

void paintShip(TShip ship){

	ship.g_points[0].x = cos(0 + ship.angle) * ship.radius + ship.pos.x;
  ship.g_points[0].y = sin(0 + ship.angle) * ship.radius + ship.pos.y;
	
  ship.g_points[1].x = cos(2.7 + ship.angle) * ship.radius + ship.pos.x;
  ship.g_points[1].y = sin(2.7 + ship.angle) * ship.radius + ship.pos.y;

  ship.g_points[2].x = cos(-2.7 + ship.angle) * ship.radius + ship.pos.x;
  ship.g_points[2].y = sin(-2.7 + ship.angle) * ship.radius + ship.pos.y;
  

	esat::DrawSetStrokeColor(ship.color.r,ship.color.g,ship.color.b,255);
	esat::DrawSetFillColor(ship.color.r,ship.color.g,ship.color.b,50);
	esat::DrawSolidPath(&ship.g_points[0].x, 3);
}

void input(){
  
  ship.acceleration = {0.0f,0.0f};
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
  {
    ship.acceleration.x = cos(ship.angle) * 0.15; 
    ship.acceleration.y = sin(ship.angle) * 0.15; 
  }
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left))
  {
    ship.angle -= 0.05;
  }
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right))
  {
    ship.angle += 0.05;
  }
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
  {
    ship.acceleration.x = -cos(ship.angle) * 0.05; 
    ship.acceleration.y = -sin(ship.angle) * 0.05; 
  }
}

void movementShip(){
  if (ship.pos.x > 800) ship.pos.x = 1;
  if (ship.pos.x < 0) ship.pos.x = 799;
  if (ship.pos.y > 800) ship.pos.y = 1;
  if (ship.pos.y < 0) ship.pos.y = 799;

  ship.speed = zoro::SumVec2(zoro::ScaleVec2(ship.speed,0.988), ship.acceleration);
  ship.pos = zoro::SumVec2(ship.pos, ship.speed);
}

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  


  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

  	last_time = esat::Time(); 
  	esat::DrawBegin();
  	esat::DrawClear(0,0,0);
    
    input();
    movementShip();
    paintShip(ship);


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
