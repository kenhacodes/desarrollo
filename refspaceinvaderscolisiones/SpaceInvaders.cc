#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Constantes
const int SIZEX = 256, SIZEY = 224, SCALE_FACTOR = 2, ENEMIES_QT = 56;

//Time variables
unsigned char fps=25; //Control de frames por segundo
double current_time,last_time; 
double cooldown = -1, timeNow, timeStart, timeExplosion, TimeDeath;	

struct Tcolbox{
	float minx;
	float maxx;
	float miny;
	float maxy;
};

struct TBullet{
	int x, y, type = -1;
	bool isActive = false;
	bool isExplosion = false;
	esat::SpriteHandle sprites[4];
};

struct TBulletPlayer{
	int x, y;
	bool isActive = false;
	bool isExplosion = false;
	esat::SpriteHandle sprite;
};

struct TEnemy {
    int x;
    int y;
    int type;
    int speed;	
    int direccion;
	int sprSel=0;
    bool isAlive = true;
	bool canShoot = false;
	esat::SpriteHandle sprites[2];
	TBullet bullet;
    bool exploded = false;
};

struct TPlayer{
	int x, y = 400;
	esat::SpriteHandle sprite;
    esat::SpriteHandle expSprite;
	TBulletPlayer bullet;
    bool isDead = false;
    bool canShoot = true;
};

//VARIABLES GLOBALES

//Player
int playerSpeed = 10;
TPlayer player;
esat::SpriteHandle playerBulletExplosion;

//Variables enemigos
TEnemy UFO;
TEnemy enemies[5][11];
int foundR, foundC, foundBR, foundBC, globalAceleration=10, shootsCount = 0, bulletSpriteIndex=0, rCont = 4, cCont = 0;
float speed = 0.6;
bool direction = true, check = false;

esat::SpriteHandle zigZagBullet[4],verticalBullet[4],diagonalBullet[4],aSpritesEnemies[6]; 

//SPRITES 
void initSpritesEnemies(){
    int posAdjX = 32,posAdjY = 30;
    // Recoge los sprites
    aSpritesEnemies[0] = esat::SpriteFromFile("./Recursos/sprites/space__0000_A1.png");
    aSpritesEnemies[1] = esat::SpriteFromFile("./Recursos/sprites/space__0001_A2.png");
    aSpritesEnemies[2] = esat::SpriteFromFile("./Recursos/sprites/space__0002_B1.png");
    aSpritesEnemies[3] = esat::SpriteFromFile("./Recursos/sprites/space__0003_B2.png");
    aSpritesEnemies[4] = esat::SpriteFromFile("./Recursos/sprites/space__0004_C1.png");
    aSpritesEnemies[5] = esat::SpriteFromFile("./Recursos/sprites/space__0005_C2.png");

    //disparo de zig zag
    esat::SpriteHandle bulletSheet = esat::SpriteFromFile("./Recursos/sprites/disparoZigZag.png");
    zigZagBullet[0] = esat::SubSprite(bulletSheet,0,0,6,14);
    zigZagBullet[1] = esat::SubSprite(bulletSheet,6,0,6,14);
    zigZagBullet[2] = esat::SubSprite(bulletSheet,12,0,6,14);
    zigZagBullet[3] = esat::SubSprite(bulletSheet,18,0,6,14);

    //disaparo mov vertical
    bulletSheet = esat::SpriteFromFile("./Recursos/sprites/disparo_mov_vertical.png");
    verticalBullet[0] = esat::SubSprite(bulletSheet,0,0,6,14);
    verticalBullet[1] = esat::SubSprite(bulletSheet,6,0,6,14);
    verticalBullet[2] = esat::SubSprite(bulletSheet,12,0,6,14);
    verticalBullet[3] = esat::SubSprite(bulletSheet,18,0,6,14);
    
    //disparo cruzado
    bulletSheet = esat::SpriteFromFile("./Recursos/sprites/disparo_diagonal.png");
    diagonalBullet[0] = esat::SubSprite(bulletSheet,0,0,6,14);
    diagonalBullet[1] = esat::SubSprite(bulletSheet,6,0,6,14);
    diagonalBullet[2] = esat::SubSprite(bulletSheet,12,0,6,14);
    diagonalBullet[3] = esat::SubSprite(bulletSheet,18,0,6,14);

    // Crea los structs de los enemigos en una array 
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 11; j++){
            enemies[i][j] = {j*posAdjX,posAdjY+(i*posAdjX)};
            if(i == 0){
                enemies[i][j].sprites[0] = aSpritesEnemies[0];
                enemies[i][j].sprites[1] = aSpritesEnemies[1];
                enemies[i][j].type = 0;
            } else if(i == 1 || i == 2){
                enemies[i][j].sprites[0] = aSpritesEnemies[2];
                enemies[i][j].sprites[1] = aSpritesEnemies[3];
                enemies[i][j].type = 1;
            } else {
                enemies[i][j].sprites[0] = aSpritesEnemies[4];
                enemies[i][j].sprites[1] = aSpritesEnemies[5];
                enemies[i][j].type = 2;
            }
        }
    }

    esat::SpriteRelease(bulletSheet);
}
void initSprites() {
    // UFO
    UFO.sprites[0] = esat::SpriteFromFile("./Recursos/sprites/space__0007_UFO.png");
    UFO.sprites[1] = esat::SpriteFromFile("./Recursos/sprites/space__0009_EnemyExplosion.png");
	// PLAYER BULLET EXPLOSION
	playerBulletExplosion = esat::SpriteFromFile("./Recursos/sprites/playerProjectileExplosion.png");
	initSpritesEnemies();
}
void ReleaseSprite() {
    // Liberamos memoria ocupada liberando sprites que hayamos cargado nosotros previamente.
    esat::SpriteRelease(UFO.sprites[0]);
    esat::SpriteRelease(UFO.sprites[1]);

	for(int i = 0; i < 5; i++){
        for(int j = 0; j < 11; j++){
            for(int k = 0; k < 2; k++){
                esat::SpriteRelease(enemies[i][j].sprites[k]);
            }
            for(int w = 0; w < 4; i++){
                esat::SpriteRelease(enemies[i][j].bullet.sprites[0]);
            }
        }
    }
    for(int i = 0; i < 6; i++){
        esat::SpriteRelease(aSpritesEnemies[i]);
    }
    for(int i = 0; i < 4; i++){
        esat::SpriteRelease(zigZagBullet[i]);
        esat::SpriteRelease(verticalBullet[i]);
        esat::SpriteRelease(diagonalBullet[i]);
    }
}

