/*	
		Author: Guillermo Boscá
		Date: 19/12/2023
*/

#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// -- CONSTANTS --

// Screen sizes
const int k_width = 224, k_height = 256;
const float k_scale = 3;
const int k_total_background_stars = 140;
const float k_PI = 3.141592;

// Game Order Reference 
// -> StartAnim,Eye, Fly, Docking, Ships, Meteors, Docking, Pile, EndAnim. -> (Next level) 

//ENUMS TYPE VARIABLES
// Used for level selection
enum gameManager {GameOver ,Menu, Scoreboard, Docking, StartAnim, EndAnim, EyeGreen, EyeBlue,
	FlyGreen, FlyBlue, ShipsGreen, ShipsBlue, MeteorGreen, MeteorBlue, PileGreen, PileBlue};
// Used to know enemy type
enum enemyType {Eye, Fly, Ship, Meteor, Pile};
// Used for level state. Levels always start at Pre. 
enum levelState {Pre, OnGoing, Pos};
// Used to know the color of the enemy.
enum enemyColor {Green, Blue};
// Different endings to the Docking levels.
enum dockingState {OutOfTime, Success, Failure};

//STRUCTS
//Used as coordinates.
struct Tcoord{
	float x, y;
};
//Used to calculate box type colisions.
struct Thitbox{
	float minx;
	float maxx;
	float miny;
	float maxy;
};
//Stars in the background
struct TbackgroundStar
{
	float x, y;
	bool visible;
	int red, green, blue;
	int cooldown;
};
//Bullet of player
struct Tbullet
{
	float x,y;
	bool isActive;
};
//Player
struct Tplayer {
	esat::SpriteTransform st;
	int currentShip = 1;
	bool ship1_isAlive = true, ship2_isAlive = true, ship3_isAlive = true;
	bool ship1_isActive = true, ship2_isActive = false, ship3_isActive = false;
	Tbullet bullets[5];
	bool isExploding = false;
	int score = 0;
};
//Move pattern structure
struct TMovePattern{
	int nextPattern;
	float speed;
	float max_rotations;
	Tcoord rotation_center_offset;
	bool isEligible;
};
//Enemy data variables. Variables change usage depending on Enemy Type!
struct TEnemy{
	enum enemyType type;
	enum enemyColor color;
	bool isAlive = true;
	bool isAggresive;
	bool isMoving;
	bool isAppearing = false;
	bool joined;
	bool isExploding = false;
	int explodingCounter = 0;
	esat::SpriteTransform st;
	Tcoord ref_rotation;
	Tcoord start_pos;
	float rotations;
	float speed;
	float max_rotations;
	int pattern;
};
// High Score Struct 
struct THiScore{
	int score = 5000;
	char name[12] = {"Nichibutsu\0"};
};

//Fps control
unsigned char fps=25; 
double current_time,last_time;

// GLOBAL VARIABLES

//Controls game state.
enum gameManager g_gameManager;
enum gameManager nextLevel; // Used before calling a Docking level so it knows where to go after.
enum levelState g_levelState = Pre;

int level = 1;
int g_credits = 0;

bool twoPlayers = false; 
int g_playerid = 0; // Only two players so can be 0 or 1.



float g_splineStep = 0;
double g_aliveOnLevelTime;

int g_trueHeight, g_trueWidth; // True hight & width screen scaled.
int g_bulletHeight, g_bulletWidth; 
int g_playerSpeed;
int playerSpriteExplosion = 0; 

//Docking variables
esat::SpriteTransform dockingst;
enum dockingState g_dockingState;
int dockingship;
int dockingBonus;
float g_gravity_down = 0.2;
float g_gravity_side;
float g_dockingHeight = 0;

//Meteor
int meteorCounter = 0;

//UI
THiScore hiscore[5]; // List of highscores.
char outputArray[7]; // Used to generate strings for the UI.
// Used as position for menu start timing of texts.
int g_menutext_1_x;
int g_menutext_2_x;
int g_menutext_3_x;

//Patterns for enemies
TMovePattern patternsEye[18];
TMovePattern patternsFly[18];
TMovePattern patternsShip[19];
TMovePattern patternsPile[4];

//Array of Enemies + Players
Tplayer players[2];
TEnemy eyeEnemies[16];
TEnemy flyEnemies[16];
TEnemy shipEnemies[16];
TEnemy meteorEnemies[16];
TEnemy pileEnemies[16];

//Sprites

//Player Sprites and variable of sprites
esat::SpriteHandle g_player_spr[3], g_fuel[3], g_playerExplosion[4];
int g_fuel_counter = 0, g_fuel_cooldown;

//Enemies Sprites	
esat::SpriteHandle g_spr_eye_blue[15], g_spr_eye_green[15];
esat::SpriteHandle g_spr_fly_blue[15], g_spr_fly_green[15];
esat::SpriteHandle g_spr_ship_blue[15], g_spr_ship_green[15];
esat::SpriteHandle g_spr_pile_blue[14], g_spr_pile_green[14];
esat::SpriteHandle g_spr_meteor_blue[8], g_spr_meteor_green[8];
esat::SpriteHandle g_spr_meteor_explosion[8], g_spr_enemy_explosion[4];

//Fonts Sprites
esat::SpriteTransform st_fonts;
esat::SpriteHandle g_spr_nichibitsu;

TbackgroundStar g_backgroundStars[k_total_background_stars]; 

// FUNCTIONS

//START and END FUNCTIONS

//Initializes sounds used in game.
void initSounds(){
	//NO SOUNDS.
}
//Initializes sprites used in game + fonts.
void initSprites(){
	// -- Font Sprites --
	esat::SpriteTransformInit(&st_fonts);
	st_fonts.scale_x = k_scale;
	st_fonts.scale_y = k_scale;
	g_spr_nichibitsu = esat::SpriteFromFile("./resources/sprites/font_sprites/nichibitsu.png");
	

	// -- Player Sprites --
	g_player_spr[0] = esat::SpriteFromFile("./resources/sprites/player/ship1.png");
	g_player_spr[1] = esat::SpriteFromFile("./resources/sprites/player/ship2.png");
	g_player_spr[2] = esat::SpriteFromFile("./resources/sprites/player/ship3.png");

	

	esat::SpriteHandle fuelSheet = esat::SpriteFromFile("./resources/sprites/player/fuel.png");
	g_fuel[0] = esat::SubSprite(fuelSheet, 0,0,18,16);
	g_fuel[1] = esat::SubSprite(fuelSheet, 18,0,18,16);
	g_fuel[2] = esat::SubSprite(fuelSheet, 36,0,18,16);
	esat::SpriteRelease(fuelSheet);

	esat::SpriteHandle playerExplosionSheet = esat::SpriteFromFile("./resources/sprites/player/player_explosion.png");
	g_playerExplosion[0] = esat::SubSprite(playerExplosionSheet, 0,0,34,35);
	g_playerExplosion[1] = esat::SubSprite(playerExplosionSheet, 34,0,34,35);
	g_playerExplosion[2] = esat::SubSprite(playerExplosionSheet, 68,0,34,35);
	g_playerExplosion[3] = esat::SubSprite(playerExplosionSheet, 102,0,34,35);
	esat::SpriteRelease(playerExplosionSheet);

	// -- Enemy Sprites --
	
	// -- Enemy explosions (meteor and enemy)


	esat::SpriteHandle meteorExplosionSheet = esat::SpriteFromFile("./resources/sprites/enemies/meteor_explosion.png");
	for (int i = 0; i < 4; i++)
	{
		g_spr_meteor_explosion[i] = esat::SubSprite(meteorExplosionSheet, i*34, 0, 34, 33);
		g_spr_meteor_explosion[i+4] = esat::SubSprite(meteorExplosionSheet, i*34, 33, 34, 33);
	}
	esat::SpriteRelease(meteorExplosionSheet);

	esat::SpriteHandle enemyExplosionSheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_explosion.png");

	for (int i = 0; i < 4; i++)
	{
		g_spr_enemy_explosion[i] = esat::SubSprite(enemyExplosionSheet, i*18, 0, 18, 18);
	}
	esat::SpriteRelease(enemyExplosionSheet);
	// -- Eye enemy
	
	esat::SpriteHandle enemyEyeBlue1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_eye_blue_1.png");
	esat::SpriteHandle enemyEyeBlue2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_eye_blue_2.png");
	esat::SpriteHandle enemyEyeGreen1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_eye_green_1.png");
	esat::SpriteHandle enemyEyeGreen2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_eye_green_2.png");

	for (int i = 0; i < 6; i++)
	{
		g_spr_eye_blue[i] = esat::SubSprite(enemyEyeBlue1Sheet, i*18,0,17,16);
		g_spr_eye_green[i] = esat::SubSprite(enemyEyeGreen1Sheet, i*18,0,17,16);
	}
		g_spr_eye_blue[6] = esat::SubSprite(enemyEyeBlue1Sheet, 108,0,16,16);
		g_spr_eye_green[6] = esat::SubSprite(enemyEyeGreen1Sheet, 108,0,16,16);
	for (int i = 0; i < 8; i++)
	{
		g_spr_eye_blue[i+7] = esat::SubSprite(enemyEyeBlue2Sheet, i*17,0,17,16);
		g_spr_eye_green[i+7] = esat::SubSprite(enemyEyeGreen2Sheet, i*17,0,17,16);
	}

	esat::SpriteRelease(enemyEyeBlue1Sheet);
	esat::SpriteRelease(enemyEyeBlue2Sheet);
	esat::SpriteRelease(enemyEyeGreen1Sheet);
	esat::SpriteRelease(enemyEyeGreen2Sheet);
	
	// -- Fly enemy

	esat::SpriteHandle enemyFlyBlue1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_fly_blue_1.png");
	esat::SpriteHandle enemyFlyBlue2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_fly_blue_2.png");
	esat::SpriteHandle enemyFlyGreen1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_fly_green_1.png");
	esat::SpriteHandle enemyFlyGreen2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_fly_green_2.png");

	for (int i = 0; i < 7; i++)
	{
		g_spr_fly_blue[i] = esat::SubSprite(enemyFlyBlue1Sheet, i*18,0,17,17);
		g_spr_fly_green[i] = esat::SubSprite(enemyFlyGreen1Sheet, i*17,0,17,13);
	}
	for (int i = 0; i < 8; i++)
	{
		g_spr_fly_blue[i+7] = esat::SubSprite(enemyFlyBlue2Sheet, i*17,0,18,17);
		g_spr_fly_green[i+7] = esat::SubSprite(enemyFlyGreen2Sheet, i*17,0,18,18);
	}

	esat::SpriteRelease(enemyFlyBlue1Sheet);
	esat::SpriteRelease(enemyFlyBlue2Sheet);
	esat::SpriteRelease(enemyFlyGreen1Sheet);
	esat::SpriteRelease(enemyFlyGreen2Sheet);
	
	// -- Ship enemy

	esat::SpriteHandle enemyShipBlue1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_ship_blue_1.png");
	esat::SpriteHandle enemyShipBlue2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_ship_blue_2.png");
	esat::SpriteHandle enemyShipGreen1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_ship_green_1.png");
	esat::SpriteHandle enemyShipGreen2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_ship_green_2.png");

	for (int i = 0; i < 7; i++)
	{
		g_spr_ship_blue[i] = esat::SubSprite(enemyShipBlue1Sheet, i*18,0,18,19);
		g_spr_ship_green[i] = esat::SubSprite(enemyShipGreen1Sheet, i*18,0,18,17);
	}
	for (int i = 0; i < 8; i++)
	{
		g_spr_ship_blue[i+7] = esat::SubSprite(enemyShipBlue2Sheet, i*18,0,18,18);
		g_spr_ship_green[i+7] = esat::SubSprite(enemyShipGreen2Sheet, i*18,0,18,19);
	}

	esat::SpriteRelease(enemyShipBlue1Sheet);
	esat::SpriteRelease(enemyShipBlue2Sheet);
	esat::SpriteRelease(enemyShipGreen1Sheet);
	esat::SpriteRelease(enemyShipGreen2Sheet);

	// -- Pile enemy
	
	esat::SpriteHandle enemyPileBlue1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_pile_blue_1.png");
	esat::SpriteHandle enemyPileBlue2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_pile_blue_2.png");
	esat::SpriteHandle enemyPileGreen1Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_pile_green_1.png");
	esat::SpriteHandle enemyPileGreen2Sheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_pile_green_2.png");	

	for (int i = 0; i < 7; i++)
	{
		g_spr_pile_blue[i] = esat::SubSprite(enemyPileBlue1Sheet, i*18,0,18,18);
		g_spr_pile_blue[i+7] = esat::SubSprite(enemyPileBlue2Sheet, i*18,0,18,18);

		g_spr_pile_green[i] = esat::SubSprite(enemyPileGreen1Sheet, i*18,0,18,17);
		g_spr_pile_green[i+7] = esat::SubSprite(enemyPileGreen2Sheet, i*18,0,18,17);
	}
	
	esat::SpriteRelease(enemyPileBlue1Sheet);
	esat::SpriteRelease(enemyPileBlue2Sheet);
	esat::SpriteRelease(enemyPileGreen1Sheet);
	esat::SpriteRelease(enemyPileGreen2Sheet);

	// -- Meteor enemy

	esat::SpriteHandle enemyMeteorBlueSheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_meteor_blue.png");
	esat::SpriteHandle enemyMeteorGreenSheet = esat::SpriteFromFile("./resources/sprites/enemies/enemy_meteor_green.png");

	for (int i = 0; i < 8; i++)
	{
		g_spr_meteor_blue[i] = esat::SubSprite(enemyMeteorBlueSheet, i*18,0,18,18);
		g_spr_meteor_green[i] = esat::SubSprite(enemyMeteorGreenSheet, i*18,0,18,15);
	}

	esat::SpriteRelease(enemyMeteorBlueSheet);
	esat::SpriteRelease(enemyMeteorGreenSheet);

}
/*Generates an array of TbackgroundStar with a random position
	and using a specified color palette. 
	Should be used only once unless new generation needed.*/
