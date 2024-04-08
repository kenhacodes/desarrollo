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
	zoro::Vec2D pos = {400.0f, 400.0f};
	zoro::Vec2D speed = {0.0f,0.0f};
  zoro::Vec2D g_points[3];
  float radius = 50.0f;
  float angle = 0.0f;
  zoro::Vec2D acceleration = {0.0f,0.0f};
	rgb color = {255, 5, 5};
};

struct TPlanet{
  zoro::Vec2D pos;
  float gravity;
  float radius;
};

TShip ship;
zoro::Vec2D g_points[50];
zoro::Vec2D gravity;

TPlanet p1;
TPlanet p2;
TPlanet p3;
TPlanet p4;


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
  if (ship.pos.x > 800) ship.speed.x *= -1;
  if (ship.pos.x < 0) ship.speed.x *= -1;
  if (ship.pos.y > 800) ship.speed.y *= -1;
  if (ship.pos.y < 0) ship.speed.y *= -1;
  
  gravity = {0,0};

  if(zoro::MagnitudeVec2D(zoro::SubtractVec2D(p1.pos, ship.pos)) < p1.radius*20){
    gravity = zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(p1.pos, ship.pos)), p1.gravity); 
  }
  if(zoro::MagnitudeVec2D(zoro::SubtractVec2D(p2.pos, ship.pos)) < p2.radius*20){
      gravity = zoro::SumVec2D(gravity,zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(p2.pos, ship.pos)), p2.gravity));  
  }
  if (zoro::MagnitudeVec2D(zoro::SubtractVec2D(p3.pos, ship.pos)) < p3.radius*20)
  {
      gravity = zoro::SumVec2D(gravity,zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(p3.pos, ship.pos)), p3.gravity));

  }
  if (zoro::MagnitudeVec2D(zoro::SubtractVec2D(p4.pos, ship.pos)) < p4.radius*20)
  {
      gravity = zoro::SumVec2D(gravity,zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(p4.pos, ship.pos)), p4.gravity));
  }
  
  ship.speed = zoro::SumVec2D(zoro::ScaleVec2D(ship.speed,0.999), ship.acceleration);
  ship.speed = zoro::SumVec2D(ship.speed, gravity);
  ship.pos = zoro::SumVec2D(ship.pos, ship.speed);
}

void paintPlanet(TPlanet planet){


	float normalAngle = 6.28f / 25;
	for (int i = 0; i < 25; i++)
	{
		g_points[i].x = cos(normalAngle * i) * planet.radius + planet.pos.x;
  	g_points[i].y = sin(normalAngle * i) * planet.radius + planet.pos.y;
	}

	esat::DrawSetStrokeColor(255,255,255,255);
	esat::DrawSetFillColor(255,255,255,100);
	esat::DrawSolidPath(&g_points[0].x, 25);

}

void init(){
  p1.gravity = 0.1;
  p2.gravity = 0.05;
  p3.gravity = 0.06;
  p4.gravity = 0.03;

  p1.pos = {500, 400};
  p2.pos = {100, 700};
  p3.pos = {300, 300};
  p4.pos = {700, 200};

  p1.radius = 16;
  p2.radius = 12;
  p3.radius = 14;
  p4.radius = 5;
}

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);
  
  init();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

  	last_time = esat::Time(); 
  	esat::DrawBegin();
  	esat::DrawClear(0,0,0);
    
    input();
    movementShip();
    paintShip(ship);
    paintPlanet(p1);
    paintPlanet(p2);
    paintPlanet(p3);
    paintPlanet(p4);

  
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
