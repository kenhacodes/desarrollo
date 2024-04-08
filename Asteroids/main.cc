#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "asteroidsAPI.h"

enum WindowState {
  MENU = 0,
  LOGIN,
  INGAME
};

struct TShip{
	zoro::Vec2 pos = {400.0f, 400.0f};
	zoro::Vec2 speed = {0.0f,0.0f};
  zoro::Vec2 acceleration = {0.0f,0.0f};
  zoro::Vec3 *g_points;
  zoro::Vec2 *dr_points;
  zoro::Vec3 *g_Fuelpoints;
  zoro::Vec2 *dr_Fuelpoints;
  int kNPoints = 5;
  float scale = 50.0f;
  float angle = 0.0f;
	ast::rgb color = {255, 255, 255};
  bool IsPressing = false;
  bool isPaintingFuel = false;
};

struct TUser{
  char *name;     // 20
  char *email;    // 32
  char *lastname; // 20
  char *pass;     // 24
  char *prov;     // 16
  char *country;  // 20
  int credits;    // 4
};

TShip ship;

ast::TAsteroid *asteroids;
ast::TAsteroid asteroidTest;
ast::TAsteroid asteroidTest1;
ast::TAsteroid asteroidTest2;
ast::TAsteroid asteroidTest3;

ast::TAsteroidData *astDataTypes;

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

float sprFuelTimeRef = 100.0f;
double sprFuelTime = 0.0;

void init(){
  
  asteroids = nullptr;
  
  asteroidTest.pos = {400.0f, 400.0f};
  asteroidTest.size = ast::SMALL;
  asteroidTest.type = 0;
  asteroidTest.dir = {1.0f, 0.2f};
  asteroidTest.next = nullptr;

  asteroidTest1.pos = {100.0f, 400.0f};
  asteroidTest1.size = ast::SMALL;
  asteroidTest1.type = 0;
  asteroidTest1.dir = {-0.8f, 0.4f};
  asteroidTest1.next = nullptr;

  asteroidTest2.pos = {100.0f, 200.0f};
  asteroidTest2.size = ast::SMALL;
  asteroidTest2.type = 0;
  asteroidTest2.dir = {0.2f, -0.3f};
  asteroidTest2.next = nullptr;

  asteroidTest3.pos = {250.0f, 250.0f};
  asteroidTest3.size = ast::SMALL;
  asteroidTest3.type = 0;
  asteroidTest3.dir = {0.3f, 0.7f};
  asteroidTest3.next = nullptr;

  ast::Insert(&asteroids, asteroidTest);
  
  asteroidTest.pos = {200.0f, 400.0f};
  asteroidTest.type = 1;
  ast::Insert(&asteroids, asteroidTest);
  
  asteroidTest.pos = {400.0f, 200.0f};
  asteroidTest.type = 2;
  ast::Insert(&asteroids, asteroidTest);
  
}