void generateBackground(){

	for (int i = 0; i < k_total_background_stars; i++)
	{
		//Star position
		float random_x_position = 5 + rand()%(g_trueWidth)-5;
		g_backgroundStars[i].x = random_x_position;
		float random_y_position = 5 + rand()%(g_trueHeight)-5;
		g_backgroundStars[i].y = random_y_position;

		//Visible
		g_backgroundStars[i].visible = true;

		//Star color random (5 color palettes) 
		int randomColor = rand()%7;
		switch (randomColor)
		{
		case 0:
				g_backgroundStars[i].red=240;
				g_backgroundStars[i].green=77;
				g_backgroundStars[i].blue=55;
			break;
		case 1:
				g_backgroundStars[i].red=124;
				g_backgroundStars[i].green=235;
				g_backgroundStars[i].blue=185;
			break;
		case 2:
				g_backgroundStars[i].red=224;
				g_backgroundStars[i].green=159;
				g_backgroundStars[i].blue=227;
			break;
		case 3:
				g_backgroundStars[i].red=234;
				g_backgroundStars[i].green=245;
				g_backgroundStars[i].blue=110;
			break;
		case 4:
				g_backgroundStars[i].red=110;
				g_backgroundStars[i].green=238;
				g_backgroundStars[i].blue=245;
			break;
		case 5:
				g_backgroundStars[i].red=245;
				g_backgroundStars[i].green=211;
				g_backgroundStars[i].blue=232;
			break;
		case 6:
				g_backgroundStars[i].red=47;
				g_backgroundStars[i].green=66;
				g_backgroundStars[i].blue=245;
			break;
		default:
				printf("\n->default star color generated (Check random limit!)");
				g_backgroundStars[i].red=240;
				g_backgroundStars[i].green=77;
				g_backgroundStars[i].blue=55;
			break;
		}
	}
	

}
//Should release every sprite in memory.
void releaseSprites(){

	//Font Sprites
	esat::SpriteRelease(g_spr_nichibitsu);

	//Player Sprites
	esat::SpriteRelease(g_player_spr[0]);
	esat::SpriteRelease(g_player_spr[1]);
	esat::SpriteRelease(g_player_spr[2]);
	esat::SpriteRelease(g_fuel[0]);
	esat::SpriteRelease(g_fuel[1]);
	esat::SpriteRelease(g_fuel[2]);
	//Enemy Sprites
	for (int i = 0; i < 15; i++)
	{
		esat::SpriteRelease(g_spr_eye_blue[i]);
		esat::SpriteRelease(g_spr_eye_green[i]);
		esat::SpriteRelease(g_spr_fly_blue[i]);
		esat::SpriteRelease(g_spr_fly_green[i]);
		esat::SpriteRelease(g_spr_ship_blue[i]);
		esat::SpriteRelease(g_spr_ship_green[i]);
	}
	for (int i = 0; i < 8; i++)
	{
		esat::SpriteRelease(g_spr_meteor_blue[i]);
		esat::SpriteRelease(g_spr_meteor_green[i]);
		esat::SpriteRelease(g_spr_meteor_explosion[i]);
	}
	for (int i = 0; i < 14; i++)
	{
		esat::SpriteRelease(g_spr_pile_blue[i]);
		esat::SpriteRelease(g_spr_pile_green[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		esat::SpriteRelease(g_spr_enemy_explosion[i]);
		esat::SpriteRelease(g_playerExplosion[i]);
	}
	
	
}
//Initialized the player data for player 1 and 2.
void initPlayerStruct(){
	for (int i = 0; i < 2; i++)
	{
		esat::SpriteTransformInit(&players[i].st);
		players[i].st.scale_x = k_scale;
		players[i].st.scale_y = k_scale;
		
		players[i].st.x = g_trueWidth/2;
		players[i].st.y = g_trueHeight*0.78f;

		players[i].st.sprite_origin_x = esat::SpriteWidth(g_player_spr[players[i].currentShip-1])/2;
	}
	dockingst.scale_x = k_scale;
	dockingst.scale_y = k_scale;
	dockingst.x = g_trueWidth/2;
}
// Initializes the enemies struct with the start values.
void initEnemiesStruct(){
	//Eyes
	for (int i = 0; i < 8; i++)
	{
		eyeEnemies[i].isAlive = true;
		eyeEnemies[i].isAggresive = false;
		eyeEnemies[i+8].isAlive = true;
		eyeEnemies[i+8].isAggresive = false;
		esat::SpriteTransformInit(&eyeEnemies[i].st);
		esat::SpriteTransformInit(&eyeEnemies[i+8].st);
		if (i<2)
		{
			eyeEnemies[i].start_pos.x = 76*k_scale;
			eyeEnemies[i].start_pos.y = 82*k_scale;
			eyeEnemies[i+8].start_pos.x = 76*k_scale;
			eyeEnemies[i+8].start_pos.y = 82*k_scale;
		}else if (i<4)
		{
			eyeEnemies[i].start_pos.x = 112*k_scale;
			eyeEnemies[i].start_pos.y = 66*k_scale;
			eyeEnemies[i+8].start_pos.x = 112*k_scale;
			eyeEnemies[i+8].start_pos.y = 66*k_scale;
		}else if (i<6)
		{
			eyeEnemies[i].start_pos.x = 112*k_scale;
			eyeEnemies[i].start_pos.y = 98*k_scale;
			eyeEnemies[i+8].start_pos.x = 112*k_scale;
			eyeEnemies[i+8].start_pos.y = 98*k_scale;
		}else{
			eyeEnemies[i].start_pos.x = 148*k_scale;
			eyeEnemies[i].start_pos.y = 82*k_scale;
			eyeEnemies[i+8].start_pos.x = 148*k_scale;
			eyeEnemies[i+8].start_pos.y = 82*k_scale;
		}

		eyeEnemies[i].st.scale_x = k_scale;
		eyeEnemies[i].st.scale_y = k_scale;
		eyeEnemies[i+8].st.scale_x = k_scale;
		eyeEnemies[i+8].st.scale_y = k_scale;
		
		eyeEnemies[i+8].color = Blue;
		eyeEnemies[i+8].st.sprite_origin_y = esat::SpriteHeight(g_spr_eye_blue[0])/2;
		eyeEnemies[i+8].st.sprite_origin_x = esat::SpriteWidth(g_spr_eye_blue[0])/2;
		
		eyeEnemies[i].color = Green;
		eyeEnemies[i].st.sprite_origin_y = esat::SpriteHeight(g_spr_eye_green[0])/2;
		eyeEnemies[i].st.sprite_origin_x = esat::SpriteWidth(g_spr_eye_green[0])/2;
	}
	//Fly
	for (int i = 0; i < 8; i++)
	{
		flyEnemies[i].isAlive = true;
		flyEnemies[i].isAggresive = false;
		flyEnemies[i+8].isAlive = true;
		flyEnemies[i+8].isAggresive = false;
		esat::SpriteTransformInit(&flyEnemies[i].st);
		esat::SpriteTransformInit(&flyEnemies[i+8].st);

		flyEnemies[i].st.scale_x = k_scale;
		flyEnemies[i].st.scale_y = k_scale;
		flyEnemies[i+8].st.scale_x = k_scale;
		flyEnemies[i+8].st.scale_y = k_scale;
	
		flyEnemies[i].color = Green;
		flyEnemies[i].st.sprite_origin_y = esat::SpriteHeight(g_spr_fly_green[0])/2;
		flyEnemies[i].st.sprite_origin_x = esat::SpriteWidth(g_spr_fly_green[0])/2;

		flyEnemies[i+8].color = Blue;
		flyEnemies[i+8].st.sprite_origin_y = esat::SpriteHeight(g_spr_fly_blue[0])/2;
		flyEnemies[i+8].st.sprite_origin_x = esat::SpriteWidth(g_spr_fly_blue[0])/2;
	}
	
	//Ship
	for (int i = 0; i < 8; i++)
	{
		shipEnemies[i].isAlive = true;
		shipEnemies[i].isAggresive = false;
		shipEnemies[i+8].isAlive = true;
		shipEnemies[i+8].isAggresive = false;
		esat::SpriteTransformInit(&shipEnemies[i].st);
		esat::SpriteTransformInit(&shipEnemies[i+8].st);

		shipEnemies[i].st.scale_x = k_scale;
		shipEnemies[i].st.scale_y = k_scale;
		shipEnemies[i+8].st.scale_x = k_scale;
		shipEnemies[i+8].st.scale_y = k_scale;
	
		shipEnemies[i].color = Green;
		shipEnemies[i].st.sprite_origin_y = esat::SpriteHeight(g_spr_ship_green[0])/2;
		shipEnemies[i].st.sprite_origin_x = esat::SpriteWidth(g_spr_ship_green[0])/2;

		shipEnemies[i+8].color = Blue;
		shipEnemies[i+8].st.sprite_origin_y = esat::SpriteHeight(g_spr_ship_green[0])/2;
		shipEnemies[i+8].st.sprite_origin_x = esat::SpriteWidth(g_spr_ship_green[0])/2;
	}

	for (int i = 0; i < 8; i++)
	{
		meteorEnemies[i].isAlive = true;
		meteorEnemies[i].isAggresive = false;
		meteorEnemies[i+8].isAlive = true;
		meteorEnemies[i+8].isAggresive = false;

		esat::SpriteTransformInit(&meteorEnemies[i].st);
		esat::SpriteTransformInit(&meteorEnemies[i+8].st);

		meteorEnemies[i].st.scale_x = k_scale;
		meteorEnemies[i].st.scale_y = k_scale;
		meteorEnemies[i+8].st.scale_x = k_scale;
		meteorEnemies[i+8].st.scale_y = k_scale;

		meteorEnemies[i].color = Green;
		meteorEnemies[i].st.sprite_origin_y = esat::SpriteHeight(g_spr_meteor_green[0])/2;
		meteorEnemies[i].st.sprite_origin_x = esat::SpriteWidth(g_spr_meteor_green[0])/2;

		meteorEnemies[i+8].color = Blue;
		meteorEnemies[i+8].st.sprite_origin_y = esat::SpriteHeight(g_spr_meteor_green[0])/2;
		meteorEnemies[i+8].st.sprite_origin_x = esat::SpriteWidth(g_spr_meteor_green[0])/2;
	}
	//Pile
	for (int i = 0; i < 8; i++)
	{
		pileEnemies[i].isAlive = true;
		pileEnemies[i].isAggresive = false;
		pileEnemies[i+8].isAlive = true;
		pileEnemies[i+8].isAggresive = false;

		esat::SpriteTransformInit(&pileEnemies[i].st);
		esat::SpriteTransformInit(&pileEnemies[i+8].st);

		pileEnemies[i].st.scale_x = k_scale;
		pileEnemies[i].st.scale_y = k_scale;
		pileEnemies[i+8].st.scale_x = k_scale;
		pileEnemies[i+8].st.scale_y = k_scale;

		pileEnemies[i].color = Green;
		pileEnemies[i].st.sprite_origin_y = esat::SpriteHeight(g_spr_pile_green[0])/2;
		pileEnemies[i].st.sprite_origin_x = esat::SpriteWidth(g_spr_pile_green[0])/2;

		pileEnemies[i+8].color = Blue;
		pileEnemies[i+8].st.sprite_origin_y = esat::SpriteHeight(g_spr_pile_green[0])/2;
		pileEnemies[i+8].st.sprite_origin_x = esat::SpriteWidth(g_spr_pile_green[0])/2;
	}


	printf("\ninit enemies complete.");
}
// Initializes the patterns used for the enemies.
void initPatterns(){
	//Eligible movements EYE
	patternsEye[0] = {-1, 6, 180, 	{ 0 , 30*k_scale}, true}; 						// Semicircle Down Right
	patternsEye[1] = {-1, -6, 180, { 0 , 30*k_scale}, true}; 						// Semicircle Down Left 
	patternsEye[2] = {-1, -5, 60,{ 30*k_scale , -30*k_scale}, false};		// Straight up Right
	patternsEye[3] = {-1, 5, 60, { -30*k_scale , -30*k_scale}, false}; 	// Straight up Left
	patternsEye[4] = {3, 6, 180, 	{ 0 , 30*k_scale}, true};							// Semicircle Down Right + Up
	patternsEye[5] = {2, -6, 180, 	{ 0 , 30*k_scale}, true};							// Semicircle Down Left + Up
	patternsEye[6] = {7, 6, 180, 	{ 0 , 30*k_scale}, true};							// Infinite Semicircle Down Right + Left 
	patternsEye[7] = {6, -6, 180, { 0 , 30*k_scale}, true};							// Infinite Semicircle Down Left + Right
	patternsEye[8] = {10, 6, 180, 	{ 0 , 30*k_scale}, true}; 						// Semicircle Down Right and continue right
	patternsEye[9] = {11,-6, 180, { 0 , 30*k_scale}, true}; 							// Semicircle Down Left and continue left
	patternsEye[10] = {-1, 0.09, 1,{ 45*k_scale , 0}, false}; 		// Right
	patternsEye[11] = {-1, 0.09, 1,{ -45*k_scale , 0}, false}; 	// Left
	patternsEye[12] = {-1, 6, 300, 	{ 0 , 30*k_scale}, true}; 					// Full(almost) circle down right
	patternsEye[13] = {-1, -6, 300, 	{ 0 , 30*k_scale}, true}; 					// Full(almost)  circle down left
	patternsEye[14] = {-1, 6, 300, 	{ 0 , -30*k_scale}, true}; 					// Full(almost)  circle up right
	patternsEye[15] = {-1, -6, 300, 	{ 0 , -30*k_scale}, true};					// Full(almost)  circle up left
	patternsEye[16] = {-1, 6, 180, 	{ 0 , -30*k_scale}, true}; 					// Semicircle up Right
	patternsEye[17] = {-1, -6, 180, { 0 , -30*k_scale}, true}; 					// Semicircle up Left 

	//Eligible movements FLY
	patternsFly[0] = {-1, 7, 90, {20 * k_scale, 50*k_scale}, false}; 	//Start to right
	patternsFly[1] = {-1, 7, 90, {-20 * k_scale, 50*k_scale}, false};	//Start to left
	patternsFly[2] = {-1, 7, 180, {0, 30 * k_scale},true}; // Semicircle Down Right
	patternsFly[3] = {-1, -7, 180, {0, 30 * k_scale},true}; // Semicircle Down Left
	patternsFly[4] = {-1, 0.12, 1, {35*k_scale, 0}, false}; // Right
	patternsFly[5] = {-1, 0.12, 1, {-35*k_scale, 0}, false}; // Left
	patternsFly[6] = {8, 0.05, 1, {0, 130*k_scale}, true}; // Down to left + up
	patternsFly[7] = {9, 0.05, 1, {0, 130*k_scale}, true}; // Down to Right + up
	patternsFly[8] = {10, -7, 180, {-35*k_scale, 0}, false}; // Semicircle Left + up
	patternsFly[9] = {10, 7, 180, {35*k_scale, 0}, false}; // Semicircle Right + up
	patternsFly[10] = {-1, 0.05, 1, {0, -130*k_scale}, false}; // Up
	patternsFly[11] = {14, 7, 180, {0, 30 * k_scale},true}; // Semicircle Down Right + Left into Infinite
	patternsFly[12] = {11, 0.12, 1, {-35*k_scale, 0}, false}; // Left + Semicircle Down Left into Infinite
	patternsFly[13] = {12, -7, 180, {0, 30 * k_scale},true}; // Semicircle Down Left + Right into Infinite
	patternsFly[14] = {13, 0.12, 1, {35*k_scale, 0}, false}; // Right + Semicircle Down Right into Infinite
	patternsFly[15] = {4, 7, 180, {0, 30 * k_scale},true}; // Semicircle Down Right into left
	patternsFly[16] = {5,-7, 180, {0, 30 * k_scale},true}; // Semicircle Down Left into right

	//Eligible movements Ship
	patternsShip[0] = {-1, -8, 200, {25 * k_scale, 1*k_scale}, false}; 	// Hard Right 
	patternsShip[1] = {-1, 8, 200, {-25 * k_scale, 1*k_scale}, false}; 	// Hard Left 
	patternsShip[2] = {-1, -6, 360, {0, 50 * k_scale}, true}; 	// Circle Right
	patternsShip[3] = {-1, 6, 360, {0, 50 * k_scale}, true}; 	// Circle Left
	patternsShip[4] = {6, 0.05, 1, {0, 130*k_scale}, true}; // Down to left circle
	patternsShip[5] = {7, 0.05, 1, {0, 130*k_scale}, true}; // Down to Right circle
	patternsShip[6] = {-1, -6, 270, {-50*k_scale, 0}, false}; 	// Circle Rightl
	patternsShip[7] = {-1, 6, 270, {50*k_scale, 0}, false}; 	// Circle Right
	patternsShip[8] = {11, 7, 180, {0, 30 * k_scale},true}; // Semicircle Down Right + Left into Infinite
	patternsShip[9] = {8, 0.12, 1, {-35*k_scale, 0}, false}; // Left + Semicircle Down Left into Infinite
	patternsShip[10] = {9, -7, 180, {0, 30 * k_scale},true}; // Semicircle Down Left + Right into Infinite
	patternsShip[11] = {10, 0.12, 1, {35*k_scale, 0}, false}; // Right + Semicircle Down Right into Infinite
	patternsShip[12] = {13, -2, 17, {0, 160*k_scale}, true}; 	// Special attack start
	patternsShip[13] = {14, 0.05, 1, {-100*k_scale, 90*k_scale}, false}; // Special attack invisible
	patternsShip[14] = {15, 10, 90, {35*k_scale, 0}, false}; 	// Special attack circle
	patternsShip[15] = {16, 0.18, 1, {30*k_scale, 0}, false}; // Special attack Right
	patternsShip[16] = {17, -14, 75, {0, 22*k_scale}, false}; //  Right small circle 
	patternsShip[17] = {14, 0.1, 1, {25*k_scale, 40*k_scale}, false}; // Special invisible

	//Eligible movements Pile
	patternsPile[0] = {1, 8, 170, {15 * k_scale, 0}, true}; 	// Hard Right 
	patternsPile[1] = {0, -8, 170, {-15 * k_scale, 0}, true}; 	// Hard Left 
	patternsPile[2] = {-1, 0.01, 1, {0, 300*k_scale}, true};    // Down
}

// UTILS

//	Paints a red outline of the Thitbox. Used for debugging.
void painThitbox(Thitbox box){
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
/*	Checks if there is a colison.
		Receives 2 params Thitbox. Returns bool.
		If they collide returns true.
		If NOT false.
*/
bool checkHitBox(Thitbox c1, Thitbox c2){
	bool res = false;
	//Checks min max position to see if they colide.
	if (c1.minx < c2.maxx && c1.maxx > c2.minx && c1.miny < c2.maxy && c1.maxy > c2.miny)
	{
		res = true;
	}
	//painThitbox(c1);
	//painThitbox(c2);
	return res;
	
}
/*	Returns a Thitbox
		using as params a sprite, 
		the position(x,y) of the sprite and a expander.
		The expander expands the Thitbox by a number 
		of pixes on every side.
		Usefull if a bigger colider is needed.
*/
Thitbox getHitboxFromSprite(esat::SpriteHandle sprite, int x, int y, int expander, bool centered){
	Thitbox res;
	if (centered)
	{
		res.minx = (x - (esat::SpriteWidth(sprite)/3)*k_scale);
		res.maxx = (x + (esat::SpriteWidth(sprite)/3)*k_scale);
		res.miny = y - (expander*k_scale);
		res.maxy = y + (esat::SpriteHeight(sprite)*k_scale)/2;
	}else{
		res.minx = x-(expander*k_scale);
		res.maxx = esat::SpriteWidth(sprite) + x + (expander*k_scale);
		res.miny = y - (expander*k_scale);
		res.maxy = esat::SpriteHeight(sprite) + y + (expander*k_scale);
	}
	return res;
}  
/*Returns in the char array the number in char format.
The result must be always in a size 7 array. 
*/ 
void getStringFromInt(int number, char result[]) {
    
    for (int i = 0; i < 6; ++i) {
        result[i] = '0';
    }
    int counter = 5;
    
    while (number > 0 && counter >= 0) {
        result[counter] = (char)((number % 10) + '0');
        number /= 10;
        --counter;
    }
		// Last character is \0 to finish the text.
		result[6] = '\0';
}
// Rotation movement based on a center position. If changeRotation is true also changes the angle of the sprite accordingly.
void rotate(esat::SpriteTransform *st, float cx, float cy, float dangulo, bool changeRotation){
  //Rotation
	float dx = st->x - cx, dy = st->y - cy, r = sqrt(dx*dx + dy*dy), a = atan2(dy, dx);
	// If changeRotation TRUE
	if (changeRotation)
	{
		// Rotation of sprite based on position rotation.
		if (dangulo < 0)
		{
			st->angle = a;
			if (level>1)
			{
				dangulo -= level*0.25;
			}
		}else{
			st->angle = a + k_PI;
			if (level>1)
			{
				dangulo += level*0.25;
			}
		}
	}
  a -= dangulo / 180 * k_PI;
  //New position.
	st->x = cx + r*cos(a);
  st->y = cy + r*sin(a);
}

// -- UI --

// UI used in menu and scoreboard level.
void menuUi(){
	
	esat::DrawSetFillColor(78,252,241);
  esat::DrawSetTextSize(40);
  esat::DrawText(10*k_scale,11*k_scale,"1 ST");
	esat::DrawText(85*k_scale,11*k_scale,"HI SCORE");
	esat::DrawText(190*k_scale,11*k_scale,"2 ND");
	esat::DrawSetFillColor(76,76,176);
	esat::DrawText(3*k_scale,255*k_scale,"CREDIT");
	
	//Credit
	outputArray[7];
	getStringFromInt(g_credits, outputArray);
	for (int i = 0; i < 4; i++)
	{
		outputArray[i] = ' ';
	}
	
	esat::DrawText(45*k_scale,255*k_scale,outputArray);


	esat::DrawSetFillColor(250,250,250);
	//Score
	outputArray[7];
	getStringFromInt(players[0].score, outputArray);
	esat::DrawText(3*k_scale,20*k_scale,outputArray);
	getStringFromInt(players[1].score, outputArray);
	esat::DrawText(176*k_scale,20*k_scale,outputArray);
	//Hi Score
	
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(90*k_scale,20*k_scale,outputArray);
	
	// Adds credits 
	if (esat::IsKeyUp('C'))
	{
		g_credits++;
	}
}
// UI used in-game.
void gameUi(){
	
	// TEXT 
	esat::DrawSetFillColor(78,252,241); 
  esat::DrawSetTextSize(40);

  esat::DrawText(10*k_scale,11*k_scale,"1 ST");
	esat::DrawText(85*k_scale,11*k_scale,"HI SCORE");
	if (twoPlayers)
	{
		esat::DrawText(190*k_scale,11*k_scale,"2 ND");
	}
	
	
	esat::DrawSetFillColor(76,76,176);//Color RGB texto a mostrar
	esat::DrawText(3*k_scale,255*k_scale,"CREDIT");
	
	//Credit
	getStringFromInt(g_credits, outputArray);
	//Replaces the first 4 character for spaces.
	for (int i = 0; i < 4; i++)
	{
		outputArray[i] = ' ';
	}
	
	esat::DrawText(45*k_scale,255*k_scale,outputArray);


	esat::DrawSetFillColor(250,250,250);
	//Score
	getStringFromInt(players[0].score, outputArray);
	esat::DrawText(3*k_scale,20*k_scale,outputArray);
	if (twoPlayers)
	{
		getStringFromInt(players[1].score, outputArray);
		esat::DrawText(176*k_scale,20*k_scale,outputArray);
	}
	
	
	//Hi Score
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(90*k_scale,20*k_scale,outputArray);
	
	// Nichibitsu japanese text
	st_fonts.sprite_origin_x =  esat::SpriteWidth(g_spr_nichibitsu)/2;
	st_fonts.x = g_trueWidth/2;
	st_fonts.y = 20*k_scale;
	esat::DrawSprite(g_spr_nichibitsu,st_fonts);

}

// -- GAME --

// Reset Enemy data for each level.
void resetStartEnemyPositions(){
	g_aliveOnLevelTime = esat::Time();
	dockingst.y = g_trueHeight+ (esat::SpriteHeight(g_player_spr[2])*4);
	dockingst.sprite_origin_x = esat::SpriteWidth(g_player_spr[players[g_playerid].currentShip-1])/2;

	g_menutext_1_x = g_trueWidth*1.2;
	g_menutext_2_x = g_trueWidth*2;
	g_menutext_3_x = g_trueWidth*2.7;

	for (int i = 0; i < 8; i++)
	{	
		// -- EYES --
		eyeEnemies[i].isAlive = true;
		eyeEnemies[i].isMoving = false;
		eyeEnemies[i].isAggresive = false;
		eyeEnemies[i].st.angle = 0;
		eyeEnemies[i].rotations = 0;
		eyeEnemies[i].joined = true;
		eyeEnemies[i].isExploding = false;
		eyeEnemies[i].explodingCounter = 0;
		eyeEnemies[i+8].isAlive = true;
		eyeEnemies[i+8].isMoving = false;
		eyeEnemies[i+8].isAggresive = false;
		eyeEnemies[i+8].st.angle = 0;
		eyeEnemies[i+8].rotations = 0;
		eyeEnemies[i+8].joined = true;
		eyeEnemies[i+8].isExploding = false;
		eyeEnemies[i+8].explodingCounter = 0;

		if(i<2){
			eyeEnemies[i].st.x = g_trueWidth*0.4;
			eyeEnemies[i].st.y = g_trueHeight*0.3;
			eyeEnemies[i+8].st.x = g_trueWidth*0.4;
			eyeEnemies[i+8].st.y = g_trueHeight*0.3;
		}else if(i<4){
			eyeEnemies[i].st.x = g_trueWidth*0.5;
			eyeEnemies[i].st.y = g_trueHeight*0.25;
			eyeEnemies[i+8].st.x = g_trueWidth*0.5;
			eyeEnemies[i+8].st.y = g_trueHeight*0.25;
		}else if(i<6){
			eyeEnemies[i].st.x = g_trueWidth*0.5;
			eyeEnemies[i].st.y = g_trueHeight*0.35;
			eyeEnemies[i+8].st.x = g_trueWidth*0.5;
			eyeEnemies[i+8].st.y = g_trueHeight*0.35;
		}else{
			eyeEnemies[i].st.x = g_trueWidth*0.6;
			eyeEnemies[i].st.y = g_trueHeight*0.3;
			eyeEnemies[i+8].st.x = g_trueWidth*0.6;
			eyeEnemies[i+8].st.y = g_trueHeight*0.3;
		}

		// -- FLIES --
		flyEnemies[i].start_pos.x = g_trueWidth*0.3 + rand()%g_trueWidth*0.4;
		flyEnemies[i].start_pos.y = g_trueHeight*0.5;
		flyEnemies[i+8].start_pos.x = g_trueWidth*0.3 + rand()%g_trueWidth*0.4;
		flyEnemies[i+8].start_pos.y = g_trueHeight*0.5;
		
		flyEnemies[i].isMoving = false;
		flyEnemies[i].isAlive = true;
		flyEnemies[i].isAggresive = false;
		flyEnemies[i].st.angle = 0;
		flyEnemies[i].rotations = 0;
		flyEnemies[i].isExploding = false;
		flyEnemies[i].explodingCounter = 0;

		flyEnemies[i+8].isMoving = true;
		flyEnemies[i+8].isAlive = true;
		flyEnemies[i+8].isAggresive = false;
		flyEnemies[i+8].st.angle = 0;
		flyEnemies[i+8].rotations = 0;
		flyEnemies[i+8].isExploding = false;
		flyEnemies[i+8].explodingCounter = 0;

		flyEnemies[i].pattern = rand()%2;
		flyEnemies[i+8].pattern = rand()%2;
		flyEnemies[i].ref_rotation.x = rand()%20 * k_scale;
		flyEnemies[i+8].ref_rotation.x = rand()%20 * k_scale;
		flyEnemies[i].ref_rotation.y = rand()%60 * k_scale;
		flyEnemies[i+8].ref_rotation.y = rand()%60 * k_scale;

		// -- SHIPS --
		//shipEnemies[i].start_pos.x = g_trueWidth*0.5;
		//shipEnemies[i].start_pos.y = g_trueHeight*0.7;
		//shipEnemies[i+8].start_pos.x = g_trueWidth*0.5;
		//shipEnemies[i+8].start_pos.y = g_trueHeight*0.7;

		shipEnemies[i].isMoving = false;
		shipEnemies[i].isAlive = true;
		shipEnemies[i].isAggresive = false;
		shipEnemies[i].st.angle = 0;
		shipEnemies[i].rotations = 0;
		shipEnemies[i].isExploding = false;
		shipEnemies[i].explodingCounter = 0;

		shipEnemies[i+8].isMoving = true;
		shipEnemies[i+8].isAlive = true;
		shipEnemies[i+8].isAggresive = false;
		shipEnemies[i+8].st.angle = 0;
		shipEnemies[i+8].rotations = 0;
		shipEnemies[i+8].isExploding = false;
		shipEnemies[i+8].explodingCounter = 0;
		//Left Side
		if (i<4)
		{
			
			if (i<2)
			{
				//Long
				shipEnemies[i].start_pos.x = g_trueWidth*0.15;
				shipEnemies[i].start_pos.y = g_trueHeight*0.5;
				shipEnemies[i].ref_rotation.x = g_trueWidth*0.28;
				shipEnemies[i].ref_rotation.y = g_trueHeight*0.12;

				shipEnemies[i+8].start_pos.x = g_trueWidth*0.15;
				shipEnemies[i+8].start_pos.y = g_trueHeight*0.5;
				shipEnemies[i+8].ref_rotation.x = g_trueWidth*0.28;
				shipEnemies[i+8].ref_rotation.y = g_trueHeight*0.12;
			}else{
				//Short
				shipEnemies[i].start_pos.x = g_trueWidth*0.38;
				shipEnemies[i].start_pos.y = g_trueHeight*0.7;
				shipEnemies[i].ref_rotation.x = g_trueWidth*0.42;
				shipEnemies[i].ref_rotation.y = g_trueHeight*0.4;

				shipEnemies[i+8].start_pos.x = g_trueWidth*0.38;
				shipEnemies[i+8].start_pos.y = g_trueHeight*0.7;
				shipEnemies[i+8].ref_rotation.x = g_trueWidth*0.42;
				shipEnemies[i+8].ref_rotation.y = g_trueHeight*0.4;
			}
			
			
		}else{
			if (i<6)
			{
				//Long
				shipEnemies[i].start_pos.x = g_trueWidth*0.85;
				shipEnemies[i].start_pos.y = g_trueHeight*0.5;
				shipEnemies[i].ref_rotation.x = g_trueWidth*0.72;
				shipEnemies[i].ref_rotation.y = g_trueHeight*0.12;

				shipEnemies[i+8].start_pos.x = g_trueWidth*0.85;
				shipEnemies[i+8].start_pos.y = g_trueHeight*0.5;
				shipEnemies[i+8].ref_rotation.x = g_trueWidth*0.72;
				shipEnemies[i+8].ref_rotation.y = g_trueHeight*0.12;
			}else{
				//Short
				shipEnemies[i].start_pos.x = g_trueWidth*0.62;
				shipEnemies[i].start_pos.y = g_trueHeight*0.7;
				shipEnemies[i].ref_rotation.x = g_trueWidth*0.58;
				shipEnemies[i].ref_rotation.y = g_trueHeight*0.4;

				shipEnemies[i+8].start_pos.x = g_trueWidth*0.62;
				shipEnemies[i+8].start_pos.y = g_trueHeight*0.7;
				shipEnemies[i+8].ref_rotation.x = g_trueWidth*0.58;
				shipEnemies[i+8].ref_rotation.y = g_trueHeight*0.4;
			}
			
			
		}
		if (rand()%2 == 0)
			{
				shipEnemies[i].start_pos.x += rand()%12*k_scale;
				shipEnemies[i].start_pos.y += rand()%8*k_scale;
				shipEnemies[i].ref_rotation.x += rand()%18*k_scale;
				shipEnemies[i].ref_rotation.y += rand()%20*k_scale;

				shipEnemies[i+8].start_pos.x += rand()%12*k_scale;
				shipEnemies[i+8].start_pos.y += rand()%8*k_scale;
				shipEnemies[i+8].ref_rotation.x += rand()%18*k_scale;
				shipEnemies[i+8].ref_rotation.y += rand()%20*k_scale;

				
			}else{
				shipEnemies[i].start_pos.x -= rand()%12*k_scale;
				shipEnemies[i].start_pos.y -= rand()%8*k_scale;
				shipEnemies[i].ref_rotation.x -= rand()%18*k_scale;
				shipEnemies[i].ref_rotation.y -= rand()%20*k_scale;

				shipEnemies[i+8].start_pos.x -= rand()%12*k_scale;
				shipEnemies[i+8].start_pos.y -= rand()%8*k_scale;
				shipEnemies[i+8].ref_rotation.x -= rand()%18*k_scale;
				shipEnemies[i+8].ref_rotation.y -= rand()%20*k_scale;
			}
		shipEnemies[i].pattern = rand()%2;

		// -- METEORS --
		meteorEnemies[i].isMoving = false;
		meteorEnemies[i].isAlive = true;
		meteorEnemies[i].isAggresive = false;
		meteorEnemies[i].st.angle = 0;
		meteorEnemies[i].rotations = 0;
		meteorEnemies[i].isExploding = false;
		meteorEnemies[i].explodingCounter = 0;

		if (i%2 == 0)
		{
			meteorEnemies[i].start_pos.x = g_trueWidth*0.8 + rand()%g_trueWidth*0.1;
			meteorEnemies[i].start_pos.y = g_trueHeight*0.15 + rand()%g_trueHeight*0.1; 

			meteorEnemies[i].ref_rotation.x = -g_trueWidth*0.1 + rand()%g_trueWidth*0.15;
			meteorEnemies[i].ref_rotation.y = g_trueHeight*1.2;

			meteorEnemies[i+8].start_pos.x = g_trueWidth*0.8 + rand()%g_trueWidth*0.1;
			meteorEnemies[i+8].start_pos.y = g_trueHeight*0.15 + rand()%g_trueHeight*0.1; 

			meteorEnemies[i+8].ref_rotation.x = -g_trueWidth*0.1 + rand()%g_trueWidth*0.15;
			meteorEnemies[i+8].ref_rotation.y = g_trueHeight*1.2;

		}else{

			meteorEnemies[i].start_pos.x = g_trueWidth*0.1 + rand()%g_trueWidth*0.2;
			if (rand()%2 == 0)
			{
				meteorEnemies[i].start_pos.y = meteorEnemies[i-1].start_pos.y + 15*k_scale + rand()%g_trueHeight*0.1; 
			}else{
				meteorEnemies[i].start_pos.y = meteorEnemies[i-1].start_pos.y - 15*k_scale - rand()%g_trueHeight*0.1; 
			}
			meteorEnemies[i].ref_rotation.x = g_trueWidth*0.95 + rand()%g_trueWidth*0.1;
			meteorEnemies[i].ref_rotation.y = g_trueHeight*1.2 + rand()%g_trueHeight*0.1;

			
			meteorEnemies[i+8].start_pos.x = g_trueWidth*0.1 + rand()%g_trueWidth*0.2;
			if (rand()%2 == 0)
			{
				meteorEnemies[i+8].start_pos.y = meteorEnemies[i-1+8].start_pos.y + 15*k_scale + rand()%g_trueHeight*0.1; 
			}else{
				meteorEnemies[i+8].start_pos.y = meteorEnemies[i-1+8].start_pos.y - 15*k_scale - rand()%g_trueHeight*0.1; 
			}
			meteorEnemies[i+8].ref_rotation.x = g_trueWidth*0.95 + rand()%g_trueWidth*0.1;
			meteorEnemies[i+8].ref_rotation.y = g_trueHeight*1.2 + rand()%g_trueHeight*0.1;
		}
		if (i == 1)
		{
			meteorEnemies[i].isAggresive = true;
			meteorEnemies[i-1].isAggresive = true;
			meteorEnemies[i+8].isAggresive = true;
			meteorEnemies[i+7].isAggresive = true;
		}
		
		// -- PILE --
		pileEnemies[i].isAlive = true;
		pileEnemies[i].isMoving = true;
		pileEnemies[i].isAggresive = false;
		pileEnemies[i].st.angle = 0;
		pileEnemies[i].rotations = 0;
		pileEnemies[i].joined = true;
		pileEnemies[i].isExploding = false;
		pileEnemies[i].explodingCounter = 0;

		pileEnemies[i].st.x = g_trueWidth * 0.1 + (g_trueWidth*0.1*i);
		pileEnemies[i].st.y = -50*k_scale;
		
		pileEnemies[i+8].isAlive = true;
		pileEnemies[i+8].isMoving = true;
		pileEnemies[i+8].isAggresive = false;
		pileEnemies[i+8].st.angle = 0;
		pileEnemies[i+8].rotations = 0;
		pileEnemies[i+8].joined = true;
		pileEnemies[i+8].isExploding = false;
		pileEnemies[i+8].explodingCounter = 0;

		pileEnemies[i+8].st.x = g_trueWidth * 0.1 + (g_trueWidth*0.1*i);
		pileEnemies[i+8].st.y = -50*k_scale;

		if (i%2==0)
		{
			pileEnemies[i].st.y += 30*k_scale;
			pileEnemies[i+8].st.y += 30*k_scale;
		}
		pileEnemies[i].start_pos.x = pileEnemies[i].st.x;
		pileEnemies[i].start_pos.y = pileEnemies[i].st.y;
		pileEnemies[i+8].start_pos.x = pileEnemies[i+8].st.x;
		pileEnemies[i+8].start_pos.y = pileEnemies[i+8].st.y;
	}
	printf("\nReset Enemies complete.");
}
// Reset Players values
void resetPlayer(){
	for (int i = 0; i < 2; i++)
	{
		players[i].ship1_isActive = true;
		players[i].ship2_isActive = false;
		players[i].ship3_isActive = false;
		players[i].ship1_isAlive = true;
		players[i].ship2_isAlive = true;
		players[i].ship3_isAlive = true;
		players[i].currentShip = 1;
	}
}
// Paints every star in background and moves it across the screen.
void paintBackgroundStars(){
	float squareArea = 1.0f * k_scale; 
	for (int i = 0; i < k_total_background_stars; i++)
	{
		// If star is below the screen moves it up.
		if (g_backgroundStars[i].y>g_trueHeight)
		{
			float random_y_position = rand()%3;
			g_backgroundStars[i].y = -1.0f-random_y_position;
			float random_x_position = 5 + rand()%(g_trueWidth)-5;
			g_backgroundStars[i].x = random_x_position;
		}
		//Movement down using window scale for speed reference
		g_backgroundStars[i].y += 1.5*k_scale;

		//Array for painting 
		float starSquare[10] = {
			g_backgroundStars[i].x, g_backgroundStars[i].y,
			g_backgroundStars[i].x+squareArea, g_backgroundStars[i].y,
			g_backgroundStars[i].x+squareArea, g_backgroundStars[i].y+squareArea,
			g_backgroundStars[i].x, g_backgroundStars[i].y+squareArea,
			g_backgroundStars[i].x, g_backgroundStars[i].y
		};
		int random = rand()%8;
		//If visible paint star
		if (g_backgroundStars[i].visible)
		{
			//Draw visible star
			esat::DrawSetFillColor(g_backgroundStars[i].red,
													g_backgroundStars[i].green,
													g_backgroundStars[i].blue);
			esat::DrawSetStrokeColor(g_backgroundStars[i].red,
													g_backgroundStars[i].green,
													g_backgroundStars[i].blue, 130);
			esat::DrawSolidPath(starSquare, 5);

			//Randomly can go invisible.
			if (random == 0 && g_backgroundStars[i].y<g_trueHeight-40 && g_backgroundStars[i].y > 30)
			{
				g_backgroundStars[i].visible = false;
				//Invisible cooldown
				g_backgroundStars[i].cooldown = 6;
			}
		}else{
			//Not visible star 
			//If cooldown 0 might go visible 50/50 chance
			if (g_backgroundStars[i].cooldown<=0 && random%2==0)
			{
				g_backgroundStars[i].visible = true;
				//Semi transparent version of star
				esat::DrawSetFillColor(g_backgroundStars[i].red,
													g_backgroundStars[i].green,
													g_backgroundStars[i].blue, 
													69);
				esat::DrawSetStrokeColor(g_backgroundStars[i].red,
													g_backgroundStars[i].green,
													g_backgroundStars[i].blue,0);
				esat::DrawSolidPath(starSquare, 5);
			}
			
			if (g_backgroundStars[i].cooldown>=4)
			{
				//Semi transparent version of star
				esat::DrawSetFillColor(g_backgroundStars[i].red,
													g_backgroundStars[i].green,
													g_backgroundStars[i].blue, 
													69);
				esat::DrawSetStrokeColor(g_backgroundStars[i].red,
													g_backgroundStars[i].green,
													g_backgroundStars[i].blue,0);
				esat::DrawSolidPath(starSquare, 5);
			}
			//Star invisble cooldown
			g_backgroundStars[i].cooldown--;
		}
	}
}
// Paints player and player bullet. 
void paintPlayer(bool paintFuel){
	// Temp st based on the player.
	esat::SpriteTransform temp = players[g_playerid].st;
	// If player exploding paint the explosion
	if (players[g_playerid].isExploding)
	{
		esat::DrawSprite(g_playerExplosion[playerSpriteExplosion/4], temp);
		playerSpriteExplosion++;
		if (playerSpriteExplosion >= 16)
		{
			players[g_playerid].isExploding = false;
			playerSpriteExplosion = 0;
		}
		
	}else{ // If player not exploding
		// If ship1 active and alive paint it and add the height to temp.
		if (players[g_playerid].ship1_isActive && players[g_playerid].ship1_isAlive)
		{
			esat::DrawSprite(g_player_spr[0], temp);
			temp.y += esat::SpriteHeight(g_player_spr[0])*k_scale;
			players[g_playerid].currentShip = 1;
		}
		// If ship2 active and alive change the sprite origin, paint the ship and add the height
		if (players[g_playerid].ship2_isActive && players[g_playerid].ship2_isAlive)
		{
			temp.sprite_origin_x = esat::SpriteWidth(g_player_spr[1])/2;
			esat::DrawSprite(g_player_spr[1], temp);	
			temp.y += esat::SpriteHeight(g_player_spr[1])*k_scale;
			players[g_playerid].currentShip = 2;
		}
		//If ship3 active and alive change the sprite origin, paint the ship and add the height
		if (players[g_playerid].ship3_isActive && players[g_playerid].ship3_isAlive)
		{
			temp.sprite_origin_x = esat::SpriteWidth(g_player_spr[2])/2;
			esat::DrawSprite(g_player_spr[2], temp);
			temp.y += esat::SpriteHeight(g_player_spr[2])*k_scale;
			players[g_playerid].currentShip = 3;
		}
	}
	
	//If any ship active
	if (players[g_playerid].ship1_isActive || players[g_playerid].ship2_isActive || players[g_playerid].ship3_isActive)
	{	
		//And paintfuel is true
		if (paintFuel)
		{
			//Paint fuel with animation

			temp.sprite_origin_x = esat::SpriteWidth(g_fuel[g_fuel_counter])/2;
			temp.x += k_scale;
			esat::DrawSprite(g_fuel[g_fuel_counter], temp);

			// Fuel sprite control with cooldown
			if(g_fuel_cooldown >= 2){
				if(g_fuel_counter >1){
					g_fuel_counter = 0;
				}
			g_fuel_counter++;
			g_fuel_cooldown = 0;
		}
			g_fuel_cooldown++;
		}
	}
	// Paint bullets if they are active. Also move them
	for (int i = 0; i < 5; i++)
	{
		if (players[g_playerid].bullets[i].isActive)
		{
			
			float bulletEdges[10] = {
				players[g_playerid].bullets[i].x, players[g_playerid].bullets[i].y,
				g_bulletWidth+players[g_playerid].bullets[i].x, players[g_playerid].bullets[i].y,
				g_bulletWidth+players[g_playerid].bullets[i].x, g_bulletHeight+players[g_playerid].bullets[i].y,
				players[g_playerid].bullets[i].x, g_bulletHeight+players[g_playerid].bullets[i].y,
				players[g_playerid].bullets[i].x, players[g_playerid].bullets[i].y
			};

		esat::DrawSetFillColor(255,255,255);
		esat::DrawSolidPath(bulletEdges,5);
		//Bullet movement speed = 7
		players[g_playerid].bullets[i].y -= 7*k_scale;
		}
	}
}
//Checks colisions on walls player and bullets
void wallColision(){
	//Width current ship sprite /2
	int halfPlayerSpr = esat::SpriteWidth(g_player_spr[players[g_playerid].currentShip-1])/2 * k_scale;	
	
	//Checks if x position is outside of window and moves it back.
	if (players[g_playerid].st.x + halfPlayerSpr > g_trueWidth)
	{
		players[g_playerid].st.x = g_trueWidth - halfPlayerSpr;
	}
	if(players[g_playerid].st.x - halfPlayerSpr < 0)
	{
		players[g_playerid].st.x = halfPlayerSpr;
	}
	
	//Resets bullets when out of screen.
	for (int i = 0; i < 5; i++)
	{
		if (players[g_playerid].bullets[i].y < -g_bulletHeight)
		{
			players[g_playerid].bullets[i].isActive = false;
		}
	}
}
//Shoots bullet depending on ships and bullets state.
void shoot(){
	
	float temp = 0.0f;
	if (!players[g_playerid].bullets[0].isActive && players[g_playerid].ship1_isActive)
	{
		players[g_playerid].bullets[0].isActive = true;
		players[g_playerid].bullets[0].x = players[g_playerid].st.x;
		players[g_playerid].bullets[0].y = players[g_playerid].st.y - 4*k_scale;	
	}
	else if(!players[g_playerid].bullets[1].isActive && !players[g_playerid].bullets[2].isActive
		&& players[g_playerid].ship2_isActive){
		
		if (players[g_playerid].ship1_isActive) {temp += esat::SpriteHeight(g_player_spr[0])*k_scale;}

		players[g_playerid].bullets[1].isActive = true;
		players[g_playerid].bullets[2].isActive = true;

		players[g_playerid].bullets[1].x = players[g_playerid].st.x - 8*k_scale;
		players[g_playerid].bullets[1].y = players[g_playerid].st.y + temp;

		players[g_playerid].bullets[2].x = players[g_playerid].st.x + 8*k_scale;
		players[g_playerid].bullets[2].y = players[g_playerid].st.y + temp;
		//temp += esat::SpriteHeight(g_player[1])*k_scale;
	}
	else if(!players[g_playerid].bullets[3].isActive && !players[g_playerid].bullets[4].isActive
		&& players[g_playerid].ship3_isActive){

		if (players[g_playerid].ship1_isActive) {temp += esat::SpriteHeight(g_player_spr[0])*k_scale;}
		if (players[g_playerid].ship2_isActive) {temp += esat::SpriteHeight(g_player_spr[1])*k_scale;}

		players[g_playerid].bullets[3].isActive = true;
		players[g_playerid].bullets[4].isActive = true;

		players[g_playerid].bullets[3].x = players[g_playerid].st.x - 12*k_scale;
		players[g_playerid].bullets[3].y = players[g_playerid].st.y + temp;

		players[g_playerid].bullets[4].x = players[g_playerid].st.x + 12*k_scale;
		players[g_playerid].bullets[4].y = players[g_playerid].st.y + temp;
		}
	
}
// INPUT CONTROL
void gameInput(){
	// Moves player to the left
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left))
	{
		players[g_playerid].st.x -= g_playerSpeed;
	}
	// Moves player to the right
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right))
	{
		players[g_playerid].st.x += g_playerSpeed;
	}
	// Shoots bullet
	if (esat::IsSpecialKeyUp(esat::kSpecialKey_Space))
	{
		shoot();
	}
}
// Common on every game-level functions
void basicGameLoop(){
	gameInput();
	wallColision();
	paintPlayer(true);
}
// On colision checks what player ship to destroy and if is game over.
void onColision(){
	// If Ship3 active game over
	if (players[g_playerid].ship3_isActive)
	{
		printf("GAMEOVER");
		g_aliveOnLevelTime = esat::Time();
		g_gameManager = GameOver;
	}
	// If Ship2 active unalive it and active 3rd one.
	if (players[g_playerid].ship2_isActive)
	{
		players[g_playerid].ship2_isActive =  false;
		players[g_playerid].ship2_isAlive =  false;
		if (players[g_playerid].ship3_isAlive)
		{
			players[g_playerid].ship3_isActive = true;
		}
	}
	//If ship1 active unalive it
	if (players[g_playerid].ship1_isActive){
		players[g_playerid].ship1_isActive = false;
		players[g_playerid].ship1_isAlive = false;
		//If ship2 not active activate it
		if (!players[g_playerid].ship2_isActive)
		{
			players[g_playerid].ship2_isActive = true;
		}
	}
	
}
// Checks if there is a colision between player and another hitbox. Used for enemy-player colisions. 
void checkColisionEnemyPlayer(Thitbox enemy){
	
	//Player colision
	Thitbox player = getHitboxFromSprite(g_player_spr[players[g_playerid].currentShip-1], players[g_playerid].st.x, players[g_playerid].st.y, 0, true);
	
	
	//If ship3 is active adjusts the colision box.
	if (players[g_playerid].ship3_isActive && players[g_playerid].ship3_isAlive)
	{
		player.maxy += esat::SpriteHeight(g_player_spr[2])*k_scale;
		player.minx -= 4*k_scale;
		player.maxx += 4*k_scale;
	}
	
	//Checks colision.
	if (checkHitBox(enemy, player))
	{
		g_levelState = Pos;
		players[g_playerid].isExploding = true;
	}
	
}

