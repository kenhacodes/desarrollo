#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "zoroMathLib.h"

struct rgb
{
	unsigned char r,g,b;
};

struct Tcolbox
{
	float minx,maxx,miny,maxy;
};

struct Tplayer
{
	int score = 0;
	zoro::Vec2D pos;
	zoro::Vec2D speed;
	bool flying = false;
	int fuel = 101;
};

struct Tenemy
{
	zoro::Vec2D pos;
	zoro::Vec2D dir;
	bool isAlive = true;
};

struct TDebris{
	zoro::Vec2D pos;
	bool IsPositioned;
	bool IsBeingGrabbed;
};

struct TRayCharles
{
	zoro::Vec2D pos;
	zoro::Vec2D dir;
	zoro::Vec2D origin;
	rgb color = {255,255,255};
	TRayCharles* next;
};

enum gamestate {
	PREPARATION = 0, 
	FUEL, 
	ENDING
};

const float koriginalWidth = 200;
const float koriginalHeight = 150;

float kscreenScale = 4.0f;
float reescaleSprite = 4.0f;
float scale;
float kwidth = 800.0f, kheight = 600.0f;

esat::SpriteTransform st;
esat::SpriteTransform stPlayer;
esat::SpriteTransform stItem;
esat::SpriteTransform stSpaceship;

esat::SpriteHandle spritesheet;
esat::SpriteHandle background;
esat::SpriteHandle* g_spr_player;
esat::SpriteHandle g_fuel;
esat::SpriteHandle* g_spaceship;
esat::SpriteHandle* g_spaceship_fuel;

unsigned char LEVEL = 1;
enum gamestate gamestate = PREPARATION;

zoro::Vec2D *p; //Square points 

Tplayer *player;
Tcolbox playerbbox;
int currentPlayer = 0; // 0 or 1

//Movement values
zoro::Vec2D gravity = {0.0f,0.8f};
float playerBaseSpeed = 2.0f;
float playerPreNormalizedSpeed = 26.0f;
float playerFuelPropulsion = -40.0f;

// Player var
float playerColOffset;
unsigned char playerFrame = 0;
unsigned char fly = 0; // +4
unsigned char right = 0; // +8
zoro::Vec2D reposition = {0,0};

// Ray
TRayCharles *ray;
int knBulletPerShot = 13;
float shotLength = 160;
bool shot_key_pressed = false;
float max_angle_shot = 0.6;
float mx,my;
zoro::Vec2D rayOriginPos;

// Player jetpac variables
zoro::Circle2D inner;
zoro::Circle2D outer;
int numTotalG_Points = 202;
zoro::Vec2D *g_points;
float normalAngle = (6.28f / 100);
float offset = (-3.14/4);
unsigned char percentage = 80;
unsigned char red, green;
int fuel_jetpac_delay = 800;

// Fuel
int fuelCount = 0;
zoro::Vec2D fuelPos = {50.0f,0.0f};
bool fuelIsBeingGrabbed = false;
bool fuelIsPositioned = false;
float fuelGravity = 0.33f;
float heightFuel;
int destination_posx;

// Spaceship
bool fuelFrame = 0; // [0,1]
TDebris *spaceDebris;
unsigned char currentShip = 0; // [0,1,2]

Tcolbox *platforms;

unsigned char fps=60; //Control de frames por segundo.

double current_time,last_time;
double chrono_time = 0;
double fuel_time = 0;
double shot_delay = 0;

float tempf;
bool AnimationState = false;

void resetValuesNextPhase();
void resetValuesNewGame();
void FuelSpawner();

bool checkHitBox(Tcolbox c1, Tcolbox c2){
	bool res = false;
	if (c1.minx < c2.maxx && c1.maxx > c2.minx && c1.miny < c2.maxy && c1.maxy > c2.miny)
	{
		res = true;
	}
	return res;
}
void setPlayerBbox(){
	playerbbox.minx = (*(player+currentPlayer)).pos.x;
	playerbbox.maxx = (*(player+currentPlayer)).pos.x + (esat::SpriteWidth((*(g_spr_player + fly + playerFrame)))*scale);
	playerbbox.miny = (*(player+currentPlayer)).pos.y;
	playerbbox.maxy = (*(player+currentPlayer)).pos.y + (esat::SpriteHeight((*(g_spr_player + fly + playerFrame)))*scale);
}

zoro::Vec2D grabPhysics(zoro::Vec2D item, zoro::Vec2D destination, float force){
	if (zoro::MagnitudeVec2D(zoro::SubtractVec2D(destination, item)) < 0.5*kscreenScale)
	{
		return item;
	}
	if (zoro::MagnitudeVec2D(zoro::SubtractVec2D(destination, item)) > koriginalWidth*0.5)
	{
		return destination;
	}
	
	if (zoro::MagnitudeVec2D(zoro::SubtractVec2D(destination, item)) > koriginalWidth*0.1)
	{
		return zoro::SumVec2D(item, zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(destination, item)),force*kscreenScale*2.33f));
	}
	return zoro::SumVec2D(item, zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D(destination, item)),force*kscreenScale));
}