//Creador struct plater
void RegisterPlayer(){ 
	player.x = 100;
	player.sprite = esat::SpriteFromFile("./Recursos/sprites/space__0006_Player.png");
    player.expSprite = esat::SpriteFromFile("./Recursos/sprites/space__0010_PlayerExplosion.png");
	player.bullet.sprite = esat::SpriteFromFile("./Recursos/sprites/Projectile_Player.png");
}

//UTILS
bool checkColBox(Tcolbox c1, Tcolbox c2){
	bool res = false;
	if (c1.minx < c2.maxx && c1.maxx > c2.minx && c1.miny < c2.maxy && c1.maxy > c2.miny)
	{
		res = true;
	}
	return res;
	
}
Tcolbox getColboxFromSprite(int x, int y,esat::SpriteHandle sprite){
	Tcolbox res;
	res.minx = x;
	res.maxx = esat::SpriteWidth(sprite) + x;
	res.miny = y;
	res.maxy = esat::SpriteHeight(sprite) + y;
	return res;
}  

//UFO
void SetUFO(){
	UFO.direccion = rand()%2; //Determinamos la direccion del UFO
	UFO.speed = 5;
	// Determinamos la posicion inicial dependiendo de direccion del UFO
	UFO.x = UFO.direccion == 0 ? 0-esat::SpriteWidth(UFO.sprites[0]) : SIZEX*SCALE_FACTOR; 
	UFO.y = 5 + esat::SpriteHeight(UFO.sprites[0]);
}
void MoveUFO(){
  switch(UFO.direccion){
    case 0:
		UFO.x += UFO.speed;
		cooldown = UFO.x >= SIZEX*SCALE_FACTOR ? -1 : cooldown; // cuando la posicion en X sea mayor que WindowX, reiniciamos el cooldown
	break;
    case 1:
		UFO.x -= UFO.speed;
		// cuando la posicion en X sea menor que 0 - dimensiones del sprite, reiniciamos el cooldown
		cooldown = UFO.x <= (0 - esat::SpriteWidth(UFO.sprites[0])) ? -1 : cooldown; 
	break;
  }
}
void StartUFO(){
	// Iniciamos el cooldown en el caso cuando no lo este
	if(cooldown == -1){
		//cooldown = 20 + rand()%71; // random entre 20 y 90
		cooldown =rand()%3;
		timeStart = esat::Time(); // Guardamos el tiempo de inicio de cooldown para calculos posteriores
		SetUFO(); // Determinamos la posicion inicial y direccion del UFO 
	} else {
		timeNow = esat::Time(); // Guardamos el tiempo actual para hacer calculos de cooldown
		if(UFO.isAlive){
			// Si la diferencia entre el tiempo actual y el tiempo de inicio de cooldown es mayor que cooldown, dibujamos el UFO 
            if(timeNow - timeStart >= cooldown*1000){
                esat::DrawSprite(UFO.sprites[0], UFO.x, UFO.y);
                MoveUFO();
            }
		}else {
			if(timeNow - timeExplosion <= 1000.0){
				esat::DrawSprite(UFO.sprites[1], UFO.x, UFO.y);
			}else {
				cooldown = -1;
				UFO.isAlive = true;
			}
		}
	}
}