// -- Game states/levels
// Menu Start
void menu(){
	// UI used in menu levels
	menuUi();

	//Menu anumation
	if (g_menutext_1_x > 48*k_scale)
	{
		g_menutext_1_x -= 2*k_scale;
	}
	if (g_menutext_2_x > 75*k_scale)
	{
		g_menutext_2_x -= 2*k_scale;
	}
	if (g_menutext_3_x > 75*k_scale)
	{
		g_menutext_3_x -= 2*k_scale;
		
		//Start timer for next part of the ui animation
		g_aliveOnLevelTime = esat::Time();

	}else{ // After main animation is finished text appears on a timer

		if (esat::Time() - g_aliveOnLevelTime > 2000)
		{
			esat::DrawSetFillColor(78,252,241);
			esat::DrawText(55*k_scale,150*k_scale,"YOU  CAN  GET  A  LOT");
			if (esat::Time() - g_aliveOnLevelTime > 2600)
			{	
				esat::DrawText(55*k_scale,170*k_scale,"OF  FUN  AND  THRILL");
				if (esat::Time() - g_aliveOnLevelTime > 3500)
				{
					esat::DrawSetFillColor(250,250,250);
					esat::DrawText(80*k_scale,215*k_scale,"Nichibitsu");
					esat::DrawText(30*k_scale,230*k_scale,"1980  NIHON  BUSSAN  CD   LTD");
				}
			}
		}
	}
	// Nichibitsu logo in japanese
	st_fonts.sprite_origin_x =  esat::SpriteWidth(g_spr_nichibitsu)/2;
	st_fonts.x = g_trueWidth/2;
	st_fonts.y = 21*k_scale;
	esat::DrawSprite(g_spr_nichibitsu,st_fonts);

	esat::DrawSetFillColor(10,230,10);
	esat::DrawText(g_menutext_1_x,60*k_scale,"TRIP  TO  THE  SPACE WAR");
	esat::DrawSetFillColor(237,227,81);
	esat::DrawText(g_menutext_2_x,90*k_scale,"MOON  CRESTA");
	esat::DrawSetFillColor(214,81,237);
	esat::DrawText(g_menutext_3_x,130*k_scale,"TRY  IT  NOW  !!");
	esat::DrawSetFillColor(78,252,241);

	// Go to scoreboard after pressing Space 
	if (esat::IsSpecialKeyUp(esat::kSpecialKey_Space))
	{
		g_gameManager = Scoreboard;
	}
	
	

}
// Scoreboard Menu
void scoreboard(){
	menuUi();
	outputArray[7];
	esat::DrawSetFillColor(194,79,214);
	esat::DrawText(85*k_scale,40*k_scale,"SCORE");
	esat::DrawText(160*k_scale,40*k_scale,"NAME");

	esat::DrawSetFillColor(200,50,50);
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(40*k_scale,55*k_scale,"NO1");
	esat::DrawText(75*k_scale,55*k_scale,outputArray);
	esat::DrawText(135*k_scale,55*k_scale,hiscore[0].name);

	esat::DrawSetFillColor(200,105,50);
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(40*k_scale,70*k_scale,"NO2");
	esat::DrawText(75*k_scale,70*k_scale,outputArray);
	esat::DrawText(135*k_scale,70*k_scale,hiscore[0].name);
	
	esat::DrawSetFillColor(200,175,50);
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(40*k_scale,85*k_scale,"NO3");
	esat::DrawText(75*k_scale,85*k_scale,outputArray);
	esat::DrawText(135*k_scale,85*k_scale,hiscore[0].name);
	
	esat::DrawSetFillColor(50,200,50);
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(40*k_scale,100*k_scale,"NO4");
	esat::DrawText(75*k_scale,100*k_scale,outputArray);
	esat::DrawText(135*k_scale,100*k_scale,hiscore[0].name);
	
	esat::DrawSetFillColor(78,252,241);
	getStringFromInt(hiscore[0].score, outputArray);
	esat::DrawText(40*k_scale,115*k_scale,"NO5");
	esat::DrawText(75*k_scale,115*k_scale,outputArray);
	esat::DrawText(135*k_scale,115*k_scale,hiscore[0].name);

	esat::DrawSetFillColor(250,250,250);
	esat::DrawText(80*k_scale,215*k_scale,"Nichibitsu");
	esat::DrawText(30*k_scale,230*k_scale,"1980  NIHON  BUSSAN  CD   LTD");

	//Go to next level if there are credits.
	if (esat::IsSpecialKeyUp(esat::kSpecialKey_Space) && g_credits>0)
	{
		//If more than 1 credit go 2 players.
		if (g_credits>1)
		{
			twoPlayers = true;
			g_credits-=2;
		}else{
			g_credits--;
		}

		g_gameManager = EyeGreen;
	}
}