void initAstData(){
  
  astDataTypes = (ast::TAsteroidData*) malloc(4 * sizeof(ast::TAsteroidData));

  (astDataTypes + 0)->kNPoints = 12;
  (astDataTypes + 1)->kNPoints = 11;
  (astDataTypes + 2)->kNPoints = 10;
  (astDataTypes + 3)->kNPoints = 12;

  (astDataTypes + 0)->dr_points = (zoro::Vec2*) malloc((astDataTypes + 0)->kNPoints * sizeof(zoro::Vec2));
  (astDataTypes + 0)->g_points = (zoro::Vec3*) malloc((astDataTypes + 0)->kNPoints * sizeof(zoro::Vec3));

  (astDataTypes + 1)->dr_points = (zoro::Vec2*) malloc((astDataTypes + 1)->kNPoints * sizeof(zoro::Vec2));
  (astDataTypes + 1)->g_points = (zoro::Vec3*) malloc((astDataTypes + 1)->kNPoints * sizeof(zoro::Vec3));

  (astDataTypes + 2)->dr_points = (zoro::Vec2*) malloc((astDataTypes + 2)->kNPoints * sizeof(zoro::Vec2));
  (astDataTypes + 2)->g_points = (zoro::Vec3*) malloc((astDataTypes + 2)->kNPoints * sizeof(zoro::Vec3));

  (astDataTypes + 3)->dr_points = (zoro::Vec2*) malloc((astDataTypes + 3)->kNPoints * sizeof(zoro::Vec2));
  (astDataTypes + 3)->g_points = (zoro::Vec3*) malloc((astDataTypes + 3)->kNPoints * sizeof(zoro::Vec3));

  // Asteroid 0

  (*((*(astDataTypes + 0)).g_points + 0)) = {0.4f, -0.22f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 1)) = {0.87f, -0.5f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 2)) = {0.44f, -0.95f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 3)) = {0.0f, -0.72f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 4)) = {-0.38f, -0.95f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 5)) = {-0.90f, -0.4f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 6)) = {-0.77f, 0.0f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 7)) = {-0.9f, 0.48f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 8)) = {-0.5f, 0.95f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 9)) = {-0.3f, 0.75f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 10)) = {0.31f, 0.97f, 1.0f};
  (*((*(astDataTypes + 0)).g_points + 11)) = {0.88f, 0.25f, 1.0f};
  
  // Asteroid 1
  
  (*((*(astDataTypes + 1)).g_points + 0)) = {-0.1f, 0.1f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 1)) = {-0.2f, 0.95f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 2)) = {0.17f, 0.97f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 3)) = {0.75f, 0.07f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 4)) = {0.75f, -0.25f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 5)) = {0.37f, -0.99f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 6)) = {-0.25f, -0.99f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 7)) = {-0.8f, -0.25f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 8)) = {-0.43f, 0.05f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 9)) = {-0.8f, 0.18f, 1.0f};
  (*((*(astDataTypes + 1)).g_points + 10)) = {-0.5f, 0.93f, 1.0f};
  
  // Asteroid 2

  (*((*(astDataTypes + 2)).g_points + 0)) = {0.55f, -0.1f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 1)) = {0.78f, -0.6f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 2)) = {0.41f, -0.94f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 3)) = {-0.02f, -0.56f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 4)) = {-0.43f, -0.9f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 5)) = {-0.77f, -0.61f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 6)) = {-0.77f, 0.52f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 7)) = {-0.48f, 0.89f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 8)) = {0.2f, 0.89f, 1.0f};
  (*((*(astDataTypes + 2)).g_points + 9)) = {0.74f, 0.45f, 1.0f};

  
  // Asteroid 3
  (*((*(astDataTypes + 3)).g_points + 0)) = {0.14f, 0.0f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 1)) = {0.78f, -0.27f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 2)) = {0.78f, -0.5f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 3)) = {0.24f, -0.9f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 4)) = {-0.31f, -0.9f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 5)) = {-0.12f, -0.5f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 6)) = {-0.70f, -0.5f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 7)) = {-0.70f, 0.16f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 8)) = {-0.40f, 0.85f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 9)) = {0.11f, 0.62f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 10)) = {0.42f, 0.84f, 1.0f};
  (*((*(astDataTypes + 3)).g_points + 11)) = {0.78f, 0.41f, 1.0f};

} 

void resetShip(TShip *ship){
  
  free(ship->g_points);
  free(ship->dr_points);
  free(ship->g_Fuelpoints);
  free(ship->dr_Fuelpoints);

  ship->pos = {400.0f, 400.0f};
  ship->speed = {0.0f,0.0f};
  ship->acceleration = {0.0f,0.0f};
  ship->scale = 30.0f;
  ship->angle = 0.0f;
  ship->dr_points = (zoro::Vec2*) malloc(ship->kNPoints * sizeof(zoro::Vec2));
  ship->g_points = (zoro::Vec3*) malloc(ship->kNPoints * sizeof(zoro::Vec3));
  *(ship->g_points + 0) = {+0.8f, +0.0f, 1.0f};
  *(ship->g_points + 1) = {-0.25f, +0.35f, 1.0f};
  *(ship->g_points + 2) = {-0.1f, +0.2f, 1.0f};
  *(ship->g_points + 3) = {-0.1f, -0.2f, 1.0f};
  *(ship->g_points + 4) = {-0.25f, -0.35f, 1.0f};

  ship->dr_Fuelpoints = (zoro::Vec2*) malloc(3 * sizeof(zoro::Vec2));
  ship->g_Fuelpoints = (zoro::Vec3*) malloc(3 * sizeof(zoro::Vec3));
  *(ship->g_Fuelpoints + 0) = {-0.13f, +0.19f, 1.0f};
  *(ship->g_Fuelpoints + 1) = {-0.13f, -0.19f, 1.0f};
  *(ship->g_Fuelpoints + 2) = {-0.55f, +0.0f, 1.0f};

}