void initParams(char *param){
	printf("\nInit Params.");
	kscreenScale = atof(param);
	kwidth = koriginalWidth * kscreenScale;
	kheight = koriginalHeight * kscreenScale;
}
void initValues(){

	gravity = zoro::ScaleVec2D(gravity, kscreenScale);
	playerBaseSpeed *= kscreenScale;
	playerPreNormalizedSpeed *= kscreenScale;
	playerFuelPropulsion *= kscreenScale;
	
	//SpriteTransform
	esat::SpriteTransformInit(&st);
	esat::SpriteTransformInit(&stPlayer);
	esat::SpriteTransformInit(&stItem);
	esat::SpriteTransformInit(&stSpaceship);
	
	// Pointers
	p = (zoro::Vec2D*)malloc(4*sizeof(zoro::Vec2D));
	player = (Tplayer*)malloc(2*sizeof(Tplayer));
	g_spr_player = (esat::SpriteHandle*) malloc(16*sizeof(esat::SpriteHandle));
	platforms = (Tcolbox*) malloc(4*sizeof(Tcolbox));
	g_points = (zoro::Vec2D*)malloc(numTotalG_Points*sizeof(zoro::Vec2D));
	g_spaceship = (esat::SpriteHandle*) malloc(12*sizeof(esat::SpriteHandle));
	g_spaceship_fuel = (esat::SpriteHandle*) malloc(2*sizeof(esat::SpriteHandle));
	spaceDebris = (TDebris*)malloc(3*sizeof(TDebris));
	
	

	TRayCharles* start = (TRayCharles*)malloc(sizeof(TRayCharles));
	start->dir = {0.0f,0.0f};
	start->pos = {-100.0f, -100.0f};

	ray = nullptr;
	
	start->next = ray;
	ray = start;
	
	shotLength *= kscreenScale;

	// Init values
	player->pos = { kwidth*0.2f, kheight*0.7f };
	(*(player+1)).pos = { kwidth*0.4f, kheight*0.7f };

	player->flying = false;
	(*(player+1)).flying = false;

	inner.radius = 0.9f * kscreenScale;
  outer.radius = 2.7f * kscreenScale;

  
	// Tcolbox
	(*(platforms + 0)) = {-1000,1000,144,150};
	(*(platforms + 1)) = {25,63,56,62};
	(*(platforms + 2)) = {94,119,75,81};
	(*(platforms + 3)) = {150,188,37,44};

	for (int i = 0; i < 4; i++)
	{
		(*(platforms + i)).maxx *= kscreenScale;
		(*(platforms + i)).minx *= kscreenScale;
		(*(platforms + i)).maxy *= kscreenScale;
		(*(platforms + i)).miny *= kscreenScale;
	}
	resetValuesNextPhase();
	resetValuesNewGame();
}
void initSprites(){
	printf("\nInit Sprites.");
	
	scale = (float) kscreenScale / (float) reescaleSprite;
	
	st.scale_x = scale;
	st.scale_y = scale;
	stPlayer.scale_x = scale;
	stPlayer.scale_y = scale;
	stItem.scale_x = scale;
	stItem.scale_y = scale;
	stSpaceship.scale_x = scale;
	stSpaceship.scale_y = scale;
	
	spritesheet = esat::SpriteFromFile("./assets/spritesjetpac.png");
	
	//Background
	background = esat::SpriteFromFile("./assets/background.png");
	
	//Walking
	*(g_spr_player+0) = esat::SubSprite(spritesheet,92,132,57,92);
	*(g_spr_player+1) = esat::SubSprite(spritesheet,184,132,57,92);
	*(g_spr_player+2) = esat::SubSprite(spritesheet,275,132,57,92);
	*(g_spr_player+3) = esat::SubSprite(spritesheet,367,132,57,92);
	
	// Jetpac active
	*(g_spr_player+4) = esat::SubSprite(spritesheet,769,132,57,92);
	*(g_spr_player+5) = esat::SubSprite(spritesheet,863,132,57,92);
	*(g_spr_player+6) = esat::SubSprite(spritesheet,964,132,57,92);
	*(g_spr_player+7) = esat::SubSprite(spritesheet,1060,132,57,92);

	//Walking right
	*(g_spr_player+8) = esat::SubSprite(spritesheet,98,256,57,92);
	*(g_spr_player+9) = esat::SubSprite(spritesheet,190,256,57,92);
	*(g_spr_player+10) = esat::SubSprite(spritesheet,282,256,57,92);
	*(g_spr_player+11) = esat::SubSprite(spritesheet,373,256,57,92);
	
	// Jetpac active right
	*(g_spr_player+12) = esat::SubSprite(spritesheet,772,248,57,92);
	*(g_spr_player+13) = esat::SubSprite(spritesheet,872,248,57,92);
	*(g_spr_player+14) = esat::SubSprite(spritesheet,972,248,57,92);
	*(g_spr_player+15) = esat::SubSprite(spritesheet,1068,248,57,92);
	
	// Items
	g_fuel = esat::SubSprite(spritesheet,92,428,64,44);


	// Spaceship
	// -> 1
	*(g_spaceship+0) = esat::SubSprite(spritesheet, 320,564,64,64);
	*(g_spaceship+1) = esat::SubSprite(spritesheet, 320,652,64,64);
	*(g_spaceship+2) = esat::SubSprite(spritesheet, 320,744,64,64);
	// -> 2
	*(g_spaceship+3) = esat::SubSprite(spritesheet, 460,564,64,64);
	*(g_spaceship+4) = esat::SubSprite(spritesheet, 460,656,64,64);
	*(g_spaceship+5) = esat::SubSprite(spritesheet, 460,744,64,64);
	// -> 3
	*(g_spaceship+6) = esat::SubSprite(spritesheet, 616,564,64,64);
	*(g_spaceship+7) = esat::SubSprite(spritesheet, 616,656,64,64);
	*(g_spaceship+8) = esat::SubSprite(spritesheet, 616,748,64,64);
	
	*(g_spaceship_fuel+0) = esat::SubSprite(spritesheet, 531, 911, 64, 60);
	*(g_spaceship_fuel+1) = esat::SubSprite(spritesheet, 651, 911, 64, 60);

	// Enemy

	esat::SpriteRelease(spritesheet);
	printf(" - END init sprites\n");
}
void freeValues(){
	free(g_spr_player);
	free(g_spaceship_fuel);
	free(p);
	free(player);
	free(platforms);
}