// Paints the ship that the player must dock to.
void paintDockingPlayer(){
	//Temp sprite transform based on dockingst
	esat::SpriteTransform temp = dockingst;
	
	//If player 2 is not active but alive
	if (!players[g_playerid].ship2_isActive && players[g_playerid].ship2_isAlive){
		//Paint ship, add the height to temp and changeSpriteOrigin
		temp.sprite_origin_x = esat::SpriteWidth(g_player_spr[1])/2;
		esat::DrawSprite(g_player_spr[1], temp);
		temp.y += esat::SpriteHeight(g_player_spr[1])*k_scale;
	}
	//If ship3 is alive and ship2 is not alive or ship 2 is active
	if (players[g_playerid].ship3_isAlive && (!players[g_playerid].ship2_isAlive || players[g_playerid].ship2_isActive)){
		//Paint ship, add the height to temp and changeSpriteOrigin
		temp.sprite_origin_x = esat::SpriteWidth(g_player_spr[2])/2;
		esat::DrawSprite(g_player_spr[2], temp);
		temp.y += esat::SpriteHeight(g_player_spr[2])*k_scale;
	}

	temp.sprite_origin_x = esat::SpriteWidth(g_fuel[g_fuel_counter])/2;
	temp.x += k_scale;
	esat::DrawSprite(g_fuel[g_fuel_counter], temp);

	// Fuel sprite control with cooldown
	if(g_fuel_cooldown >= 2){
		if(g_fuel_counter >1){
			g_fuel_counter = 0;
		}
		g_fuel_counter++;
		g_fuel_cooldown = 0;
	}
	g_fuel_cooldown++;
}
// Preparation of the docking.
void dockingPre(){

		//Gets the docking height the first time dockingPre is executed.
		if (g_dockingHeight == 0)
		{
			g_dockingHeight = players[g_playerid].st.y;
			if (players[g_playerid].ship1_isActive && players[g_playerid].ship1_isAlive)
			{
				g_dockingHeight += esat::SpriteHeight(g_player_spr[0])*k_scale;
			}
			if (players[g_playerid].ship2_isActive && players[g_playerid].ship2_isAlive)
			{
				g_dockingHeight += esat::SpriteHeight(g_player_spr[1])*k_scale;
			}
		}
		// Moves the player up. If he is already up else.
		if (players[g_playerid].st.y >= g_trueHeight*0.4)
		{
			players[g_playerid].st.y -= 1*k_scale;
			paintPlayer(true);
		}else{
			
			//Paints both player and docking ship.
			paintPlayer(false);
			paintDockingPlayer();
			 // Moves the docking ship up from below. If he is already up goes to next phase.
			if (dockingst.y > g_dockingHeight)
			{
				dockingst.y -= 1*k_scale;
			}else{ 
				
				g_levelState = OnGoing;
				//Random gravity left or right.
				if (rand()%2 == 0)
				{
					g_gravity_side = -0.4f;
				}else{
					g_gravity_side = 0.4f;
				}
				g_aliveOnLevelTime = -1;
			}
			
	}
	//UI

	esat::DrawSetFillColor(200,200,50);
	esat::DrawText(70*k_scale,60*k_scale,"MISSION   AIM");

	esat::DrawSetFillColor(75,200,50);
	esat::DrawText(45*k_scale,80*k_scale,"DOCKING   OF   ROCKETS");

	esat::DrawSetFillColor(78,252,241);
	esat::DrawText(35*k_scale,100*k_scale,"USING   LEVER   AND   BUTTON");
}
// Docking gameplay part.
void dockingOnGoing(){

	// First time on dockingOnGoing starts the timer.
	if (g_aliveOnLevelTime == -1)
	{
		g_aliveOnLevelTime = esat::Time();
	}
	// Timer.
	float docking_time = esat::Time() - g_aliveOnLevelTime;
	
	// Checks the time to see if there is still time to dock. If not goes to levelState Pos with OutOfTime result.
	if (docking_time <= 30000) 
	{
		// Moves player slightly to the left.
		if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left) && g_gravity_side > -1){
			g_gravity_side -= 0.07f;
		}
		// Moves player slightly to the right.
		if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right) && g_gravity_side < 1){
			g_gravity_side += 0.07f;
		}
		// If space pressed moves player up with a limit. If not moves it down with a limit.
		if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Space)){
			if (g_gravity_down > -0.15f)
			{
				g_gravity_down -= 0.8f;
			}
			// Paints player with fuel.
			paintPlayer(true);
		}else{

			if (g_gravity_down < 0.6)
			{
				g_gravity_down += 0.05f;	
			}
			// Paints player with no fuel.
			paintPlayer(false);
		}
	// Changes player position.
	players[g_playerid].st.y += g_gravity_down*k_scale;
	players[g_playerid].st.x += g_gravity_side*k_scale;

	
	paintDockingPlayer();

	// Creates the player hitbox and ajust it.
	Thitbox hitboxPlayer = getHitboxFromSprite(g_player_spr[players[g_playerid].currentShip-1], players[g_playerid].st.x, players[g_playerid].st.y, 0, true);
	hitboxPlayer.maxy += (esat::SpriteHeight(g_player_spr[players[g_playerid].currentShip-1])*k_scale)/2;
	if (players[g_playerid].currentShip == 2 && !players[g_playerid].ship1_isActive)
	{
		hitboxPlayer.maxy -= (hitboxPlayer.maxy - players[g_playerid].st.y)*0.5;
	}
	// Creates the hitbox of the  the player for landing .
	Thitbox hitboxPlayerLanding =  {players[g_playerid].st.x-2.5*k_scale, players[g_playerid].st.x + 2.5*k_scale,
		hitboxPlayer.miny + 2*k_scale , hitboxPlayer.maxy};
	hitboxPlayer.maxy -= 2*k_scale; 
	Thitbox landing = {g_trueWidth*0.48, g_trueWidth*0.52, g_dockingHeight-1, g_dockingHeight+2*k_scale};
	
	// Creates the hitbox for the docking where you cannot land.
	Thitbox badLanding;
	if (players[g_playerid].currentShip == 1 && players[g_playerid].ship2_isAlive)
	{
		dockingship = 2;
		badLanding = getHitboxFromSprite(g_player_spr[1], dockingst.x, dockingst.y, -1, true);
	}else{
		dockingship = 3;
		badLanding = getHitboxFromSprite(g_player_spr[2], dockingst.x, dockingst.y, -2, true);
	}
	badLanding.maxx += 4*k_scale;
	badLanding.minx -= 4*k_scale;
	
	// Checks if the player does the landing correctly.
	if (checkHitBox(landing, hitboxPlayerLanding))
	{
		// Bonus Score for docking calculation.
		getStringFromInt(30000-docking_time, outputArray);
		dockingBonus = 30000-docking_time;
		dockingBonus /= 1000;
		
		// Finishes OnGoing good.
		g_dockingState = Success;
		g_levelState = Pos;
		g_aliveOnLevelTime = -1;
	}
	// Checks if the player does not do the landing correctly.
	if(checkHitBox(badLanding, hitboxPlayer)){
		
		// Finishes OnGoing badly.
		g_dockingState = Failure;
		g_levelState = Pos;
		g_aliveOnLevelTime = -1;
		// Player explosion.
		players[g_playerid].isExploding = true;
	}

	}else{
		g_dockingState = OutOfTime;
		g_levelState = Pos;
		g_aliveOnLevelTime = -1;
	}
	
	
	//UI
	esat::DrawSetFillColor(200,200,50);
	esat::DrawText(60*k_scale,60*k_scale,"DOCKING   TIME");

	esat::DrawSetFillColor(250,250,250);
	getStringFromInt(30000-docking_time, outputArray);
	char seconds[3] = {outputArray[1], outputArray[2], '\0'};
	char miliseconds[2] = {outputArray[3], '\0'};
	esat::DrawText(85*k_scale,80*k_scale,seconds);
	
	esat::DrawText(105*k_scale,80*k_scale,miliseconds);
}
// After Docking part with result message.
void dockingPos(){
	
	// First time on dockingPos starts the timer.
	if (g_aliveOnLevelTime == -1)
	{
		g_aliveOnLevelTime = esat::Time();
	}
	
	paintPlayer(false);
	paintDockingPlayer();

	//Check the result of the docking
	
	if (g_dockingState == Success) //Activates the new ship and goes to next level.
	{
		//Timer so its not instant.
		if (esat::Time() - g_aliveOnLevelTime > 4000)
		{ 
			if (dockingship == 2)
		{
			players[g_playerid].ship2_isActive = true;
		}else{
			players[g_playerid].ship3_isActive = true;
		}
		players[g_playerid].st.x = g_trueWidth/2;
			g_gameManager = nextLevel;
			g_levelState = Pre;
			players[g_playerid].score += dockingBonus*100;
		}

	}else if (g_dockingState == Failure) // Unalives the current ships and activates the next one.
	{
	
		if (!players[g_playerid].isExploding)
		{
			if (players[g_playerid].currentShip == 1)
			{
				players[g_playerid].ship1_isAlive = false;
				players[g_playerid].ship2_isActive = true;
			}else if (players[g_playerid].currentShip == 2)
			{
				players[g_playerid].ship1_isAlive = false;
				players[g_playerid].ship2_isAlive = false;
				players[g_playerid].ship3_isActive = true;
			}else if (players[g_playerid].currentShip == 3)
			{
				// GAME OVER
				g_aliveOnLevelTime = esat::Time();
				g_gameManager = GameOver;
			}
			players[g_playerid].st.x = g_trueWidth/2;
			players[g_playerid].st.y = g_trueHeight*0.78f;
			g_gameManager = nextLevel;
			g_levelState = Pre;
		}

	}else if (g_dockingState == OutOfTime) // Makes the docking ship go away and goes to the next level
	{
		dockingst.y += 1*k_scale;
		if (esat::Time() - g_aliveOnLevelTime > 4000)
		{ 
			players[g_playerid].st.x = g_trueWidth/2;
			players[g_playerid].st.y = g_trueHeight*0.78f;
			g_gameManager = nextLevel;
			g_levelState = Pre;
		}
	}
	
	//UI
	if (g_dockingState == Success)
	{
		esat::DrawSetFillColor(80,200,240);
		esat::DrawText(60*k_scale,60*k_scale,"DOCKING   BONUS");
		getStringFromInt(dockingBonus, outputArray);
		for (int i = 0; i < 4; i++)
		{
			outputArray[i] = ' ';
		}
		esat::DrawText(75*k_scale,80*k_scale, outputArray);
		esat::DrawText(100*k_scale,80*k_scale, "X  100 ");

		//Docking bonus number
		getStringFromInt(dockingBonus*100, outputArray);
		for (int i = 0; i < 2; i++)
		{
			outputArray[i] = ' ';
		}
		esat::DrawText(145*k_scale,80*k_scale, outputArray);

		esat::DrawSetFillColor(220,240,80);
		esat::DrawText(70*k_scale,200*k_scale, "RIGHT  ON  !");
	}else
	{
		esat::DrawSetFillColor(250,80,80);
		esat::DrawText(60*k_scale,60*k_scale,"SORRY  NO  BONUS");
	}
}
//Docking level manager.
void docking(){
	
	bool isDockingAble = true;

	// Checks if docking is possible.
	if (players[g_playerid].currentShip == 2 && !players[g_playerid].ship3_isAlive)
	{
		isDockingAble = false;
	}
	if (players[g_playerid].currentShip == 3)
	{
		isDockingAble = false;
	}
	if (players[g_playerid].currentShip == 1 && !players[g_playerid].ship3_isAlive && !players[g_playerid].ship2_isAlive)
	{
		isDockingAble = false;
	}
	
	if (isDockingAble)
	{
		switch (g_levelState)
		{
		case Pre:
			dockingPre(); // Start Animation of docking
			break;
		case OnGoing:
			dockingOnGoing(); // Main Docking game
			break;
		case Pos:
			dockingPos(); // End Animation of docking
			break;
		default:
			printf("ERROR EYE LEVEL STATE!");
			break;
		}
	}else{
		g_gameManager = nextLevel;
		g_levelState = Pre;
	}
	
	gameUi();
}
//Start Animation.
void startAnim(){
	//NOT DONE BECAUSE OF TIME. :(
	printf("\n Start Animation.");
}
// Animation after finishing last level.
void endAnim(){
	//Moves player up.
	paintPlayer(true);
	players[g_playerid].st.y -= 2*k_scale;
	
	//UI
	esat::DrawSetFillColor(250,80,80);
	esat::DrawText(95*k_scale,40*k_scale,"FAR   OUT!!");

	//If already outside of screen reset game and start with an increased level.
	if (players[g_playerid].st.y <= -30 * k_scale)
	{
		g_levelState = Pre;
		g_gameManager = EyeGreen;
		level++;
		resetPlayer();
		initPlayerStruct();
		resetStartEnemyPositions();

		g_splineStep = 0;

		printf("NEW LEVEL! - %d -", level);
	}
}