void paintShip(TShip p_ship){

  zoro::Mat3 m = zoro::MatIdentity3();
  m = zoro::Mat3Multiply(zoro::Mat3Translate(p_ship.pos), m);
	m = zoro::Mat3Multiply(zoro::Mat3Scale(	p_ship.scale, p_ship.scale), m);
	m = zoro::Mat3Multiply(zoro::Mat3Rotate( p_ship.angle), m);

	for (int i = 0; i < p_ship.kNPoints; i++)
	{
		zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *( p_ship.g_points + i));
		*(p_ship.dr_points + i) = {tmp.x, tmp.y};
	}

  esat::DrawSetStrokeColor(p_ship.color.r,p_ship.color.g,p_ship.color.b,255);
	esat::DrawSetFillColor(p_ship.color.r,p_ship.color.g,p_ship.color.b,20);
	esat::DrawSolidPath(&p_ship.dr_points[0].x, p_ship.kNPoints);

  if (p_ship.IsPressing && esat::Time() - sprFuelTime > sprFuelTimeRef)
  {
    p_ship.isPaintingFuel = true;
  }

  if (p_ship.isPaintingFuel)
  {
    if (esat::Time() - sprFuelTime > sprFuelTimeRef){
      p_ship.isPaintingFuel = false;
      sprFuelTime = esat::Time();
    } 

    for (int i = 0; i < 3; i++)
	  {
		  zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *( p_ship.g_Fuelpoints + i));
		  *(p_ship.dr_Fuelpoints + i) = {tmp.x, tmp.y};
	  }
    esat::DrawSolidPath(&p_ship.dr_Fuelpoints[0].x, 3);
  }
}

void moveAsteroid(ast::TAsteroid *p_asteroid){
  
  p_asteroid->pos = zoro::SumVec2(p_asteroid->pos, zoro::ScaleVec2(p_asteroid->dir, p_asteroid->speed));
  
  if (p_asteroid->pos.x > 800) p_asteroid->pos.x = 1;
  if (p_asteroid->pos.x < 0) p_asteroid->pos.x = 799;
  if (p_asteroid->pos.y > 800) p_asteroid->pos.y = 1;
  if (p_asteroid->pos.y < 0) p_asteroid->pos.y = 799;
}

void paintAsteroid(ast::TAsteroid *p_asteroid){
  zoro::Mat3 m = zoro::MatIdentity3();
  
  m = zoro::Mat3Multiply(zoro::Mat3Translate(p_asteroid->pos), m);
	m = zoro::Mat3Multiply(zoro::Mat3Scale(	50, 50), m);
	m = zoro::Mat3Multiply(zoro::Mat3Rotate( p_asteroid->angle), m);

  ast::TAsteroidData temp = *(astDataTypes+p_asteroid->type);

  for (int i = 0; i < temp.kNPoints; i++)
	{
		zoro::Vec3 tmp = zoro::Mat3TransformVec3(m, *( temp.g_points + i));
		*(temp.dr_points + i) = {tmp.x, tmp.y};
	}
  esat::DrawSetStrokeColor(p_asteroid->color.r,p_asteroid->color.g,p_asteroid->color.b,255);
	esat::DrawSetFillColor(p_asteroid->color.r,p_asteroid->color.g,p_asteroid->color.b,10);
	esat::DrawSolidPath(&temp.dr_points[0].x, temp.kNPoints);
}

void asteroidManager(ast::TAsteroid *p_asteroid){
  ast::TAsteroid* p = p_asteroid;
  
  while (p != nullptr)
  {
    printf("\nx %.f - y: %.f", p->pos.x, p->pos.y);
    moveAsteroid(p);
    paintAsteroid(p);
    p = p->next;
  }
}

void input(){
  
  ship.acceleration = {0.0f,0.0f};
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
  {
    ship.acceleration.x = cos(ship.angle) * 0.19; 
    ship.acceleration.y = sin(ship.angle) * 0.19; 
    ship.IsPressing = !ship.IsPressing;
  }
  if (esat::IsSpecialKeyUp(esat::kSpecialKey_Up))
  {
    ship.IsPressing = false;
  }
  
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left))
  {
    ship.angle -= 0.075;
  }
  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right))
  {
    ship.angle += 0.075;
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

void ClearDrawCoolEffectUwU(){
  zoro::Vec2 p[4];
  p[0] = {-1.0f, -1.0f};
  p[1] = {801.0f, -1.0f};
  p[2] = {801.0f, 801.0f};
  p[3] = {-1.0f, 801.0f};
  
  esat::DrawSetStrokeColor(0,0,0,255);
	esat::DrawSetFillColor(0,0,0,100);
	esat::DrawSolidPath(&p[0].x, 4);

}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,800);
	WindowSetMouseVisibility(true);

  initAstData();
  resetShip(&ship);
  init();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

  	last_time = esat::Time(); 
  	esat::DrawBegin();
  	esat::DrawClear(0,0,0,255);
    ClearDrawCoolEffectUwU();

    input();
    movementShip();
    paintShip(ship);
    
    paintAsteroid(&asteroidTest);
    moveAsteroid(&asteroidTest);
    paintAsteroid(&asteroidTest1);
    moveAsteroid(&asteroidTest1);
    paintAsteroid(&asteroidTest2);
    moveAsteroid(&asteroidTest2);
    paintAsteroid(&asteroidTest3);
    moveAsteroid(&asteroidTest3);
    //asteroidManager(asteroids);
    
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