void resetValuesNextLevel(){
	
	fuelCount = 0;
	AnimationState = false;

	(spaceDebris + 2)->pos = {130,128};
	(spaceDebris + 0)->IsPositioned = true;
	(spaceDebris + 1)->IsPositioned = true;

	FuelSpawner();
}
void resetValuesNextPhase(){
	(spaceDebris + 0)->pos = {33,40};
	(spaceDebris + 1)->pos = {98,59};
	(spaceDebris + 2)->pos = {130,128};
	(spaceDebris + 0)->IsPositioned = false;
	(spaceDebris + 1)->IsPositioned = false;
	(spaceDebris + 0)->IsBeingGrabbed = false;
	(spaceDebris + 1)->IsBeingGrabbed = false;

	(spaceDebris + 2)->IsPositioned = true;
	(spaceDebris + 2)->IsBeingGrabbed = false;

	fuelPos = {-1000,-1000};
}
void resetValuesNewGame(){

}

void LevelManager(){
	if (LEVEL==5) currentShip = 1;
	if (LEVEL==9) currentShip = 2;
	if (LEVEL==13) currentShip = 0;
	
	resetValuesNextLevel();
	gamestate = FUEL;
	if (LEVEL == 5 || LEVEL == 9 || LEVEL == 13)
	{
		resetValuesNextPhase();
		gamestate = PREPARATION;
	}
	
}

void paintUI(){
	st.x=0;
	st.y=0;
	esat::DrawSprite(background, st);
}

void debugPaint(){
	esat::DrawSetFillColor(200,200,200,150);
	esat::DrawSetStrokeColor(255,0,0,255);
	for (int i = 0; i < 4; i++)
	{
		(*(p+0)).x = (*(platforms + i)).minx;
		(*(p+0)).y = (*(platforms + i)).miny;
		(*(p+1)).x = (*(platforms + i)).maxx;
		(*(p+1)).y = (*(platforms + i)).miny;
		(*(p+2)).x = (*(platforms + i)).maxx;
		(*(p+2)).y = (*(platforms + i)).maxy;
		(*(p+3)).x = (*(platforms + i)).minx;
		(*(p+3)).y = (*(platforms + i)).maxy;
		esat::DrawSolidPath(&p[0].x,4);
	}

	(*(p+0)).x = playerbbox.minx;
	(*(p+0)).y = playerbbox.miny;
	(*(p+1)).x = playerbbox.maxx;
	(*(p+1)).y = playerbbox.miny;
	(*(p+2)).x = playerbbox.maxx;
	(*(p+2)).y = playerbbox.maxy;
	(*(p+3)).x = playerbbox.minx;
	(*(p+3)).y = playerbbox.maxy;
	esat::DrawSetFillColor(200,200,200,10);
	esat::DrawSolidPath(&p[0].x,4);
}
void painThitbox(Tcolbox box){
    float p[10] = {
			box.minx, box.miny,
			box.maxx, box.miny,
			box.maxx, box.maxy,
			box.minx, box.maxy,
			box.minx, box.miny
		};
		//Color of outline. Red atm.
    esat::DrawSetStrokeColor(255,0,0,255);
    esat::DrawPath(p, 5);
}