// Pre level starts the level with the enemy animation.
void eyeLevelPre(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++)
	{
		if (i%2==0)
		{
			eyeEnemies[i].st.angle=k_PI;
		}
		
		//Start animation
		if(i<2){	
			// New position = original position + (end position - original position) * step
			eyeEnemies[i].st.x = g_trueWidth*0.4 + (eyeEnemies[i].start_pos.x-g_trueWidth*0.4) * g_splineStep;
			eyeEnemies[i].st.y = g_trueHeight*0.3 + (eyeEnemies[i].start_pos.y-g_trueHeight*0.3)* g_splineStep;
		}else if(i<4){
			eyeEnemies[i].st.x = g_trueWidth*0.5+ (eyeEnemies[i].start_pos.x-g_trueWidth*0.5)* g_splineStep;
			eyeEnemies[i].st.y = g_trueHeight*0.25+ (eyeEnemies[i].start_pos.y-g_trueHeight*0.25)* g_splineStep;
		}else if(i<6){
			eyeEnemies[i].st.x = g_trueWidth*0.5+ (eyeEnemies[i].start_pos.x-g_trueWidth*0.5)* g_splineStep;
			eyeEnemies[i].st.y = g_trueHeight*0.35+ (eyeEnemies[i].start_pos.y-g_trueHeight*0.35)* g_splineStep;
		}else{
			eyeEnemies[i].st.x = g_trueWidth*0.6+ (eyeEnemies[i].start_pos.x-g_trueWidth*0.6)* g_splineStep;
			eyeEnemies[i].st.y = g_trueHeight*0.3+ (eyeEnemies[i].start_pos.y-g_trueHeight*0.3)* g_splineStep;
		}
		
		if (eyeEnemies[i].isExploding)
		{
			esat::DrawSprite(g_spr_enemy_explosion[eyeEnemies[i].explodingCounter], eyeEnemies[i].st);
			eyeEnemies[i].explodingCounter++;
			if (eyeEnemies[i].explodingCounter >=4)
			{
				eyeEnemies[i].isAlive = false;
				eyeEnemies[i].explodingCounter = 0;
				eyeEnemies[i].isExploding = false;
			}
		}else{
			if (eyeEnemies[i].isAlive)
			{
				if(color == Green){
					esat::DrawSprite(g_spr_eye_green[0], eyeEnemies[i].st);
				}else{
					esat::DrawSprite(g_spr_eye_blue[0], eyeEnemies[i].st);
				}
			}
		}
	}
	g_splineStep+=0.1;

	// If animation finishes go to next phase.
	if (g_splineStep >=1)
	{
		printf("\nEye Level Pre Finished. Ongoing->");
		g_splineStep = 0;
		g_levelState = OnGoing;
	}
	
}
// OnGoing is the main gameplay part of the level. Completed after defeating all enemies.
void eyeLevelOnGoing(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Counter for Alive enemies
	int enemyCounter = 0;
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue){ arrayOffSet = 8; }
	
	for ( int i = 0+arrayOffSet; i < 8+arrayOffSet; i++)
	{
		
		if (eyeEnemies[i].isAlive){
			//Adds 1 to alive enemies.
			enemyCounter+=1;
			// If enemy is aggresive
			if (eyeEnemies[i].isAggresive){	
				// If enemy moves and rotations is less than max
				if (eyeEnemies[i].isMoving && eyeEnemies[i].max_rotations > eyeEnemies[i].rotations){
					// If appearing true 
					if (eyeEnemies[i].isAppearing)
					{
						// Makes them rotate while appearing
						float vt = eyeEnemies[i].rotations / 50 * k_PI * k_scale*0.1;
						if (eyeEnemies[i].speed > 0)
						{
							eyeEnemies[i].st.x = (vt * 15  + 5) * cos(vt);
							eyeEnemies[i].st.y = (vt * 15 + 5) * sin(vt);
						}else{
							eyeEnemies[i].st.y = (vt * 15 + 5) * cos(vt);
							eyeEnemies[i].st.x = (vt * 15 + 5) * sin(vt);
						}
						eyeEnemies[i].st.x += eyeEnemies[i].ref_rotation.x;
						eyeEnemies[i].st.y += eyeEnemies[i].ref_rotation.y;
						eyeEnemies[i].st.angle = k_PI;

						//Makes the sprite go from small to big
						float sprite_sections = eyeEnemies[i].max_rotations / 8;
						
						int sprite = 7;
						for (int j = 0; j < 7; ++j) {
    					if (eyeEnemies[i].rotations < sprite_sections * (j + 1)) {
        				sprite = 14 - j;
        				break;
   						 }
						}

						//Paint enemy based on color and sprite
						if(color == Green ){
							esat::DrawSprite(g_spr_eye_green[sprite], eyeEnemies[i].st);
						}else{
							esat::DrawSprite(g_spr_eye_blue[sprite], eyeEnemies[i].st);
						}
						// Adds enemy speed to rotations but always positive.
						if (eyeEnemies[i].speed < 0)
						{
							eyeEnemies[i].rotations += -eyeEnemies[i].speed;
						}else{
							eyeEnemies[i].rotations += eyeEnemies[i].speed;
						}
						//If rotation is finished make them stop appearing.
						if (eyeEnemies[i].max_rotations < eyeEnemies[i].rotations)
						{
							eyeEnemies[i].isAppearing = false;
							eyeEnemies[i].isMoving = false;
							eyeEnemies[i].pattern = 0;
						}
						

					}else{ //If appearing false
						
						// Check Spline type patterns
						if (eyeEnemies[i].pattern == 10 || eyeEnemies[i].pattern == 11)
						{
							// Moves enemy based on pattern. This only works for spline type patterns.
							eyeEnemies[i].st.x = eyeEnemies[i].ref_rotation.x + (patternsEye[eyeEnemies[i].pattern].rotation_center_offset.x) * eyeEnemies[i].rotations;
							eyeEnemies[i].st.y = eyeEnemies[i].ref_rotation.y + (patternsEye[eyeEnemies[i].pattern].rotation_center_offset.y) * eyeEnemies[i].rotations;
							
						}else{ // If not a spline pattern
							
							//Rotates enemy based on pattern using rotate function.
							rotate(&eyeEnemies[i].st,
								eyeEnemies[i].ref_rotation.x,eyeEnemies[i].ref_rotation.y,eyeEnemies[i].speed,true);
						}
						
						// For pattern 6 and 7 rotate 180 degrees the sprite.
						if (eyeEnemies[i].pattern == 6 || eyeEnemies[i].pattern == 7)
						{
							eyeEnemies[i].st.angle += k_PI;
						}else if (eyeEnemies[i].joined && i%2==0 && eyeEnemies[i].pattern != 10 && eyeEnemies[i].pattern != 11)
						{
							//If eye is joined with another and is even also rotate 180 degrees the sprite.
							eyeEnemies[i].st.angle += k_PI;
						}
						
						// Adds enemy speed to rotations but always positive.
						if (eyeEnemies[i].speed < 0)
						{
							eyeEnemies[i].rotations += -eyeEnemies[i].speed;
						}else{
							eyeEnemies[i].rotations += eyeEnemies[i].speed;
						}
						// If level is bigger than 1 there is an incremental based on level chance of sudden change of pattern of enemy.
						if (level > 1)
						{
							int difficulty = level*5;
							if (difficulty > 600)
							{
								difficulty = 600;
							}
							int random = rand()%1000-difficulty;
							if (random == 0)
							{
								eyeEnemies[i].isMoving = false;
							}						
						}
						
					}
				}else{ // If ship is not moving or rotations are bigger than the max
					//New movement
					int newPattern;

					// If next pattern is not predetermined by the current pattern
					if (patternsEye[eyeEnemies[i].pattern].nextPattern == -1)
					{
						do
						{
							//Select random pattern from array.
							newPattern = rand()%16;
							// Repeat this until a patter is found that is eligible
						} while (!patternsEye[newPattern].isEligible );
					
					}else{
						//Select predetermined pattern
						newPattern = patternsEye[eyeEnemies[i].pattern].nextPattern;
					}
					// If eye is joined with another and odd have the pattern be the one from before it.
					if (eyeEnemies[i].joined && i%2!=0)
					{
						newPattern = eyeEnemies[i-1].pattern;
					}
					// Put the pattern info inside the player with start values.
					eyeEnemies[i].pattern = newPattern;
					eyeEnemies[i].rotations = 0;
					eyeEnemies[i].speed = patternsEye[newPattern].speed;
					eyeEnemies[i].max_rotations =  patternsEye[newPattern].max_rotations;
					
					//If movement pattern is spline type.
					if (eyeEnemies[i].pattern == 10 || eyeEnemies[i].pattern == 11){
						
						eyeEnemies[i].ref_rotation.x = eyeEnemies[i].st.x;
						eyeEnemies[i].ref_rotation.y = eyeEnemies[i].st.y;
						// Move the enemy.
						eyeEnemies[i].st.x = eyeEnemies[i].ref_rotation.x + (patternsEye[eyeEnemies[i].pattern].rotation_center_offset.x) * eyeEnemies[i].rotations;
						eyeEnemies[i].st.y = eyeEnemies[i].ref_rotation.y + (patternsEye[eyeEnemies[i].pattern].rotation_center_offset.y) * eyeEnemies[i].rotations;
						// Change angle based on movement direction.
						eyeEnemies[i].st.angle = atan2(patternsEye[eyeEnemies[i].pattern].rotation_center_offset.y, patternsEye[eyeEnemies[i].pattern].rotation_center_offset.x) - k_PI/2;
						
						if (eyeEnemies[i].joined && i%2!=0)
						{
							eyeEnemies[i-1].st.angle = eyeEnemies[i].st.angle - k_PI;
						}
						
					}else{
						// Ref rotation is the center of the rotation.
						eyeEnemies[i].ref_rotation.x = patternsEye[newPattern].rotation_center_offset.x + eyeEnemies[i].st.x;
						eyeEnemies[i].ref_rotation.y = patternsEye[newPattern].rotation_center_offset.y + eyeEnemies[i].st.y;
						
						// Rotate enemy using rotate funcion.
						rotate(&eyeEnemies[i].st,
						eyeEnemies[i].ref_rotation.x,eyeEnemies[i].ref_rotation.y,eyeEnemies[i].speed,true);
						eyeEnemies[i].rotations += eyeEnemies[i].speed;
					}

					// Make it move.
					eyeEnemies[i].isMoving = true;

					// For pattern 6 and 7 rotate 180 degrees the sprite.
					if (eyeEnemies[i].pattern == 6 || eyeEnemies[i].pattern == 7)
						{
							eyeEnemies[i].st.angle += k_PI;
						}
				}
			}else{ // If not aggressive
				//Continue circular start movement
				if (i%2==0)
				{
					eyeEnemies[i].st.angle=k_PI;
				}
				
				// Circular 8 form movement
				float cy = eyeEnemies[i].start_pos.y;
				float cx = eyeEnemies[i].start_pos.x - 25*k_scale;
				float cx2 = eyeEnemies[i].start_pos.x + 25*k_scale;

				if (eyeEnemies[i].rotations >720)
				{
					eyeEnemies[i].rotations=0;
				}
				if(eyeEnemies[i].rotations < 360){
					rotate(&eyeEnemies[i].st,cx,cy,-6, false);
					eyeEnemies[i].rotations+=6;
				}else{
					rotate(&eyeEnemies[i].st,cx2,cy,6,false);
					eyeEnemies[i].rotations+=6;
				}
				
				
				// IF LEVEL TIME > x* (Changes based on current level)
				if (esat::Time() - g_aliveOnLevelTime > 3500-(level*150)){
					
					// RANDOM CHANCE TO GO AGGRESIVE
					if(rand()%169 == 3){
						
						// Makes them go aggressive
						if (i%2 == 0)
						{
							eyeEnemies[i+1].isAggresive = true;
						}else{
							eyeEnemies[i-1].isAggresive = true;
						}
						eyeEnemies[i].isAggresive = true;
					}
				}
				// Makes them unjoined
				if (esat::Time() - g_aliveOnLevelTime > 7500-(level*150)){
					if(rand()%369 == 3){
						if (i%2 == 0)
						{
							eyeEnemies[i+1].joined = false;
						}else{
							eyeEnemies[i-1].joined = false;
						}
						eyeEnemies[i].joined = false;
					}
				}
			}
			//Collisions

			//Enemy and window colision.
			Thitbox enemy = getHitboxFromSprite(g_spr_eye_green[0],eyeEnemies[i].st.x,eyeEnemies[i].st.y,5, true);
			Thitbox window = {0, g_trueWidth+0.0f, 0 , g_trueHeight+0.0f};
			
			//Checks if enemy colides with player.
			checkColisionEnemyPlayer(enemy);

			//Check if enemy is out of screen and makes it appear again. Its not instant to make it more natural.
			if (!checkHitBox(enemy,window))
			{
				int random =rand()%10;
				if (random == 3)
				{
					//Reset values.
					eyeEnemies[i].isAppearing = true;

					// Appearing rotation has 2 types.
					random =rand()%2;
					if (random == 1)
					{
						eyeEnemies[i].st.x = g_trueWidth*0.45;
						eyeEnemies[i].st.y = g_trueHeight*0.25;
						eyeEnemies[i].speed = 7;
						eyeEnemies[i].max_rotations =  600; 
					}else{
						eyeEnemies[i].st.x = g_trueWidth*0.55;
						eyeEnemies[i].st.y = g_trueHeight*0.25;
						eyeEnemies[i].speed = -7;
						eyeEnemies[i].max_rotations =  550; 
					}				
				
					// New position for appearing
					eyeEnemies[i].ref_rotation.x = g_trueWidth/2;
					eyeEnemies[i].ref_rotation.y = g_trueHeight*0.25;
				
					//Reset values.
					eyeEnemies[i].rotations = 0;
					eyeEnemies[i].isMoving = true;

				}
			}
			
			// Checks colision with player bullets. If colision make them explode.
			for (int j = 0; j < 5; j++)
			{
				if (players[g_playerid].bullets[j].isActive)
				{
					
					Thitbox bullet = {players[g_playerid].bullets[j].x-6,
					(players[g_playerid].bullets[j].x+g_bulletWidth+6),
					players[g_playerid].bullets[j].y,
					players[g_playerid].bullets[j].y+g_bulletHeight};

					// IF there is a colision
					if (checkHitBox(enemy, bullet))
					{
						eyeEnemies[i].isExploding = true;
						players[g_playerid].bullets[j].isActive = false;
						players[g_playerid].score += 50; // SCORE
						// Makes the the other aggressive and not joined.
						if (i%2 == 0)
						{
							eyeEnemies[i+1].isAggresive = true;
							eyeEnemies[i+1].joined = false;
						}else{
							eyeEnemies[i-1].isAggresive = true;
							eyeEnemies[i-1].joined = false;
						}
					}
				}
			}

			// If enemy not appearing
			if (!eyeEnemies[i].isAppearing)
			{
				// If exploding paint explosion and add to the counter of the sprite.
				if (eyeEnemies[i].isExploding)
				{
					esat::DrawSprite(g_spr_enemy_explosion[eyeEnemies[i].explodingCounter], eyeEnemies[i].st);
					eyeEnemies[i].explodingCounter++;
					if (eyeEnemies[i].explodingCounter >=4)
					{
						eyeEnemies[i].isAlive = false;
						eyeEnemies[i].explodingCounter = 0;
						eyeEnemies[i].isExploding = false;
					}
				}else{ // IF not exploding paint enemy normally.
					if(color == Green ){
						esat::DrawSprite(g_spr_eye_green[0], eyeEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_eye_blue[0], eyeEnemies[i].st);
					}
				}
			}
		}
	}

	if (enemyCounter<=0) { //Finish level if no more alive enemies
		printf("\n - Stage Complete! - ");
		resetStartEnemyPositions();
		g_levelState = Pre;
		if (g_gameManager == EyeGreen)
		{
			g_splineStep = 0;
			g_gameManager = EyeBlue;
		}else{		
			g_gameManager = FlyGreen;
		}
	}
}
// Pos used after colision with enemies.
void eyeLevelPos(enum enemyColor color){

	// Paint player explosion.
	if (players[g_playerid].isExploding)
	{
		paintPlayer(false);
	}else{ // After player explosion

		// onColision does the changes to the ship.
		onColision();
		// Enemies are reset withouth reviving
		int arrayOffSet = 0;
		if (color == Blue){ arrayOffSet = 8; }
		for ( int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
			//Out of screen
			eyeEnemies[i].st.x = g_trueWidth*2;
			eyeEnemies[i].isAggresive = false;
			eyeEnemies[i].pattern = 0;
			
			if (i<2)
			{
				eyeEnemies[i].start_pos.x = 76*k_scale;
				eyeEnemies[i].start_pos.y = 82*k_scale;
			}else if (i<4)
			{
				eyeEnemies[i].start_pos.x = 112*k_scale;
				eyeEnemies[i].start_pos.y = 66*k_scale;
			}else if (i<6)
			{
				eyeEnemies[i].start_pos.x = 112*k_scale;
				eyeEnemies[i].start_pos.y = 98*k_scale;
			}else{
				eyeEnemies[i].start_pos.x = 148*k_scale;
				eyeEnemies[i].start_pos.y = 82*k_scale;
			}
		}
		// Goes back to the start
		g_aliveOnLevelTime = esat::Time();
		g_levelState = Pre;	
	}
}