//ENEMIES
void setBulletSpritesList(TBullet *b,int bulletType){
    switch((*b).type){
        case 0:
            for(int i = 0; i < 4; i ++){
                (*b).sprites[i] = zigZagBullet[i];
            } 
            break;
        case 1:
            for(int i = 0; i < 4; i ++){
                (*b).sprites[i] = verticalBullet[i];
            }  
            break;
        case 2:
            for(int i = 0; i < 4; i ++){
                (*b).sprites[i] = diagonalBullet[i];
            }  
            break;
    }
}
void enemiesSpeedControl(int enemiesDead){
    if(enemiesDead > 3){
        globalAceleration = enemiesDead*4;
    }
}
void drawEnemies(){
    int enemiesDead = 0;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 11; j++){
            if(enemies[i][j].isAlive){//si esta vivo dibujamos al enemigo
                esat::DrawSprite(enemies[i][j].sprites[enemies[i][j].sprSel],enemies[i][j].x,enemies[i][j].y);
            }else{
                enemiesDead += 1;
                if(timeNow - timeExplosion <= 1000.0){
                    if(!enemies[i][j].exploded){
                        esat::DrawSprite(UFO.sprites[1],enemies[i][j].x, enemies[i][j].y);
                    }
			    }else{
                    player.canShoot = true;
                    enemies[i][j].exploded = true;
                    speed = 0.6;
                }
            }
            //disparo del enemigo
            if(enemies[i][j].bullet.isActive){
                //si el disparo esta activo movemos el disparo
                enemies[i][j].bullet.y += 10;//10 por ejemplo
                if(speed <= 0) enemies[i][j].bullet.y = SIZEY * SCALE_FACTOR;
                //si la bala no tiene tipo, le asignamos uno
                if(enemies[i][j].bullet.type == -1){
                    enemies[i][j].bullet.type = rand()%3;
                    setBulletSpritesList(&enemies[i][j].bullet,enemies[i][j].bullet.type);
                }
                //dibujamos el sprite de disparo
                if(speed > 0){
                    esat::DrawSprite(enemies[i][j].bullet.sprites[bulletSpriteIndex],enemies[i][j].bullet.x,enemies[i][j].bullet.y);//seleccionar el tipo de disparo que lanzará el enemigo
                }
                
                //primera version de colision de balas
                if(enemies[i][j].bullet.y >= SIZEY*2){
                    enemies[i][j].bullet.isActive = false;
                    enemies[i][j].canShoot = false;
                    shootsCount--;
                }else if(enemies[i][j].bullet.y + esat::SpriteHeight(enemies[i][j].bullet.sprites[bulletSpriteIndex]) >= player.y){
                    int minX = enemies[i][j].bullet.x;
                    int maxX = enemies[i][j].bullet.x + esat::SpriteWidth(enemies[i][j].bullet.sprites[bulletSpriteIndex]);
                    if(player.x <= maxX && player.x + esat::SpriteWidth(player.sprite) >= minX){
                        player.isDead = true;
                        TimeDeath = esat::Time();
                    }
                }
            }
        }
    }
    enemiesSpeedControl(enemiesDead);
}
void findFirstAliveRight(){
    bool c = false;
    for(int i = 10; i >= 0 ; i--){
        for(int j = 4; j >= 0 ; j--){
            if(enemies[j][i].isAlive && !c){
                foundR = j;
                foundC = i;
                c = true;
            }
        }
    }
}
void findFirstAliveLeft(){
    bool c = false;
    for(int i = 0; i < 11 ; i++){
        for(int j = 4; j >= 0 ; j--){
            if(enemies[j][i].isAlive && !c){
                foundR = j;
                foundC = i;
                c = true;
            }
        }
    }
}
void findFirstAliveBottom(){
    bool c = false;
    for(int j = 4; j >= 0; j--){
        for(int i = 0; i < 11; i++){
            if(enemies[j][i].isAlive && !c){
                foundBR = j;
                foundBC = i;
                c = true;
            }
        }
    }
}
int getSpriteWidth(int type){
    int adjustment = 0;
    switch(type){
        case 0: adjustment = esat::SpriteWidth(aSpritesEnemies[0]); break;
        case 1: adjustment = esat::SpriteWidth(aSpritesEnemies[2]); break;
        case 2: adjustment = esat::SpriteWidth(aSpritesEnemies[4]); break;
    }
    return adjustment;
}
int getSpriteHeight(int type){
    int adjustment = 0;
    switch(type){
        case 0: adjustment = esat::SpriteHeight(aSpritesEnemies[0]); break;
        case 1: adjustment = esat::SpriteHeight(aSpritesEnemies[2]); break;
        case 2: adjustment = esat::SpriteHeight(aSpritesEnemies[4]); break;
    }
    return adjustment;
}
void findShooters(){
    bool shooterAliveFounded = false;
    for(int i = 0; i < 11 ; i++){
        for(int j = 4; j >= 0 ; j--){
            bool b;
            if(j != 4){
                b = enemies[j+1][i].isAlive;
            }else{
                b = false;
            }
            if(enemies[j][i].isAlive && !enemies[j][i].canShoot && !shooterAliveFounded && !b && shootsCount < 3){
                //si esta vivo, lo guardo en la array y salto a la siguiente columna
                
                shooterAliveFounded = true;
                if(rand()%150 == 0){
                    enemies[j][i].bullet.x = enemies[j][i].x + getSpriteWidth(enemies[j][i].type)/2;
                    enemies[j][i].bullet.y = enemies[j][i].y + getSpriteHeight(enemies[j][i].type);
                    enemies[j][i].bullet.isActive = true;
                    enemies[j][i].canShoot = true;
                    shootsCount++;
                }
            }
        }
        shooterAliveFounded = false;
    }
}
void movement(){
    if(!direction){ // Derecha a Izquierda
        // Comprueba si es la fila que tiene que moverse comprobando la primera posición del array
        findFirstAliveLeft();
        findFirstAliveBottom();
        if(rCont == foundBR && cCont == foundBC && enemies[foundR][foundC].x <= 32){
            check = true;
        } else if(!check){ // If de comprobación de que tiene que bajar la fila 
            enemies[rCont][cCont].x-=6*speed;
            if(speed > 0){
                enemies[rCont][cCont].sprSel = !enemies[rCont][cCont].sprSel;
            }
        }

        if(check){
            enemies[rCont][cCont].y+=8;
            enemies[rCont][cCont].x+=1*speed;
            
            if(rCont == 0 && cCont == 10){
                check = false;
                direction = true;
            }
            if(speed > 0){
                enemies[rCont][cCont].sprSel = !enemies[rCont][cCont].sprSel;
            }
        }     
    } else { // Izquierda a Derecha
        // Comprueba si es la fila que tiene que moverse (DESCENDENTE!)
        findFirstAliveRight();
        findFirstAliveBottom();
        if(rCont == foundBR && cCont == foundBC && enemies[foundR][foundC].x >= SIZEX*SCALE_FACTOR - 64){
            check = true;
        } else if(!check){
            enemies[rCont][cCont].x +=6*speed;
            if(speed > 0){
                enemies[rCont][cCont].sprSel = !enemies[rCont][cCont].sprSel;
            }
        }

        if(check){
            enemies[rCont][cCont].y+=8;
            enemies[rCont][cCont].x-=1*speed;

            if(rCont == 0 && cCont == 10){
                direction = false;
                check = false;
            }
            if(speed > 0){
                enemies[rCont][cCont].sprSel = !enemies[rCont][cCont].sprSel;
            }
        }
    }

    cCont++;
    // Mueve las filas y columnas de izq a der y abajo a arriba
    if(cCont > 10){
        cCont = 0;
        rCont--;
        if(rCont < 0) rCont = 4;
    }
}
void bulletSpriteControl(){
    bulletSpriteIndex++;
    if(bulletSpriteIndex > 3){
        bulletSpriteIndex = 0;
    }
}
void killEnemy(){
    bool killed = false;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 11; j++){
            if(enemies[i][j].isAlive && !killed){
                enemies[i][j].isAlive = !enemies[i][j].isAlive;
                killed = true;
            }
        }
    }
}
void movementControl(){
    for(int i = 0; i < globalAceleration; i++){//esta variable aumenta la velocidad del grupo
            movement();
    }
}
void checkUserShootOnEnemies(){
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 11; j++){
            if(enemies[i][j].isAlive){
                Tcolbox enemy = getColboxFromSprite(enemies[i][j].x,enemies[i][j].y, enemies[i][j].sprites[0]);
		        Tcolbox bullet = getColboxFromSprite(player.bullet.x, player.bullet.y,player.bullet.sprite);
		        if (checkColBox(enemy,bullet)&&player.bullet.isActive)
		        {
                    player.bullet.isActive =false;
                    player.canShoot = false;
                    speed = 0;
                    timeExplosion = esat::Time();
                    enemies[i][j].isAlive = false;
		        }
            }
        }
    }
}
void enemyGameLoop(){

    if(!player.isDead){
	    movementControl();
        findShooters();
    }
    drawEnemies();
    bulletSpriteControl();
    checkUserShootOnEnemies();

}