void insertBulletInRay(TRayCharles** r, float i){
	mx = esat::MousePositionX();
	my = esat::MousePositionY();
	
	TRayCharles* new_ray = (TRayCharles*)malloc(sizeof(TRayCharles));
	new_ray->pos = (player+currentPlayer)->pos;
	new_ray->pos.y += (esat::SpriteWidth((*(g_spr_player + fly + playerFrame)))*scale)*0.79;
	if (mx > (player+currentPlayer)->pos.x + (esat::SpriteWidth((*(g_spr_player + fly + playerFrame)))*scale)*0.5){
		right = 8;
		new_ray->pos.x += (esat::SpriteWidth((*(g_spr_player + fly + playerFrame)))*scale);
	}else{
		right = 0;
	} 
	new_ray->origin = new_ray->pos;
	rayOriginPos = new_ray->origin;
	float baseSpeed = 1 * kscreenScale;
	float speed = baseSpeed + (i*0.2*kscreenScale) + i*kscreenScale;
	new_ray->dir = zoro::ScaleVec2D(zoro::NormalizeVec2D(zoro::SubtractVec2D({mx,my},new_ray->pos )), speed);
	zoro::Vec2D temp = {0.0f,0.0f};
	float radians = 0.0f;
	if (right == 0)
	{
		if (zoro::DotVec2D({-1.0f,0.0f}, zoro::NormalizeVec2D(new_ray->dir)) < max_angle_shot)
		{
			if (new_ray->dir.y < 0)
			{
				radians = (zoro::PI) + ( (zoro::PI / 2) * max_angle_shot);
				temp.x = cos(radians);
				temp.y = sin(radians);
				new_ray->dir = zoro::ScaleVec2D(temp, zoro::MagnitudeVec2D(new_ray->dir));
			}else{
				radians = ((3*zoro::PI)) - ( (zoro::PI / 2) * max_angle_shot);
				temp.x = cos(radians);
				temp.y = sin(radians);
				new_ray->dir = zoro::ScaleVec2D(temp, zoro::MagnitudeVec2D(new_ray->dir));
			}
		}
	}else{
		if (zoro::DotVec2D({1.0f,0.0f}, zoro::NormalizeVec2D(new_ray->dir)) < max_angle_shot)
		{
			if (new_ray->dir.y < 0)
			{
				radians = (zoro::PI*2) - ( (zoro::PI / 2) * max_angle_shot);
				temp.x = cos(radians);
				temp.y = sin(radians);
				new_ray->dir = zoro::ScaleVec2D(temp, zoro::MagnitudeVec2D(new_ray->dir));
			}else{
				radians = (zoro::PI*3) + (zoro::PI) + ( (zoro::PI / 2) * max_angle_shot);
				temp.x = cos(radians);
				temp.y = sin(radians);
				new_ray->dir = zoro::ScaleVec2D(temp, zoro::MagnitudeVec2D(new_ray->dir));
			}
		}
	}
	new_ray->next = (*r)->next;
	new_ray->color = {255,255,255};
	(*r)->next = new_ray;
}
void createRay(){
	for (int i = 0; i < knBulletPerShot; i++)
	{
		insertBulletInRay(&ray,i+1); //TODO ADD COLOR
	}
}
void cleanRay(TRayCharles *r){
	if (r!=NULL)
	{
		TRayCharles* pointer = r;
		TRayCharles* aux;
		
		while (pointer->next)
		{
			if (zoro::MagnitudeVec2D(zoro::SubtractVec2D(pointer->next->origin, pointer->next->pos)) > shotLength || pointer->next->pos.y > kheight*0.93)
			{
				aux = pointer->next;
				pointer->next = pointer->next->next;
				free(aux);
			}
			if(pointer->next != nullptr) pointer = pointer->next; 
		}
	}
}
void updateRay(TRayCharles *r){
	TRayCharles* auxd = r;
	while (auxd){
		//UPDATE
		auxd->pos = zoro::SumVec2D(auxd->pos, auxd->dir);
		//if (auxd->pos.x > kwidth) auxd->pos.x = 0;
		//if (auxd->pos.x < 0)  auxd->pos.x = kwidth;

		//DRAW
		*(p+0) = auxd->pos;
		*(p+1) = zoro::SumVec2D(*(p+0), zoro::ScaleVec2D(zoro::RightPerpendicularVec2D(zoro::NormalizeVec2D(auxd->dir)), kscreenScale*0.33));
		*(p+2) = zoro::SumVec2D(*(p+1), zoro::ScaleVec2D(zoro::NormalizeVec2D(auxd->dir), 5*kscreenScale));
		*(p+3) = zoro::SumVec2D(*(p+0), zoro::ScaleVec2D(zoro::NormalizeVec2D(auxd->dir), 5*kscreenScale));

		esat::DrawSetStrokeColor(auxd->color.r,auxd->color.g,auxd->color.b,255);
		esat::DrawSetFillColor(auxd->color.r,auxd->color.g,auxd->color.b,255);
		esat::DrawSolidPath(&p[0].x,4);
		auxd = auxd->next;
	}

	
}
void drawCrosshair(){

	mx = esat::MousePositionX();
	my = esat::MousePositionY();

	esat::DrawSetStrokeColor(150,255,0,150);
	
	zoro::Vec2D crosshair = zoro::SubtractVec2D({mx,my}, rayOriginPos);

	if (((zoro::DotVec2D({-1.0f,0.0f}, zoro::NormalizeVec2D(crosshair)) > max_angle_shot) &&
			zoro::DotVec2D({1.0f,0.0f}, zoro::NormalizeVec2D(crosshair)) < max_angle_shot) || 
			(zoro::DotVec2D({-1.0f,0.0f}, zoro::NormalizeVec2D(crosshair)) < max_angle_shot) &&
			zoro::DotVec2D({1.0f,0.0f}, zoro::NormalizeVec2D(crosshair)) > max_angle_shot)
	{
		esat::DrawSetStrokeColor(0,255,0,255);
	}

	esat::DrawLine(mx-1*kscreenScale, my-1*kscreenScale, mx-3*kscreenScale, my-3*kscreenScale);
	esat::DrawLine(mx+1*kscreenScale, my+1*kscreenScale, mx+3*kscreenScale, my+3*kscreenScale);
	esat::DrawLine(mx-1*kscreenScale, my+1*kscreenScale, mx-3*kscreenScale, my+3*kscreenScale);
	esat::DrawLine(mx+1*kscreenScale, my-1*kscreenScale, mx+3*kscreenScale, my-3*kscreenScale);
}