// Eye enemy level manager.
void eyeLevel(enum enemyColor color){

	switch (g_levelState)
	{
	case Pre:
		eyeLevelPre(color);
		break;
	case OnGoing:
		eyeLevelOnGoing(color);
		break;
	case Pos:
		eyeLevelPos(color);
		break;
	default:
		printf("ERROR EYE LEVEL STATE!");
		break;
	}
	//UI
	gameUi();
}

// Pre level starts the level with the enemy animation.
void flyLevelPre(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++)
	{	
		// Based on pattern 0 or 1 generated randomly makes a animation using a combined spline of 3 points.
		if (flyEnemies[i].pattern == 0)
		{
			// X position 
			float p1x = flyEnemies[i].start_pos.x + (g_trueWidth*0.1+flyEnemies[i].ref_rotation.x - flyEnemies[i].start_pos.x) * g_splineStep;
			float p2x = g_trueWidth*0.1+flyEnemies[i].ref_rotation.x + ((g_trueWidth*0.2 + flyEnemies[i].ref_rotation.y - g_trueWidth*0.1+flyEnemies[i].ref_rotation.x) - g_trueWidth*0.1) * g_splineStep;
			flyEnemies[i].st.x =  p1x + (p2x-p1x)*g_splineStep;

			// Y position 
			float p1y = flyEnemies[i].start_pos.y + (g_trueHeight*0.35 - flyEnemies[i].start_pos.y) * g_splineStep;
			float p2y = g_trueHeight*0.35 + ((g_trueHeight*0.15) - g_trueHeight*0.35) * g_splineStep;
			flyEnemies[i].st.y =  p1y + (p2y-p1y)*g_splineStep + (rand()%10 * 0.02);

			//Sprite angle 
			flyEnemies[i].st.angle = atan2((g_trueHeight*0.15)-flyEnemies[i].st.y, (g_trueWidth*0.1 + flyEnemies[i].ref_rotation.y) - flyEnemies[i].st.x)+ k_PI/4;

		}else{
			// X position 
			float p1x = flyEnemies[i].start_pos.x + (g_trueWidth*0.9-flyEnemies[i].ref_rotation.x - flyEnemies[i].start_pos.x) * g_splineStep;
			float p2x = g_trueWidth*0.9-flyEnemies[i].ref_rotation.x + ((g_trueWidth*0.8 - flyEnemies[i].ref_rotation.y - g_trueWidth*0.9-flyEnemies[i].ref_rotation.x) - g_trueWidth*0.1) * g_splineStep;
			flyEnemies[i].st.x =  p1x + (p2x-p1x)*g_splineStep;
		
			// Y position 
			float p1y = flyEnemies[i].start_pos.y + (g_trueHeight*0.35 - flyEnemies[i].start_pos.y) * g_splineStep;
			float p2y = g_trueHeight*0.35 + ((g_trueHeight*0.15) - g_trueHeight*0.35) * g_splineStep;
			flyEnemies[i].st.y =  p1y + (p2y-p1y)*g_splineStep + (rand()%10 * 0.02);
			
			//Sprite angle 
			flyEnemies[i].st.angle = atan2((g_trueHeight*0.15)-flyEnemies[i].st.y, (g_trueWidth*0.8 - flyEnemies[i].ref_rotation.y) - flyEnemies[i].st.x )+ k_PI/4;
		}

		// Paints enemy sprite based on color
		if(color == Green){
			esat::DrawSprite(g_spr_fly_green[0], flyEnemies[i].st);
		}else{
			esat::DrawSprite(g_spr_fly_blue[0], flyEnemies[i].st);
		}
	}
	//Add to the spline
	g_splineStep+=0.1;

	// If spline finishes go to next phase
	if (g_splineStep >=0.95){
		printf("\nFly Level Pre Finished. Ongoing->");
		g_splineStep = 0;
		g_levelState = OnGoing;

		// Before going reset positions.
		for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++)
		{
			flyEnemies[i].rotations = 0;
			flyEnemies[i].speed = patternsFly[flyEnemies[i].pattern].speed;
			flyEnemies[i].max_rotations =  patternsFly[flyEnemies[i].pattern].max_rotations;
			flyEnemies[i].isMoving = true;
			flyEnemies[i].ref_rotation.x = patternsFly[flyEnemies[i].pattern].rotation_center_offset.x + flyEnemies[i].st.x;
			flyEnemies[i].ref_rotation.y = patternsFly[flyEnemies[i].pattern].rotation_center_offset.y + flyEnemies[i].st.y;
		}
	}
}
// OnGoing is the main gameplay part of the level. Completed after defeating all enemies.
void flyLevelOnGoing(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Counter for Alive enemies
	int enemyCounter = 0;
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue){ arrayOffSet = 8; }

	for ( int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
		
		if (flyEnemies[i].isAlive){
			//Adds 1 to alive enemies.
			enemyCounter+=1;
			// If enemy moves and rotations is less than max
			if (flyEnemies[i].isMoving && flyEnemies[i].max_rotations > flyEnemies[i].rotations){
				// If appearing true 
				if (flyEnemies[i].isAppearing)
				{
					//Enemy appearing animation can go right or left
					float vt = flyEnemies[i].rotations / 50 * k_PI * k_scale*0.1;
					if (flyEnemies[i].speed > 0)
					{
						flyEnemies[i].st.x = (vt * 15 + 5) * cos(vt);
						flyEnemies[i].st.y = (vt * 15 + 5) * sin(vt);
					}else{
						flyEnemies[i].st.y = (vt * 15 + 5) * cos(vt);
						flyEnemies[i].st.x = (vt * 15 + 5) * sin(vt);
					}
					flyEnemies[i].st.x += flyEnemies[i].ref_rotation.x;
					flyEnemies[i].st.y += flyEnemies[i].ref_rotation.y;

					//Makes the sprite go from small to big
					float sprite_sections = flyEnemies[i].max_rotations / 8;

					int sprite = 7;
					for (int j = 0; j < 7; ++j) {
   					if (flyEnemies[i].rotations < sprite_sections * (j + 1)) {
       				sprite = 14 - j;
       				break;
   					}
					}

					// Fixed angle of sprite
					flyEnemies[i].st.angle = k_PI;

					//Paint enemy based on color and sprite
					if(color == Green ){
						esat::DrawSprite(g_spr_fly_green[sprite], flyEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_fly_blue[sprite], flyEnemies[i].st);
					}

					// Adds enemy speed to rotations but always positive.
					if (flyEnemies[i].speed < 0)
					{
						flyEnemies[i].rotations += -flyEnemies[i].speed;
					}else{
						flyEnemies[i].rotations += flyEnemies[i].speed;
					}

					//If rotation is finished make them stop appearing.
					if (flyEnemies[i].max_rotations < flyEnemies[i].rotations)
					{
						flyEnemies[i].isAppearing = false;
						flyEnemies[i].isMoving = false;
						flyEnemies[i].pattern = 0;
					}
				}else{ //If appearing false
					
					// Adds enemy speed to rotations but always positive.
					if (flyEnemies[i].speed < 0)
					{
						flyEnemies[i].rotations += -flyEnemies[i].speed;
					}else{
						flyEnemies[i].rotations += flyEnemies[i].speed;
					}

					// Check Spline type patterns
					if (flyEnemies[i].pattern == 4 || flyEnemies[i].pattern == 5 || flyEnemies[i].pattern == 6 || 
					flyEnemies[i].pattern == 7 || flyEnemies[i].pattern == 10 || flyEnemies[i].pattern == 14 || 
					flyEnemies[i].pattern == 12)
					{
						// Moves enemy based on pattern. This only works for spline type patterns.
						flyEnemies[i].st.x = flyEnemies[i].ref_rotation.x + (patternsFly[flyEnemies[i].pattern].rotation_center_offset.x) * flyEnemies[i].rotations;
						flyEnemies[i].st.y = flyEnemies[i].ref_rotation.y + (patternsFly[flyEnemies[i].pattern].rotation_center_offset.y) * flyEnemies[i].rotations;
						
					}else{ // If not a spline pattern
						//Rotates enemy based on pattern using rotate function.
						rotate(&flyEnemies[i].st,
							flyEnemies[i].ref_rotation.x,flyEnemies[i].ref_rotation.y,flyEnemies[i].speed,true);
						// Changes rotation of sprite by 180 degrees.
						flyEnemies[i].st.angle += k_PI;
					}
				}
			}else{ // If ship is not moving or rotations are bigger than the max
				
				//Select new movement
				int newPattern;
				// If next pattern is not predetermined by the current pattern
				if (patternsFly[flyEnemies[i].pattern].nextPattern == -1)
				{
					
					bool bad = false;
					do
					{
						//Select random pattern from array.
						newPattern = rand()%17;
						
						// Repeat this until a patter is found that is eligible
					} while (!patternsFly[newPattern].isEligible);
				}else{
					//Select predetermined pattern
					newPattern = patternsFly[flyEnemies[i].pattern].nextPattern;
				}
				// Put the pattern info inside the player with start values.
				flyEnemies[i].pattern = newPattern;
				flyEnemies[i].rotations = 0;
				flyEnemies[i].speed = patternsFly[newPattern].speed;
				flyEnemies[i].max_rotations =  patternsFly[newPattern].max_rotations;
				// Make it move.
				flyEnemies[i].isMoving = true;

				//If movement pattern is spline type.
				if (flyEnemies[i].pattern == 4 || flyEnemies[i].pattern == 5 || flyEnemies[i].pattern == 6 || 
					flyEnemies[i].pattern == 7 || flyEnemies[i].pattern == 10 || flyEnemies[i].pattern == 14 || 
					flyEnemies[i].pattern == 12){
						
					flyEnemies[i].ref_rotation.x = flyEnemies[i].st.x;
					flyEnemies[i].ref_rotation.y = flyEnemies[i].st.y;
					flyEnemies[i].rotations += flyEnemies[i].speed;
					
					// Move the enemy.
					flyEnemies[i].st.x = flyEnemies[i].ref_rotation.x + (patternsFly[flyEnemies[i].pattern].rotation_center_offset.x) * flyEnemies[i].rotations;
					flyEnemies[i].st.y = flyEnemies[i].ref_rotation.y + (patternsFly[flyEnemies[i].pattern].rotation_center_offset.y) * flyEnemies[i].rotations;
					// Change angle based on movement direction.
					flyEnemies[i].st.angle = atan2(patternsFly[flyEnemies[i].pattern].rotation_center_offset.y, patternsFly[flyEnemies[i].pattern].rotation_center_offset.x) - k_PI/2 + k_PI;

				}else{
					
					// Ref rotation is the center of the rotation.
					flyEnemies[i].ref_rotation.x = patternsFly[newPattern].rotation_center_offset.x + flyEnemies[i].st.x;
					flyEnemies[i].ref_rotation.y = patternsFly[newPattern].rotation_center_offset.y + flyEnemies[i].st.y;
					
					// Rotate enemy using rotate funcion.
					rotate(&flyEnemies[i].st, flyEnemies[i].ref_rotation.x,flyEnemies[i].ref_rotation.y,flyEnemies[i].speed,true);
					flyEnemies[i].rotations += flyEnemies[i].speed;
				}
			}
			//Collisions

			//Enemy and window colision.
			Thitbox enemy = getHitboxFromSprite(g_spr_fly_green[0],flyEnemies[i].st.x,flyEnemies[i].st.y,5, true);
			Thitbox window = {0, g_trueWidth+0.0f, 0 , g_trueHeight+0.0f};
			
			//Checks if enemy colides with player.
			checkColisionEnemyPlayer(enemy);
			
			//Check if enemy is out of screen and makes it appear again. Its not instant to make it more natural.
			if (!checkHitBox(enemy,window)){
				int random =rand()%10;
				if (random == 3)
				{
					//Reset values.
					flyEnemies[i].isAppearing = true;
					random =rand()%2;

					// Appearing rotation has 2 types.
					if (random == 1)
					{
						flyEnemies[i].st.x = g_trueWidth*0.45;
						flyEnemies[i].st.y = g_trueHeight*0.35;
						flyEnemies[i].speed = 9;
						flyEnemies[i].max_rotations =  600; //TODO SOLVE PROBLEM WITH K_SCALE
					}else{
						flyEnemies[i].st.x = g_trueWidth*0.55;
						flyEnemies[i].st.y = g_trueHeight*0.35;
						flyEnemies[i].speed = -9;
						flyEnemies[i].max_rotations =  550; //TODO SOLVE PROBLEM WITH K_SCALE
					}				
					
					// New position for appearing
					flyEnemies[i].ref_rotation.x = g_trueWidth/2;
					flyEnemies[i].ref_rotation.y = g_trueHeight*0.35;
					
					//Reset values.
					flyEnemies[i].rotations = 0;
					flyEnemies[i].isMoving = true;	
				}
			}
			
			// Checks colision with player bullets. If colision make them explode.
			for (int j = 0; j < 5; j++){
				if (players[g_playerid].bullets[j].isActive)
				{				
					Thitbox bullet = {players[g_playerid].bullets[j].x-6,
					(players[g_playerid].bullets[j].x+g_bulletWidth+6),
					players[g_playerid].bullets[j].y+g_bulletHeight,
					players[g_playerid].bullets[j].y};

					// IF there is a colision
					if (checkHitBox(enemy, bullet))
					{
						players[g_playerid].score += 30; // SCORE
						flyEnemies[i].isExploding = true;
						players[g_playerid].bullets[j].isActive = false;
					}
				}
			}
			

			// If exploding paint explosion and add to the counter of the sprite.
			if (flyEnemies[i].isExploding)
			{
				esat::DrawSprite(g_spr_enemy_explosion[flyEnemies[i].explodingCounter], flyEnemies[i].st);
				flyEnemies[i].explodingCounter++;
			
				if (flyEnemies[i].explodingCounter >=4)
				{
					flyEnemies[i].isAlive = false;
					flyEnemies[i].explodingCounter = 0;
					flyEnemies[i].isExploding = false;
				}
			}else{ // IF not exploding paint enemy normally.
				if (!flyEnemies[i].isAppearing)
				{
					if(color == Green){
						esat::DrawSprite(g_spr_fly_green[0], flyEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_fly_blue[0], flyEnemies[i].st);
					}					
				}
			}
		}
	}
	//Finish level if no more alive enemies
	if (enemyCounter<=0){
		printf("\n - Stage Complete! - ");
		
		resetStartEnemyPositions();

		g_levelState = Pre;
		if (g_gameManager == FlyGreen){
			g_splineStep = 0;
			g_gameManager = FlyBlue;
		}else{
			g_gameManager = Docking;
			nextLevel = ShipsGreen;
		}
	}
}
// Pos used after colision with enemies.
void flyLevelPos(enum enemyColor color){
	
	// Paint player explosion.
	if (players[g_playerid].isExploding)
	{
		paintPlayer(false);
	}else{ // After player explosion

		// onColision does the changes to the ship.
		onColision();

		// Enemies are reset withouth reviving
		int arrayOffSet = 0;
		if (color == Blue){ arrayOffSet = 8; }
		for ( int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){

			flyEnemies[i].isAppearing = true;
				//Random appearing
					int random =rand()%2;
					
					if (random == 1)
					{
						flyEnemies[i].st.x = g_trueWidth*0.45;
						flyEnemies[i].st.y = g_trueHeight*0.35;
						flyEnemies[i].speed = 9;
						flyEnemies[i].max_rotations =  600; 
					}else{
						flyEnemies[i].st.x = g_trueWidth*0.55;
						flyEnemies[i].st.y = g_trueHeight*0.35;
						flyEnemies[i].speed = -9;
						flyEnemies[i].max_rotations =  550; 
					}				
					
					flyEnemies[i].ref_rotation.x = g_trueWidth/2;
					flyEnemies[i].ref_rotation.y = g_trueHeight*0.35;
					
					flyEnemies[i].rotations = 0;
					flyEnemies[i].isMoving = true;	
		}
   // Goes back to the OnGoing
		g_levelState = OnGoing;	
	}
}
// Fly enemy level manager.
void flyLevel(enum enemyColor color){
	
	switch (g_levelState)
	{
	case Pre:
		flyLevelPre(color);
		break;
	case OnGoing:
		flyLevelOnGoing(color);
		break;
	case Pos:
		flyLevelPos(color);
		break;
	default:
		printf("ERROR EYE LEVEL STATE!");
		break;
	}
	gameUi();
}