//PLAYER
void updateBullet(TPlayer* player){
	player->bullet.y-=12;
	if (player->bullet.y<=10 && !player->bullet.isExplosion)
	{
		player->bullet.y=10;
		player->bullet.isExplosion=true;
		esat::DrawSprite(playerBulletExplosion, player->bullet.x, player->bullet.y);
	}else if(player->bullet.isExplosion){
		player->bullet.y=10;
		esat::DrawSprite(playerBulletExplosion, player->bullet.x, player->bullet.y);
		player->bullet.isExplosion=false;
		player->bullet.isActive=false;
	}else{
		esat::DrawSprite(player->bullet.sprite, player->bullet.x, player->bullet.y);
	}
}
void playershoot(TPlayer* player){
	player->bullet.isActive=true;
	player->bullet.x=player->x+(esat::SpriteWidth(player->sprite)/2);
	player->bullet.y=player->y-(esat::SpriteWidth(player->bullet.sprite));
}
void playerMovement(){
	if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Left) && !player.isDead){
		if(player.x >= 5) player.x -= playerSpeed;
	}
	if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Right) && !player.isDead){
		if(player.x + esat::SpriteWidth(player.sprite) <= SIZEX*SCALE_FACTOR-5) player.x += playerSpeed	;
	}
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Space) && !player.bullet.isActive && !player.isDead && player.canShoot)
	{
		playershoot(&player);
	}
	
}