void GameInput(){

	bool pressed = false;
	(*(player+currentPlayer)).speed = {0.0f,0.0f};
	 
	if (esat::IsKeyPressed('W') && percentage > 1 &&  (esat::Time() - fuel_time) > fuel_jetpac_delay) // UP
	{
		if (percentage > 1) percentage--;
		if (percentage == 1 ) fuel_time = esat::Time();
		(*(player+currentPlayer)).speed = zoro::SumVec2D((*(player+currentPlayer)).speed, {0.0f,playerFuelPropulsion});
	}

	if (esat::IsKeyPressed('A')) // LEFT
	{
		pressed = true;
		right = 0;
		(*(player+currentPlayer)).speed = zoro::SumVec2D((*(player+currentPlayer)).speed, {-playerPreNormalizedSpeed,0.0f});
	}

	if (esat::IsKeyPressed('D')) // RIGHT
	{
		pressed = true;
		right = 8;
		(*(player+currentPlayer)).speed = zoro::SumVec2D((*(player+currentPlayer)).speed, {playerPreNormalizedSpeed,0.0f});
	}

	if ((*(player+currentPlayer)).flying || pressed)
	{
		if (esat::Time() - chrono_time > 100)
		{
			playerFrame++;
			chrono_time = esat::Time();
		}
	}
	if (esat::MouseButtonDown(0) || shot_key_pressed)
	{
		if (esat::Time() - shot_delay > 333)
		{
			createRay(); 
			shot_delay = esat::Time();
			shot_key_pressed = false;
		}else{
			shot_key_pressed = true;
		}
		
		
	}
	
	if (playerFrame==4) playerFrame=0;
	
	(*(player+currentPlayer)).speed = zoro::ScaleVec2D(zoro::NormalizeVec2D((*(player+currentPlayer)).speed),playerBaseSpeed);

}
void paintJetpacFuelCircle(){
	
	inner.c = {playerbbox.maxx + (playerbbox.maxx-playerbbox.minx)/3,playerbbox.miny};
  outer.c = {playerbbox.maxx + (playerbbox.maxx-playerbbox.minx)/3,playerbbox.miny};	

	red = 240 + (-240) * (percentage*0.01);
  green = 1 + (230- 1) * (percentage*0.01);
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
	
	// Outer area
  for (int i = percentage; i > 0; i--)
  {
    g_points[percentage + percentage-i].x = cos(offset+normalAngle * (i)) * outer.radius + outer.c.x;
    g_points[percentage+ percentage-i].y = sin(offset+normalAngle * (i)) * outer.radius + outer.c.y;
  }
  
	esat::DrawSolidPath(&g_points[0].x, ((percentage*2)));

}
void playerPhysics(){
	if ((*(player+currentPlayer)).speed.y < 0 && !(*(player+currentPlayer)).flying)
	{
		(*(player+currentPlayer)).flying = true;
		//(*(player+currentPlayer)).speed = zoro::SumVec2D((*(player+currentPlayer)).speed, {0.0f,playerFuelPropulsion/4});
	}

	(*(player+currentPlayer)).speed = zoro::SumVec2D((*(player+currentPlayer)).speed, gravity);
	(*(player+currentPlayer)).pos = zoro::SumVec2D((*(player+currentPlayer)).pos, (*(player+currentPlayer)).speed);
	
	if((*(player+currentPlayer)).flying) fly = 4;
	else fly = 0;

	setPlayerBbox();
}
void playerColision(){
	bool isColiding = false;
	reposition = {0.0f,0.0f};
	playerColOffset =  (playerbbox.maxx - playerbbox.minx);
	playerbbox.minx += playerColOffset*0.05;
	playerbbox.maxx -= playerColOffset*0.05;
	playerbbox.miny += playerColOffset*0.05;
	for (int i = 0; i < 4; i++)
	{
		if (checkHitBox((*(platforms + i)),playerbbox))
		{
			isColiding = true;
			if(playerbbox.miny < (*(platforms + i)).maxy && playerbbox.maxy < (*(platforms + i)).maxy){
				if(percentage<101) percentage++;
				
				reposition.y -=  playerbbox.maxy-(*(platforms + i)).miny;
				(*(player+currentPlayer)).flying = false;
				fly = 0;
			}
			if (playerbbox.miny > (*(platforms + i)).miny && playerbbox.maxy > (*(platforms + i)).miny)
			{
				reposition.y -=  playerbbox.miny-(*(platforms + i)).maxy;
			}
			
			if(playerbbox.minx < (*(platforms + i)).maxx && playerbbox.maxx < (*(platforms + i)).maxx &&
				playerbbox.miny < (*(platforms + i)).miny && playerbbox.maxy > (*(platforms + i)).maxy){
				reposition.x -=  playerbbox.maxx-(*(platforms + i)).minx;
			}

			if(playerbbox.minx > (*(platforms + i)).minx && playerbbox.maxx > (*(platforms + i)).minx &&
				playerbbox.miny < (*(platforms + i)).miny && playerbbox.maxy > (*(platforms + i)).maxy){
				reposition.x -=  playerbbox.minx-(*(platforms + i)).maxx;
			}
			(*(player+currentPlayer)).pos = zoro::SumVec2D((*(player+currentPlayer)).pos,reposition);
			
		}
		if (!isColiding)
		{
			(*(player+currentPlayer)).flying = true;
			fly = 4;
		}
		
	}
	setPlayerBbox();
}
void paintPlayer(){
	//Real player
	stPlayer.x = (*(player+currentPlayer)).pos.x;
	stPlayer.y = (*(player+currentPlayer)).pos.y;
	
	(*(p+0)).x = playerbbox.minx+1;
	(*(p+0)).y = playerbbox.miny+1;
	(*(p+1)).x = playerbbox.maxx-1;
	(*(p+1)).y = playerbbox.miny+1;
	(*(p+2)).x = playerbbox.maxx-1;
	(*(p+2)).y = playerbbox.maxy-1;
	(*(p+3)).x = playerbbox.minx+1;
	(*(p+3)).y = playerbbox.maxy-1;
	
	esat::DrawSetFillColor(225,225,225,255);
	esat::DrawSetStrokeColor(0,0,0,0);
	esat::DrawSolidPath(&p[0].x,4);
	esat::DrawSprite(g_spr_player[fly+playerFrame+right],stPlayer);

	tempf = (esat::SpriteWidth(g_spr_player[fly+playerFrame+right])/2)*scale;
	
	//Infinte movement
	if((player+currentPlayer)->pos.x + tempf > kwidth) (player+currentPlayer)->pos.x = -tempf+1; 
	if((player+currentPlayer)->pos.x + tempf < 0) (player+currentPlayer)->pos.x = kwidth-tempf; 

	//Phantom player
	if (stPlayer.x > kwidth/2)
	{
		stPlayer.x -= kwidth;

		(*(p+0)).x -= kwidth;
		(*(p+1)).x -= kwidth;
		(*(p+2)).x -= kwidth;
		(*(p+3)).x -= kwidth;
	} 
	else{
		stPlayer.x += kwidth;

		(*(p+0)).x += kwidth;
		(*(p+1)).x += kwidth;
		(*(p+2)).x += kwidth;
		(*(p+3)).x += kwidth;
	} 
	esat::DrawSetFillColor(225,225,225,255);
	esat::DrawSetStrokeColor(0,0,0,0);
	esat::DrawSolidPath(&p[0].x,4);
	esat::DrawSprite(g_spr_player[fly+playerFrame+right],stPlayer);
	

	
}