// Pre level starts the level with the enemy animation.
void shipLevelPre(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	//Start animation
	for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
		// Start position + (end position - start position) * step p1.
		float p1x = g_trueWidth*0.5 + (shipEnemies[i].start_pos.x - g_trueWidth*0.5) * g_splineStep;
		float p2x = shipEnemies[i].start_pos.x + (shipEnemies[i].ref_rotation.x - shipEnemies[i].start_pos.x) * g_splineStep;
		shipEnemies[i].st.x =  p1x + (p2x-p1x)*g_splineStep;
		
		float p1y = g_trueHeight*0.7 + (shipEnemies[i].start_pos.y - g_trueHeight*0.7) * g_splineStep;
		float p2y = shipEnemies[i].start_pos.y + (shipEnemies[i].ref_rotation.y - shipEnemies[i].start_pos.y) * g_splineStep;
		shipEnemies[i].st.y =  p1y + (p2y-p1y)*g_splineStep;

		int spriteCount = 8 * g_splineStep;

		//If enemy exploding paint explosion animation
		if (shipEnemies[i].isExploding)
		{ 	
			esat::DrawSprite(g_spr_enemy_explosion[shipEnemies[i].explodingCounter], shipEnemies[i].st);
			shipEnemies[i].explodingCounter++;
				
			if (shipEnemies[i].explodingCounter >=4)
			{
				shipEnemies[i].isAlive = false;
				shipEnemies[i].explodingCounter = 0;
				shipEnemies[i].isExploding = false;
			}
			}else{ //If not exploding paint enemy normally
			if(color == Green){
				esat::DrawSprite(g_spr_ship_green[spriteCount+7], shipEnemies[i].st);
			}else{
				esat::DrawSprite(g_spr_ship_blue[spriteCount+7], shipEnemies[i].st);
			}
		}

		// Checks colision with player bullets. If colision make them explode.
		Thitbox enemy = getHitboxFromSprite(g_spr_ship_green[0],shipEnemies[i].st.x,shipEnemies[i].st.y,5, true);
		for (int j = 0; j < 5; j++){
			if (players[g_playerid].bullets[j].isActive)
			{		
				Thitbox bullet = {players[g_playerid].bullets[j].x-6,
				(players[g_playerid].bullets[j].x+g_bulletWidth+6),
				players[g_playerid].bullets[j].y+g_bulletHeight,
				players[g_playerid].bullets[j].y};

				// IF there is a colision
				if (checkHitBox(enemy, bullet))
				{
					players[g_playerid].score += 60; // SCORE
					shipEnemies[i].isAggresive = false;
					shipEnemies[i].isExploding = true;
					players[g_playerid].bullets[j].isActive = false;
						
				}
			}
		}
	}

	//Add to spline for animation
	g_splineStep+=0.02;
	//If animation finish go next phase
	if (g_splineStep >=0.95){
		g_splineStep = 0;
		g_levelState = OnGoing;
		for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++)
		{
			//Reset
			shipEnemies[i].rotations = 0;
			shipEnemies[i].speed = patternsShip[shipEnemies[i].pattern].speed;
			shipEnemies[i].max_rotations =  patternsShip[shipEnemies[i].pattern].max_rotations;
			shipEnemies[i].isMoving = true;
			shipEnemies[i].ref_rotation.x = patternsShip[shipEnemies[i].pattern].rotation_center_offset.x + shipEnemies[i].st.x;
			shipEnemies[i].ref_rotation.y = patternsShip[shipEnemies[i].pattern].rotation_center_offset.y + shipEnemies[i].st.y;
		}
	}
}
// OnGoing is the main gameplay part of the level. Completed after defeating all enemies.
void shipLevelOngoing(enum enemyColor color){
	basicGameLoop();
	// Counter for Alive enemies
	int enemyCounter = 0;
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){

		if (shipEnemies[i].isAlive)
		{
			//Adds 1 to alive enemies.
			enemyCounter+=1;

			// If ship moves and rotations is less than max
			if (shipEnemies[i].isMoving && shipEnemies[i].max_rotations > shipEnemies[i].rotations){
				// If appearing true 
				if (shipEnemies[i].isAppearing)
				{
					//Sprite Count used to go from small to big sprite.
					int spriteCount = 8 * shipEnemies[i].rotations -1;
					//Add rotation.
					shipEnemies[i].rotations += 0.1;
					//If rotation is finished make them stop appearing.
					if (shipEnemies[i].rotations >= 0.95)
					{
						shipEnemies[i].isAppearing = false;
						shipEnemies[i].rotations = 0;
						shipEnemies[i].pattern = 0;
						shipEnemies[i].isMoving = false;	
					}
					//Paint enemy based on color and sprite.
					if(color == Green ){
						esat::DrawSprite(g_spr_ship_green[spriteCount+8], shipEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_ship_blue[spriteCount+8], shipEnemies[i].st);
					}

				}else{ //If appearing false
					
					// Adds enemy speed to rotations but always positive.
					if (shipEnemies[i].speed < 0)
					{
						shipEnemies[i].rotations += -shipEnemies[i].speed;
					}else{
						shipEnemies[i].rotations += shipEnemies[i].speed;
					}

					// Check Spline type patterns
					if (shipEnemies[i].pattern == 4 || shipEnemies[i].pattern == 5 || shipEnemies[i].pattern == 9 || shipEnemies[i].pattern == 11
							|| shipEnemies[i].pattern == 11 || shipEnemies[i].pattern == 13 || shipEnemies[i].pattern == 15 || shipEnemies[i].pattern == 17)
					{
						// Moves enemy based on pattern. This only works for spline type patterns.
						shipEnemies[i].st.x = shipEnemies[i].ref_rotation.x + (patternsShip[shipEnemies[i].pattern].rotation_center_offset.x) * shipEnemies[i].rotations;
						shipEnemies[i].st.y = shipEnemies[i].ref_rotation.y + (patternsShip[shipEnemies[i].pattern].rotation_center_offset.y) * shipEnemies[i].rotations;
					
					}else{ // If not a spline pattern
						
						//Rotates enemy based on pattern using rotate function.
						rotate(&shipEnemies[i].st,
							shipEnemies[i].ref_rotation.x,shipEnemies[i].ref_rotation.y,shipEnemies[i].speed,true);
						//Rotates enemy 180 degrees based on the rotation from the rotate function. 	
						shipEnemies[i].st.angle += k_PI;

					}
					// If level is bigger than 1 there is an incremental based on level chance of sudden change of pattern of enemy.
					if (level>1)
					{
						if (rand()%50-level == 0)
						{
							shipEnemies[i].isMoving = false;	
							shipEnemies[i].rotations = 0;
							shipEnemies[i].pattern = 0;
						}
						
					}
					
				}
				
			}else{ // If ship is not moving or rotations are bigger than the max
				//New movement
				int newPattern;
				
				// If next pattern is not predetermined by the current pattern
				if (patternsShip[shipEnemies[i].pattern].nextPattern == -1)
				{
					bool isGood = true;
					do
					{
						//Select random pattern from array.
						newPattern = rand()%16;
						if (level == 1 && newPattern == 12)
						{
							// If level 1 and pattern 12 makes it less likely to happen.
							if (rand()%2 == 0)
							{
								isGood = false;
							}
							
						}
						// If level is larger than 2 increase the chance for pattern 12.
						if (level > 2)
						{
							int chanceForSpecialAttack = 10-level;

							if (level>5)
							{
								chanceForSpecialAttack = 4;
							}
							if (rand()%chanceForSpecialAttack == 0)
							{
								newPattern = 12;
							}			
						}
						// Repeat this until a patter is found that is eligible and passes trought the ifs.
					} while (!patternsShip[newPattern].isEligible && isGood);
				}else{
					//Select predetermined pattern
					newPattern = patternsShip[shipEnemies[i].pattern].nextPattern;
				}
				// Make pattern 12 always aggresive.
				if (newPattern == 12)
				{
					shipEnemies[i].isAggresive = true;
				}
				
				// Put the pattern info inside the player with start values.
				shipEnemies[i].pattern = newPattern;
				shipEnemies[i].rotations = 0;
				shipEnemies[i].speed = patternsShip[newPattern].speed;
				shipEnemies[i].max_rotations =  patternsShip[newPattern].max_rotations;
				// Make it move.
				shipEnemies[i].isMoving = true;

				//If movement pattern is spline type.
				if (shipEnemies[i].pattern == 4 || shipEnemies[i].pattern == 5 || shipEnemies[i].pattern == 9 || shipEnemies[i].pattern == 11
							|| shipEnemies[i].pattern == 11 || shipEnemies[i].pattern == 13 || shipEnemies[i].pattern == 15 || shipEnemies[i].pattern == 17)
					{
						// Ref rotation is the start position of the pattern.					
						shipEnemies[i].ref_rotation.x = shipEnemies[i].st.x;
						shipEnemies[i].ref_rotation.y = shipEnemies[i].st.y;
						shipEnemies[i].rotations += shipEnemies[i].speed;
						
						// Move the enemy.
						shipEnemies[i].st.x = shipEnemies[i].ref_rotation.x + (patternsShip[shipEnemies[i].pattern].rotation_center_offset.x) * shipEnemies[i].rotations;
						shipEnemies[i].st.y = shipEnemies[i].ref_rotation.y + (patternsShip[shipEnemies[i].pattern].rotation_center_offset.y) * shipEnemies[i].rotations;
						// Change angle based on movement direction.
						shipEnemies[i].st.angle = atan2(patternsShip[shipEnemies[i].pattern].rotation_center_offset.y, patternsShip[shipEnemies[i].pattern].rotation_center_offset.x) - k_PI/2 + k_PI;

					}else{
						// Ref rotation is the center of the rotation.
						shipEnemies[i].ref_rotation.x = patternsShip[newPattern].rotation_center_offset.x + shipEnemies[i].st.x;
						shipEnemies[i].ref_rotation.y = patternsShip[newPattern].rotation_center_offset.y + shipEnemies[i].st.y;

						// Rotate enemy using rotate funcion.
						rotate(&shipEnemies[i].st, shipEnemies[i].ref_rotation.x,shipEnemies[i].ref_rotation.y,shipEnemies[i].speed,true);
						shipEnemies[i].rotations += shipEnemies[i].speed;
					}
				

			}
			//Collisions

			//Enemy and window colision.
			Thitbox enemy = getHitboxFromSprite(g_spr_ship_green[0],shipEnemies[i].st.x,shipEnemies[i].st.y,5, true);
			Thitbox window = {0, g_trueWidth+0.0f, 0 , g_trueHeight+0.0f};
			
			//Checks if enemy colides with player on most patterns excepts the ones invisible.
			if (shipEnemies[i].pattern != 13  && shipEnemies[i].pattern != 17)
			{
				checkColisionEnemyPlayer(enemy);
			}
			
			//Check if enemy is out of screen and makes it appear again. Its not instant to make it more natural.
			if (!checkHitBox(enemy,window)){
				int random =rand()%10;
				if (random == 3)
				{
					//Reset values.
					shipEnemies[i].isAppearing = true;
					shipEnemies[i].isAggresive = false;
					
					shipEnemies[i].st.x = g_trueWidth*0.35 + rand()%g_trueWidth*0.35;
					shipEnemies[i].st.y = g_trueHeight*0.2 + rand()%g_trueHeight*0.2;
					
					shipEnemies[i].rotations = 0;
					shipEnemies[i].isMoving = true;	
					shipEnemies[i].st.angle = 0;

					// New ref rotation
					shipEnemies[i].ref_rotation.x = patternsShip[shipEnemies[i].pattern].rotation_center_offset.x + shipEnemies[i].st.x;
					shipEnemies[i].ref_rotation.y = patternsShip[shipEnemies[i].pattern].rotation_center_offset.y + shipEnemies[i].st.y;
				}
			}
			// Checks colision with player bullets. If colision make them explode.
			for (int j = 0; j < 5; j++){
					if (players[g_playerid].bullets[j].isActive)
					{
					
						Thitbox bullet = {players[g_playerid].bullets[j].x-6,
							(players[g_playerid].bullets[j].x+g_bulletWidth+6),
							players[g_playerid].bullets[j].y+g_bulletHeight,
							players[g_playerid].bullets[j].y};

						if (checkHitBox(enemy, bullet))
						{
							players[g_playerid].score += 60; //SCORE
							shipEnemies[i].isAggresive = false;
							shipEnemies[i].isExploding = true;
							players[g_playerid].bullets[j].isActive = false;	
						}
					}
				}
			// If ship is not appearing and pattern is not an invisible one.
			if (!shipEnemies[i].isAppearing && shipEnemies[i].pattern != 13  && shipEnemies[i].pattern != 17)
			{
				// If exploding paint explosion and add to the counter of the sprite.
				if (shipEnemies[i].isExploding)
				{
				esat::DrawSprite(g_spr_enemy_explosion[shipEnemies[i].explodingCounter], shipEnemies[i].st);
				shipEnemies[i].explodingCounter++;
				
				if (shipEnemies[i].explodingCounter >=4)
				{
					shipEnemies[i].isAlive = false;
					shipEnemies[i].explodingCounter = 0;
					shipEnemies[i].isExploding = false;
				}

				}else{ // IF not exploding. 
					
					int sprite = 0;
					// If aggressive check patterns for the one of the special attack to select sprites for animation
					if (shipEnemies[i].isAggresive)
					{
						if (shipEnemies[i].pattern == 12)
						{
							sprite =  (shipEnemies[i].rotations)/2.6;
							sprite += 7;
						}
						if (shipEnemies[i].pattern == 14)
						{
							sprite =  (shipEnemies[i].rotations)/22;
							sprite += 7;
						}
						if (shipEnemies[i].pattern == 15)
						{
							sprite =  11;
						}
						if (shipEnemies[i].pattern == 16)
						{
							sprite = (shipEnemies[i].rotations)/25;
							sprite += 12;
						}
					}
					// Paint enemy based on sprite variable.
					if(color == Green){
						esat::DrawSprite(g_spr_ship_green[sprite], shipEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_ship_blue[sprite], shipEnemies[i].st);
					}
				}	
			}
		}
	}
	if (enemyCounter<=0){ //Finish level if no more alive enemies
		printf("\n - Stage Complete! - ");
		resetStartEnemyPositions();

		g_levelState = Pre;

		if (g_gameManager == ShipsGreen){
			g_splineStep = 0;
			g_gameManager = ShipsBlue;
		}else{
			g_gameManager = MeteorGreen;
		}
	}
}
// Pos used after colision with enemies.
void shipLevelPos(enum enemyColor color){
	// Paint player explosion.
	if (players[g_playerid].isExploding)
	{
		paintPlayer(false);
	}else{// After player explosion

		// onColision does the changes to the ship.
		onColision();

		// Enemies are reset withouth reviving
		int arrayOffSet = 0;
		if (color == Blue){ arrayOffSet = 8; }
		for ( int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
			//Out of screen
			shipEnemies[i].st.x = g_trueWidth*2;
			shipEnemies[i].isAggresive = false;
			
		}
		// Goes back to midLevel
		g_levelState = OnGoing;	
	}
}
// Ships enemy level manager.
void shipsLevel(enum enemyColor color){
	
	switch (g_levelState)
	{
	case Pre:
		shipLevelPre(color);
		break;
	case OnGoing:
		shipLevelOngoing(color); 
		break;
	case Pos:
		shipLevelPos(color);
		break;
	default:
		printf("ERROR EYE LEVEL STATE!");
		break;
	}
	gameUi();
}