//MAIN LOOPS
void gameloop(){
	//Funcion que se encarga de mostar UFO en la pantalla
	StartUFO();
	playerMovement();
	if (player.bullet.isActive){
		updateBullet(&player);
	}
	if (UFO.isAlive)
	{
		Tcolbox ufo = getColboxFromSprite(UFO.x,UFO.y, UFO.sprites[0]);
		Tcolbox bullet = getColboxFromSprite(player.bullet.x, player.bullet.y,player.bullet.sprite);
		if (checkColBox(ufo,bullet)&&player.bullet.isActive)
		{
			player.bullet.isActive =false;
			UFO.speed = 0;
			timeExplosion = esat::Time();
			UFO.isAlive = false;
		}
	}

	enemyGameLoop();
	
    current_time = esat::Time();
    if(current_time - TimeDeath <= 2000.0 && player.isDead){
        esat::DrawSprite(player.expSprite, player.x, player.y);
    }else{
        player.isDead = false;
        esat::DrawSprite(player.sprite, player.x, player.y);
    }
	
}

int esat::main(int argc, char **argv) {

	srand(time(NULL));

	esat::WindowInit(SIZEX*SCALE_FACTOR, SIZEY*SCALE_FACTOR);
	WindowSetMouseVisibility(true);

	initSprites();
	RegisterPlayer();
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time();

    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

		gameloop();

    	esat::DrawEnd();
    	
    	//Control fps 
    	do{
    		current_time = esat::Time();
    	}while((current_time-last_time) <= 1000.0 / fps);
    	
		esat::WindowFrame();
  	}

	ReleaseSprite();
  	esat::WindowDestroy();

  	return 0;
}