void PlayerController(){
	GameInput();
	playerPhysics();
	playerColision();
	paintJetpacFuelCircle();
	paintPlayer();
	cleanRay(ray);
	drawCrosshair();
	updateRay(ray);
}

void GrabFuel(){
	setPlayerBbox();
	destination_posx =  (*(spaceDebris+2)).pos.x * kscreenScale;
	if (!fuelIsBeingGrabbed && checkHitBox(playerbbox, 
			{
				fuelPos.x, (fuelPos.x) + (esat::SpriteWidth(g_fuel)*scale),
				fuelPos.y, (fuelPos.y) + (esat::SpriteWidth(g_fuel)*scale)
			}) && !(fuelPos.x > destination_posx - 4 && fuelPos.x < destination_posx + 4))
	{
		fuelIsBeingGrabbed = true;
	}

	if (fuelIsBeingGrabbed)
	{
			fuelPos = grabPhysics(fuelPos, (player+currentPlayer)->pos , 0.5f);
			if ((fuelPos.x > destination_posx - 4 && fuelPos.x < destination_posx + 4))
			{
				fuelIsBeingGrabbed = false;
				fuelPos.x = destination_posx;
			}
	}
}

void FuelSpawner(){
	if (fuelCount >= 6)
	{
		return;
	}
	
	fuelPos.x = (int)(kwidth*0.1f)+rand()%(int)(kwidth*0.7f);
	fuelPos.y = -10*kscreenScale - esat::SpriteHeight(g_fuel);
	destination_posx =  (*(spaceDebris+2)).pos.x * kscreenScale;
	if (fuelPos.x  > destination_posx - 4 && fuelPos.x  < destination_posx + 4)
	{
		FuelSpawner();
	}
	
	
}
void FuelPhysics(){
	destination_posx =  (*(spaceDebris+2)).pos.x * kscreenScale;
	if(!fuelIsBeingGrabbed){
		if (fuelPos.x == destination_posx && fuelPos.y+(esat::SpriteHeight(g_fuel)*scale) > (144*kscreenScale)-1)
		{
			if (fuelCount < 6)
			{
				
				fuelCount++;	
				FuelSpawner();
			}
		}
		fuelPos.y += fuelGravity * kscreenScale;
		for (int i = 0; i < 4; i++)
		{
			if (checkHitBox(*(platforms+i), {fuelPos.x, fuelPos.x+(esat::SpriteWidth(g_fuel)*scale),
																		 fuelPos.y, fuelPos.y+(esat::SpriteHeight(g_fuel)*scale)}))
			{
				fuelPos =  zoro::SubtractVec2D(fuelPos, {0.0f, (fuelPos.y+(esat::SpriteHeight(g_fuel)*scale))-(*(platforms + i)).miny});
			}
		}
	}
	
	
	

}
void FuelPaint(bool sprite){
	stItem.x = fuelPos.x;
	stItem.y = fuelPos.y;

	(*(p+0)).x = stItem.x +1;
	(*(p+0)).y = stItem.y +1;
	(*(p+1)).x = stItem.x + (esat::SpriteWidth(g_fuel)*scale) -1;
	(*(p+1)).y = stItem.y +1;
	(*(p+2)).x = stItem.x + (esat::SpriteWidth(g_fuel)*scale)-1;
	(*(p+2)).y = stItem.y + (esat::SpriteHeight(g_fuel)*scale)-1;
	(*(p+3)).x = stItem.x +1;
	(*(p+3)).y = stItem.y + (esat::SpriteHeight(g_fuel)*scale)-1;

	esat::DrawSetFillColor(200,50,200,255);
	esat::DrawSetStrokeColor(0,0,0,0);
	esat::DrawSolidPath(&p[0].x,4);
	if (sprite) esat::DrawSprite(g_fuel,stItem);	
}