// Pre level starts the level with the enemy animation.
void meteorLevelPre(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Counter for Alive enemies
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	// Meteor spline movement.
	g_splineStep += 0.016;

	for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
		//If meteor is aggresive move it accross the window. 
		if (meteorEnemies[i].isAggresive)
		{
			int sprite = 0;
			
			// new position = original position + (end - original position) * step
			meteorEnemies[i].st.x = meteorEnemies[i].start_pos.x + (meteorEnemies[i].ref_rotation.x - meteorEnemies[i].start_pos.x) * g_splineStep;
			meteorEnemies[i].st.y = meteorEnemies[i].start_pos.y + (meteorEnemies[i].ref_rotation.y - meteorEnemies[i].start_pos.y) * g_splineStep;

			sprite =  8 - 8*g_splineStep * 4;
			if (sprite < 0)
			{
				sprite = 0;
			}
			if (sprite >= 8)
			{
				sprite = 7;
			}
			
			//If animation finishes make the next 2 meteors aggressive.			
			if (g_splineStep >= 1)
			{
				g_splineStep = 0;
				meteorEnemies[meteorCounter + arrayOffSet].isAlive=false;
				meteorEnemies[meteorCounter+1 + arrayOffSet].isAlive=false;
				meteorCounter += 2;
				
				//If all the meteors have been done go to next level
				if (meteorCounter >= 8)
				{
					printf("\n -- Stage complete --");
					resetStartEnemyPositions();
					g_splineStep = 0;
					g_levelState = Pre;
					meteorCounter = 0;
					if (g_gameManager == MeteorGreen){
						g_gameManager = MeteorBlue;
					}else{
						g_gameManager = Docking;
						nextLevel = PileGreen;
					}
				}
				
				meteorEnemies[meteorCounter + arrayOffSet].isAggresive=true;
				meteorEnemies[meteorCounter +1+ arrayOffSet].isAggresive=true;
			}
			if (meteorEnemies[i].isAlive)
			{
				//Collisions of enemy
				Thitbox enemy = getHitboxFromSprite(g_spr_meteor_green[0],meteorEnemies[i].st.x,meteorEnemies[i].st.y,5, true);

				//Checks if enemy colides with player.
				checkColisionEnemyPlayer(enemy);
				
				// Checks colision with player bullets. If colision make them explode.
				for (int j = 0; j < 5; j++){
					if (players[g_playerid].bullets[j].isActive)
					{
						Thitbox bullet = {players[g_playerid].bullets[j].x,
						(players[g_playerid].bullets[j].x+g_bulletWidth+2),
						players[g_playerid].bullets[j].y+g_bulletHeight,
						players[g_playerid].bullets[j].y};
						
						// IF there is a colision
						if (checkHitBox(enemy, bullet))
						{
							players[g_playerid].score += 200; //SCORE
							meteorEnemies[i].isExploding = true;
							players[g_playerid].bullets[j].isActive = false;
						}
					}
				}
				// If exploding paint explosion and add to the counter of the sprite.
				if (meteorEnemies[i].isExploding)
				{
				
				esat::DrawSprite(g_spr_meteor_explosion[meteorEnemies[i].explodingCounter], meteorEnemies[i].st);
				meteorEnemies[i].explodingCounter++;
				
				if (meteorEnemies[i].explodingCounter >=8)
				{
					meteorEnemies[i].isAlive = false;
					meteorEnemies[i].explodingCounter = 0;
					meteorEnemies[i].isExploding = false;
				}

				}else{// IF not exploding paint enemy normally.
					
					if(color == Green){
						esat::DrawSprite(g_spr_meteor_green[sprite], meteorEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_meteor_blue[sprite], meteorEnemies[i].st);
					}
				}
			}
		}
	}
}
// Pos used after colision with enemies.
void meteorLevelPos(enum enemyColor color){

	// Paint player explosion.
	if (players[g_playerid].isExploding)
	{
		paintPlayer(false);
	}else{// After player explosion

		// onColision does the changes to the ship.
		onColision();
		// Current meteor path is finished
		g_splineStep = 1;
		
		g_levelState = Pre;	
	}
}
// Meteor enemy level manager.
void meteorLevel(enum enemyColor color){
	
	switch (g_levelState)
	{
	case Pre:
		meteorLevelPre(color);
		break;
	case OnGoing:
		break;
	case Pos:
		meteorLevelPos(color);
		break;
	default:
		printf("ERROR EYE LEVEL STATE!");
		break;
	}
	gameUi();
}

// Pre level starts the level with the enemy animation.
void pileLevelPre(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	//Selects randomly a amount of enemies to start aggressive based on the current level
	int startAggresiveCounter = 0;
	do
	{
		for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
			
			pileEnemies[i].isAppearing = true;
			pileEnemies[i].pattern = 1;
			pileEnemies[i].rotations = 0;
			if (startAggresiveCounter < level && !pileEnemies[i].isAggresive)
			{
				if (rand()%4 == 0)
				{
					pileEnemies[i].isAggresive = true;
					pileEnemies[i].isAppearing = false;
					startAggresiveCounter++;
				}			
			}
		}
	} while (startAggresiveCounter < level );
	
	// Go to next level
	g_levelState = OnGoing;
	g_aliveOnLevelTime = esat::Time();
	g_splineStep = 0;

}
// OnGoing is the main gameplay part of the level. Completed after defeating all enemies.
void pileLevelOnGoing(enum enemyColor color){
	// Basic gameplay
	basicGameLoop();
	// Counter for Alive enemies
	int enemyCounter = 0;
	// Offset for array depending on color.
	int arrayOffSet = 0;
	if (color == Blue) { arrayOffSet = 8; }
	//If time in level is less than 2secs add to the spline
	if (esat::Time() - g_aliveOnLevelTime < 2000)
	{
		g_splineStep += 0.05;
	}else{
		g_splineStep = 0;
	}
	
	for (int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){

		if (pileEnemies[i].isAlive)
		{
			//Adds 1 to alive enemies.
			enemyCounter+=1;
			// If enemy is aggresive move it down
			if (pileEnemies[i].isAggresive)
			{

				pileEnemies[i].st.y += 4*k_scale;

			}else if (pileEnemies[i].isAppearing)
			{
				// If appearing true 

				// If time in level is less than 2secs move them down with g_spline 
				if (esat::Time() - g_aliveOnLevelTime < 2000)
				{
					pileEnemies[i].st.y = pileEnemies[i].start_pos.y + (pileEnemies[i].start_pos.y+g_trueHeight*0.3 - pileEnemies[i].start_pos.y) * g_splineStep;
				}else{ // If not move it with inside enemy variable rotations
					pileEnemies[i].st.y = pileEnemies[i].start_pos.y + (pileEnemies[i].start_pos.y+g_trueHeight*0.3 - pileEnemies[i].start_pos.y) * pileEnemies[i].rotations;
					pileEnemies[i].rotations += 0.05;
				}
				//If g_splieStep is completed complete inside enemy one also.
				if (g_splineStep >= 1)
				{
					pileEnemies[i].rotations = 1;
				}
				
				//If rotations is finished make them not be appearing anymore
				if (pileEnemies[i].rotations >= 1)
				{
					pileEnemies[i].isAppearing = false;
					//If time is less than 3 seconds insert pattern 1.
					if (esat::Time() - g_aliveOnLevelTime < 3000)
					{
						pileEnemies[i].speed = patternsPile[1].speed;
						pileEnemies[i].max_rotations =  patternsPile[1].max_rotations;
						pileEnemies[i].isMoving = true;
						
						pileEnemies[i].ref_rotation.x = patternsPile[1].rotation_center_offset.x + pileEnemies[i].st.x;
						pileEnemies[i].ref_rotation.y = patternsPile[1].rotation_center_offset.y + pileEnemies[i].st.y;
					}else{ // If more than 3 secs moving is false
						pileEnemies[i].isMoving = false;
					}
				}
			}else{ // If not aggressive and not appearing
				
				// Adds enemy speed to rotations but always positive.
				if (pileEnemies[i].speed < 0){
					pileEnemies[i].rotations += -pileEnemies[i].speed;
				}else{
					pileEnemies[i].rotations += pileEnemies[i].speed;
				}

				if (pileEnemies[i].isMoving)
				{
					// If Spline type patterns
					if (pileEnemies[i].pattern == 2)
					{
						// Moves enemy based on pattern. This only works for spline type patterns.
						pileEnemies[i].st.y = pileEnemies[i].ref_rotation.y + (patternsPile[pileEnemies[i].pattern].rotation_center_offset.y) * pileEnemies[i].rotations;
					}else{// If not a spline pattern
						
						//Rotates enemy based on pattern using rotate function.
						rotate(&pileEnemies[i].st,
							pileEnemies[i].ref_rotation.x,pileEnemies[i].ref_rotation.y,pileEnemies[i].speed,false);
					
					}
					//Random chance to go aggressive based on level
					int chance = rand()%(350-level*2);
					if (chance == 0)
					{
						pileEnemies[i].isAggresive = true;
					}	

					if (pileEnemies[i].max_rotations < pileEnemies[i].rotations)
					{
						pileEnemies[i].rotations = 0;	
						pileEnemies[i].isMoving = false;
					}
					
				}else{
					//New movement
					int newPattern;

					// If next pattern is not predetermined by the current pattern
					if (patternsPile[pileEnemies[i].pattern].nextPattern == -1){
						bool isGood = true;
						do
						{
							//Select random pattern from array.
							newPattern = rand()%3;
							if (newPattern == 2)
							{
								if (rand()%4 != 0)
								{
									isGood = false;
								}
							}
							// Repeat this until a patter is found that is eligible
						} while (!patternsPile[newPattern].isEligible && isGood);
					}else{
						//Select predetermined pattern
						newPattern = patternsPile[pileEnemies[i].pattern].nextPattern;	
						
					}
					//Make pattern 2 randomly possible no matter the current pattern.
					if (rand()%10 == 0)
					{
						newPattern = 2;
					}
					// Put the pattern info inside the player with start values.
					pileEnemies[i].pattern = newPattern;
					pileEnemies[i].rotations = 0;
					pileEnemies[i].speed = patternsPile[newPattern].speed;
					pileEnemies[i].max_rotations =  patternsPile[newPattern].max_rotations;
					pileEnemies[i].isMoving = true;

					//If movement pattern is NOT spline type.
					if (newPattern != 2)
					{
						
						pileEnemies[i].ref_rotation.x = patternsPile[newPattern].rotation_center_offset.x + pileEnemies[i].st.x;
						pileEnemies[i].ref_rotation.y = patternsPile[newPattern].rotation_center_offset.y + pileEnemies[i].st.y;
						// Rotate enemy using rotate funcion.
						rotate(&pileEnemies[i].st, pileEnemies[i].ref_rotation.x,pileEnemies[i].ref_rotation.y,pileEnemies[i].speed,false);
							pileEnemies[i].rotations += pileEnemies[i].speed;

					}else{

						pileEnemies[i].ref_rotation.x = pileEnemies[i].st.x;
						pileEnemies[i].ref_rotation.y = pileEnemies[i].st.y;
						pileEnemies[i].rotations += pileEnemies[i].speed;
					// Move the enemy.
						pileEnemies[i].st.x = pileEnemies[i].ref_rotation.x + (patternsPile[pileEnemies[i].pattern].rotation_center_offset.x) * pileEnemies[i].rotations;
						pileEnemies[i].st.y = pileEnemies[i].ref_rotation.y + (patternsPile[pileEnemies[i].pattern].rotation_center_offset.y) * pileEnemies[i].rotations;
						
					}
				}
			}
			
			//Collisions

			//Enemy and window colision.
			Thitbox enemy = getHitboxFromSprite(g_spr_pile_green[0],pileEnemies[i].st.x,pileEnemies[i].st.y,5, true);
			Thitbox window = {0, g_trueWidth+0.0f, 0 , g_trueHeight+0.0f};
			
			//Checks if enemy colides with player.
			checkColisionEnemyPlayer(enemy);

			//Check if enemy is out of screen and makes it appear again. Its not instant to make it more natural.
			if (!checkHitBox(enemy,window)){
				int random =rand()%10;
				if (random == 3)
				{
					//Reset values.
					pileEnemies[i].isAppearing = true;	
					// New position for appearing
					pileEnemies[i].st.y = pileEnemies[i].start_pos.y;
					
					if (esat::Time() - g_aliveOnLevelTime > 6000)
					{
						pileEnemies[i].st.x = g_trueWidth*0.1+rand()%g_trueWidth*0.8;
					}
					
					pileEnemies[i].rotations = 0;
					pileEnemies[i].isMoving = true;	
					pileEnemies[i].st.angle = 0;
				}
			}
			// Checks colision with player bullets. If colision make them explode.
			for (int j = 0; j < 5; j++){
					if (players[g_playerid].bullets[j].isActive)
					{
					
						Thitbox bullet = {players[g_playerid].bullets[j].x,
						(players[g_playerid].bullets[j].x+g_bulletWidth+2),
						players[g_playerid].bullets[j].y+g_bulletHeight,
						players[g_playerid].bullets[j].y};
						
						// IF there is a colision
						if (checkHitBox(enemy, bullet))
						{
							players[g_playerid].score += 100; // SCORE
							pileEnemies[i].isAggresive = false;
							pileEnemies[i].isExploding = true;
							players[g_playerid].bullets[j].isActive = false;
							pileEnemies[i].explodingCounter = 0;
						}
					}
				}
			// If exploding paint explosion and add to the counter of the sprite.
			if (pileEnemies[i].isExploding)
			{
				esat::DrawSprite(g_spr_enemy_explosion[pileEnemies[i].explodingCounter], pileEnemies[i].st);
				pileEnemies[i].explodingCounter++;
				
				if (pileEnemies[i].explodingCounter >=4)
				{
					pileEnemies[i].isAlive = false;
					pileEnemies[i].explodingCounter = 0;
					pileEnemies[i].isExploding = false;
				}

				
			}else{  // IF not exploding paint enemy normally.
				
				//Paint enemy in aggressive form
				if (pileEnemies[i].isAggresive)
				{ 
					//Animation 
					if (pileEnemies[i].explodingCounter < 14)
					{
						pileEnemies[i].explodingCounter++;
					}

					if(color == Green){
						esat::DrawSprite(g_spr_pile_green[pileEnemies[i].explodingCounter/2], pileEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_pile_blue[pileEnemies[i].explodingCounter/2], pileEnemies[i].st);
					}

				}else{
					if(color == Green){
						esat::DrawSprite(g_spr_pile_green[0], pileEnemies[i].st);
					}else{
						esat::DrawSprite(g_spr_pile_blue[0], pileEnemies[i].st);
					}
				}
			}
		}
	}
	if (enemyCounter<=0){ //Finish level if no more alive enemies
		printf("\n - Stage Complete! - ");
		resetStartEnemyPositions();
		g_levelState = Pre;
		if (g_gameManager == PileGreen){
			g_splineStep = 0;
			g_gameManager = PileBlue;
		}else{
			g_gameManager = EndAnim;
			
		}
	}
}
// Pos used after colision with enemies.
void pileLevelPos(enum enemyColor color){
	
	// Paint player explosion.
	if (players[g_playerid].isExploding)
	{
		paintPlayer(false);
	}else{ // After player explosion
		
		// onColision does the changes to the ship.
		onColision();
		// Enemies are reset withouth reviving
		int arrayOffSet = 0;
		if (color == Blue){ arrayOffSet = 8; }
		for ( int i = 0+arrayOffSet; i < 8+arrayOffSet; i++){
			//Out of screen
			pileEnemies[i].st.x = g_trueWidth*2;
			pileEnemies[i].isAggresive = false;
			pileEnemies[i].isAppearing = true;
			pileEnemies[i].pattern = 1;
			pileEnemies[i].rotations = 0;
		}
		// Goes back to the start
		g_levelState = Pre;	
	}
}
// Pile enemy level manager.
void pileLevel(enum enemyColor color){
	
	
	switch (g_levelState)
	{
	case Pre:
		pileLevelPre(color);
		break;
	case OnGoing:
		pileLevelOnGoing(color);
		break;
	case Pos:
		pileLevelPos(color);
		break;
	default:
		printf("ERROR EYE LEVEL STATE!");
		break;
	}
	gameUi();
}

// Game Over level. Shows defeat message and goes to start menu or next player if 2 players.
void gameOver(){
	if (esat::Time() - g_aliveOnLevelTime > 4000)
	{
		if (twoPlayers && g_playerid == 0)
		{
			g_levelState = Pre;
			g_gameManager = EyeGreen;
			g_playerid = 1;
		}else{
			g_playerid = 0;
			g_levelState = Pre;
			g_gameManager = Menu;
			players[0].score = 0;
			players[1].score = 0;
		}
		resetPlayer();
		resetStartEnemyPositions();
		initPlayerStruct();
	}
	
	esat::DrawSetFillColor(250,80,80);
	esat::DrawText(80*k_scale,60*k_scale,"GAME     OVER");
}


int esat::main(int argc, char **argv) {

	srand(time(NULL));

	//Windoow config
	g_trueHeight = k_height*k_scale;
	g_trueWidth = k_width*k_scale;
	g_bulletHeight = 4*k_scale;
	g_bulletWidth = 1*k_scale;
	g_playerSpeed = 4*k_scale;
	g_aliveOnLevelTime = esat::Time(); //Remember to reset before starting new stage!
	
	esat::WindowInit(g_trueWidth, g_trueHeight);
	esat::DrawSetTextFont("./resources/fonts/font.ttf"); //Get font.
	WindowSetMouseVisibility(true);
  
	//Gamestate
	g_gameManager = Menu;
	
	initSprites();
	initSounds();
	initPlayerStruct();
	initEnemiesStruct();
	generateBackground();
	resetStartEnemyPositions();
	initPatterns();

    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
			esat::DrawClear(3,3,3);

			//system("cls");	

			paintBackgroundStars();
			
			switch (g_gameManager)
			{
			case Menu:
				menu();
				break;
			case Scoreboard:
				scoreboard();
				break;
			case Docking:
				docking();
				break;
			case StartAnim:
				startAnim();
				break;
			case EndAnim:
				endAnim();
				break;
			case EyeGreen:
				eyeLevel(Green);
				break;
			case EyeBlue:
				eyeLevel(Blue);
				break;
			case FlyGreen:
				flyLevel(Green);
				break;
			case FlyBlue:
				flyLevel(Blue);
				break;
			case ShipsGreen:
				shipsLevel(Green);
				break;
			case ShipsBlue:
				shipsLevel(Blue);
				break;
			case MeteorGreen:
				meteorLevel(Green);
				break;
			case MeteorBlue:
				meteorLevel(Blue);
				break;
			case PileGreen:
				pileLevel(Green);
				break;
			case PileBlue:
				pileLevel(Blue);
				break;
			case GameOver:
				gameOver();
				break;
			default:
				printf("Error Game Manager!");
				break;
			}

    	esat::DrawEnd();
    	//FPS CONTROL
    	do{
    		current_time = esat::Time();
    	}while((current_time-last_time)<=1000.0/fps);
    	esat::WindowFrame();
  }
  releaseSprites();
  esat::WindowDestroy();
  return 0;
}
// FINALLY DONE! Although not perfect... Far from it.