void FuelController(){
	GrabFuel();
	FuelPhysics();
	FuelPaint(true);
	if (fuelCount == 6 
		&& checkHitBox(playerbbox,{ (spaceDebris + 2)->pos.x*kscreenScale, (spaceDebris + 2)->pos.x*kscreenScale+(64*scale),
																(spaceDebris + 2)->pos.y*kscreenScale, (spaceDebris + 2)->pos.y*kscreenScale+(64*scale)}))
	{
		fuelPos = {-1000.0f, -1000.0f};
		gamestate = ENDING;
	}
	
}

void UpdateSpaceship(){
	for (int i = 0; i < 3; i++)
	{
		stSpaceship.x = (*(spaceDebris+i)).pos.x * kscreenScale;
		stSpaceship.y = (*(spaceDebris+i)).pos.y * kscreenScale;

		(*(p+0)).x = stSpaceship.x;
		(*(p+0)).y = stSpaceship.y;
		(*(p+1)).x = stSpaceship.x + (esat::SpriteWidth((*(g_spaceship + (currentShip*3)))) * scale);
		(*(p+1)).y = stSpaceship.y;
		(*(p+2)).x = stSpaceship.x + (esat::SpriteWidth((*(g_spaceship+(currentShip*3)))) * scale);
		(*(p+2)).y = stSpaceship.y + (esat::SpriteHeight((*(g_spaceship+(currentShip*3)))) * scale);
		(*(p+3)).x = stSpaceship.x;
		(*(p+3)).y = stSpaceship.y + (esat::SpriteHeight((*(g_spaceship+(currentShip*3)))) * scale);

		esat::DrawSetFillColor(200,200,200,255);
		esat::DrawSetStrokeColor(0,0,0,0);
		esat::DrawSolidPath(&p[0].x,4);
	}	
	stSpaceship.x = (*(spaceDebris+2)).pos.x * kscreenScale;
	stSpaceship.y = (*(spaceDebris+2)).pos.y * kscreenScale;
	
	heightFuel = ((esat::SpriteHeight((*(g_spaceship+(currentShip*3)))) * scale)*3)/6;
	if (fuelCount>0)
	{
		*(p+0) = { stSpaceship.x, stSpaceship.y + (esat::SpriteHeight((*(g_spaceship+(currentShip*3)))) * scale) - heightFuel*fuelCount}; 
		*(p+1) = { stSpaceship.x + (esat::SpriteWidth((*(g_spaceship+(currentShip*3)))) * scale), (p+0)->y };
		*(p+2) = { (p+1)->x, stSpaceship.y + (esat::SpriteHeight((*(g_spaceship+(currentShip*3)))) * scale) };
		*(p+3) = { stSpaceship.x, stSpaceship.y + (esat::SpriteHeight((*(g_spaceship+(currentShip*3)))) * scale)};

		esat::DrawSetFillColor(200,50,200,255);
		esat::DrawSetStrokeColor(0,0,0,0);
		esat::DrawSolidPath(&p[0].x,4);	
	}

	if (fuelPos.x == destination_posx && fuelPos.y > (*(spaceDebris)).pos.y * kscreenScale) FuelPaint(false);

	for (int i = 0; i < 3; i++)
	{
		stSpaceship.x = (*(spaceDebris+i)).pos.x * kscreenScale;
		stSpaceship.y = (*(spaceDebris+i)).pos.y * kscreenScale;
		esat::DrawSprite(g_spaceship[i + (currentShip*3)],stSpaceship);
	}
}
void grabSpaceShipdebris(){
	//If ship debris center is NOT positioned yet check col and grab
	
	if(!(spaceDebris + 1)->IsPositioned){
		
		if ((spaceDebris + 1)->IsBeingGrabbed)
		{
			(spaceDebris + 1)->pos = grabPhysics((spaceDebris + 1)->pos, zoro::DivideByScaleVec2D((player+currentPlayer)->pos,kscreenScale) , 0.33f);

		}else{
			if (checkHitBox(playerbbox, 
				{ (spaceDebris + 1)->pos.x*kscreenScale, (spaceDebris + 1)->pos.x*kscreenScale+(64*scale),
					(spaceDebris + 1)->pos.y*kscreenScale, (spaceDebris + 1)->pos.y*kscreenScale+(64*scale)}))
			{
				(spaceDebris + 1)->IsBeingGrabbed = true;
			}
		}
		
	}else{
		if ((spaceDebris + 0)->IsBeingGrabbed)
		{
			(spaceDebris + 0)->pos = grabPhysics((spaceDebris + 0)->pos, zoro::DivideByScaleVec2D((player+currentPlayer)->pos,kscreenScale) , 0.33f);

		}else{
			if (checkHitBox(playerbbox, 
				{ (spaceDebris + 0)->pos.x*kscreenScale, (spaceDebris + 0)->pos.x*kscreenScale+(64*scale),
					(spaceDebris + 0)->pos.y*kscreenScale, (spaceDebris + 0)->pos.y*kscreenScale+(64*scale)}))
			{
				(spaceDebris + 0)->IsBeingGrabbed = true;
			}
		}
	}
	/*
	if(//col //debactive ){
		//debpos = debpos + playerpos
	}
	*/
	
}
void SpaceShipdebrisPhysics(){
	for (int i = 0; i < 2; i++)
	{
		int posx = (*(spaceDebris+i)).pos.x;
		destination_posx =  (*(spaceDebris+2)).pos.x;
		
		if (posx > destination_posx - 4 && posx < destination_posx + 4)
		{
			(spaceDebris + i)->IsBeingGrabbed = false;
			(spaceDebris + i)->IsPositioned = true;
			(*(spaceDebris+i)).pos.x = destination_posx;
			
			(spaceDebris+i)->pos.y += 1.0f;
			
			if (i==1)
			{
				if ((*(spaceDebris+i)).pos.y + (esat::SpriteHeight((*(g_spaceship + i + (currentShip*3)))) / reescaleSprite) > (*(spaceDebris+2)).pos.y )
				{
					(*(spaceDebris+i)).pos.y = (*(spaceDebris+2)).pos.y - (esat::SpriteHeight((*(g_spaceship + i + (currentShip*3)))) / reescaleSprite);
				}
			}else{
				if ((*(spaceDebris+i)).pos.y + (esat::SpriteHeight((*(g_spaceship + i + (currentShip*3)))) / reescaleSprite) > (*(spaceDebris+1)).pos.y)
				{
					if(gamestate == PREPARATION){
						FuelSpawner();
						gamestate = FUEL;
					} 
					(*(spaceDebris+i)).pos.y = (*(spaceDebris+2)).pos.y - ((esat::SpriteHeight((*(g_spaceship + i + (currentShip*3)))) / reescaleSprite) + (esat::SpriteHeight((*(g_spaceship + 1 + (currentShip*3)))) / reescaleSprite));
				}
			}	
		}
	}
}

void endingAnim(){
	if (AnimationState)
	{
		// DESCENDING
		for (int i = 0; i < 3; i++)
		{
			(spaceDebris+i)->pos.y += 1.0f;
		}	
		if (checkHitBox( *(platforms + 0),{ (spaceDebris + 2)->pos.x*kscreenScale, (spaceDebris + 2)->pos.x*kscreenScale+(64*scale),
																				(spaceDebris + 2)->pos.y*kscreenScale, (spaceDebris + 2)->pos.y*kscreenScale+(64*scale)} ))
		{
			LEVEL++;
			LevelManager();
		}
		
	}else{
		// ASCENDING
		for (int i = 0; i < 3; i++)
		{
			(spaceDebris+i)->pos.y -= 1.0f;
		}
		if ((spaceDebris+0)->pos.y < -10*kscreenScale) AnimationState = true;
	}

	// Fuel ascending Spaceship
	if ((spaceDebris+2)->pos.y < 133*kscreenScale)
	{
		stSpaceship.x = (*(spaceDebris+2)).pos.x * kscreenScale;
		stSpaceship.y = (*(spaceDebris+2)).pos.y * kscreenScale;
		
		//printf("\nPos: x[%f] y[%f]", stSpaceship.x, stSpaceship.y);
		(*(p+0)).x = stSpaceship.x;
		(*(p+0)).y = stSpaceship.y;
		(*(p+1)).x = stSpaceship.x + (64 * scale);
		(*(p+1)).y = stSpaceship.y;
		(*(p+2)).x = stSpaceship.x + (64 * scale);
		(*(p+2)).y = stSpaceship.y + ((64 * scale)*2);
		(*(p+3)).x = stSpaceship.x;
		(*(p+3)).y = stSpaceship.y + ((64 * scale)*2);

		esat::DrawSetFillColor(255,255,255,100);
		esat::DrawSetStrokeColor(0,0,0,0);
		//esat::DrawSolidPath(&p[0].x,4);
		
		// No funciona por alguna razon. :(
			
		//stItem.y =  stSpaceship.y + (64*scale);
		//esat::DrawSprite(g_spaceship_fuel, stItem);
	}
}

void game(){
	switch (gamestate)
	{
	case PREPARATION:
		grabSpaceShipdebris();
		SpaceShipdebrisPhysics();
		PlayerController();
		UpdateSpaceship();
		break;
	case FUEL:
		GrabFuel();
		FuelPhysics();
		FuelController();
		PlayerController();
		UpdateSpaceship();
		break;
	case ENDING:
		endingAnim();
		UpdateSpaceship();
		break;
	default:
		break;
	}
}

int esat::main(int argc, char **argv) {
	
	srand(time(NULL));

	if (argc == 2) initParams(*(argv+1));
	else printf("\nValores de ventana por defecto.");

	esat::WindowInit(kwidth, kheight);
	WindowSetMouseVisibility(true);
  
	initValues();
	initSprites();
	printf("\n kScreenScale: %.f \n", kscreenScale);
	printf("\n - Gemu Starto! - \n");
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(33,33,33);
		
		// -------------------------
		

		paintUI();
		game();
		


		//debugPaint();
		
		
		// -------------------------
    esat::DrawEnd();
    do{
    	current_time = esat::Time();
    }while((current_time-last_time)<=1000.0/fps);
    esat::WindowFrame();
}
  
freeValues();
esat::WindowDestroy();

  
return 0;
    
}
