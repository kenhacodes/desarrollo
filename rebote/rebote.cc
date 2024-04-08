#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

struct Tball
{
	esat::Vec2 pos;
	esat::Vec2 direction;
	float speed;
	bool isCompressedAnim = false;
};

struct Tmuro
{
	esat::Vec2 inicio, final;
};

esat::SpriteHandle *(ball) = nullptr;
esat::SpriteHandle *(colorBall) = nullptr;

Tball *bolas = nullptr;
Tmuro *muros = nullptr;
unsigned int kwidth = 800, kheight = 600;
unsigned int numeroPelotas = 3;
unsigned int numeroMuros = 2;
int diametro;

unsigned char fps=60; //Control de frames por segundo
double current_time,last_time;

void init(){
	ball = (esat::SpriteHandle*)malloc(2*sizeof(ball));
	*(ball) = esat::SpriteFromFile("./assets/bola1.png");
	*(ball+1) = esat::SpriteFromFile("./assets/bola2.png");
	
	colorBall = (esat::SpriteHandle*)malloc(2*sizeof(colorBall));
	*(colorBall) = esat::SpriteFromFile("./assets/azul.png");
	*(colorBall+1) = esat::SpriteFromFile("./assets/verde.png");
	
	Tball pelota;
	Tmuro muro;
	int halfspriteW = esat::SpriteWidth(*(ball)) / 2;
	int halfspriteH = esat::SpriteHeight(*(ball)) / 2;
	diametro = esat::SpriteHeight(*(ball));
	
	bolas = (Tball*)malloc(numeroPelotas*sizeof(Tball));

	for (int i = 0; i < numeroPelotas; i++)
	{
		pelota.speed = 4+rand()%2;
		pelota.direction.x = ((rand()%100));
		pelota.direction.y = ((rand()%100));
		if (rand()%2 == 0) pelota.direction.x *= -1;
		if (rand()%2 == 0) pelota.direction.y *= -1;
		pelota.direction.x /= 100;
		pelota.direction.y /= 100;
		pelota.pos.x = 2 + (rand()%(kwidth-halfspriteW-2));
		pelota.pos.y = 2 + (rand()%(kheight-halfspriteH-2));

		*(bolas+i) = pelota;
	}

	muros =(Tmuro*)malloc(numeroMuros*sizeof(Tmuro));

	for (int i = 0; i < numeroMuros; i++)
	{
		muro.inicio.x = 60+rand()%(kwidth - 60); 
		muro.inicio.y = 60+rand()%(kheight - 60); 

		muro.final.x = 60+rand()%(kwidth - 60); 
		muro.final.y = 60+rand()%(kheight - 60); 

		*(muros+i) = muro;
	}
	
}

void freeMemory(){
	esat::SpriteRelease(*(ball));
	esat::SpriteRelease(*(ball+1));
	esat::SpriteRelease(*(colorBall));
	esat::SpriteRelease(*(colorBall+1));
	free(ball);
	free(colorBall);
	free(bolas);
	
}

void moverPelota(Tball *pelota){
	float angle = atan2(pelota->direction.y, pelota->direction.x);
	pelota->pos.x += cos(angle) * pelota->speed;
	pelota->pos.y += sin(angle) * pelota->speed;
}

void colisionPelota(Tball *pelota, int id){

	float h;
	float temp;
	float angle;
	float offset;
	
	for (int i = 0; i < numeroPelotas; i++)
	{
		if (id!= i)
		{
			h = sqrt(pow((pelota->pos.x-(*(bolas+i)).pos.x),2) + pow((pelota->pos.y-(*(bolas+i)).pos.y),2));
			
			if (h < diametro-1)
			{
				offset = diametro-h;
				angle = atan2(pelota->pos.y-(*(bolas+i)).pos.y, pelota->pos.x-(*(bolas+i)).pos.x);
				
				pelota->pos.x += cos(angle);
				pelota->pos.y += sin(angle);
				
				temp = (*(bolas+i)).direction.x;
				(*(bolas+i)).direction.x = pelota->direction.x;
				pelota->direction.x = temp;

				temp = (*(bolas+i)).direction.y;
				(*(bolas+i)).direction.y = pelota->direction.y;
				pelota->direction.y = temp;
				
				temp = pelota->speed;
				pelota->speed = (*(bolas+i)).speed;
				(*(bolas+i)).speed = temp;
				
				moverPelota(pelota);
				moverPelota(&(*(bolas+i)));

			}
		}
	}
}

void colisionMuro(Tball *pelota){

	if (pelota->pos.x < 0 || pelota->pos.x > kwidth - esat::SpriteWidth(*(ball)))
	{
		pelota->direction.x *= -1;
		if (pelota->pos.x < 0)
		{
			pelota->pos.x = 1;
		}else{
			pelota->pos.x = kwidth - esat::SpriteWidth(*(ball))-1;
		}

		moverPelota(pelota);
		
	}
	if (pelota->pos.y < 0 || pelota->pos.y > kheight - esat::SpriteHeight(*(ball)))
	{
		pelota->direction.y *= -1;
		if (pelota->pos.y < 0)
		{
			pelota->pos.y = 1;
		}else{
			pelota->pos.y = kheight - esat::SpriteHeight(*(ball))-1;
		}
	
		moverPelota(pelota);
		
	}

	for (int i = 0; i < numeroMuros; i++)
	{
		
		
	}
	
}

void pintarPelota(Tball *pelota, int idPelota){
	int color = 0;
	int animspr = 1;
	
	if (idPelota%2 == 0) color = 1;
	
	if (pelota->isCompressedAnim) animspr = 0;
	
	if (rand()%80 < 1) {
		pelota->isCompressedAnim = !pelota->isCompressedAnim;
	}

	esat::DrawSprite(*(colorBall+color), pelota->pos.x, pelota->pos.y);
	esat::DrawSprite(*(ball+animspr), pelota->pos.x, pelota->pos.y);
}

int esat::main(int argc, char **argv) {

	srand(time(NULL));
	
	if(argc == 4){
		
		kwidth = atoi(*(argv+1));
		kheight = atoi(*(argv+2));
		numeroPelotas = atoi(*(argv+3));
	
	}else{
		printf("No se han introducido los valores correctamente. \n Valores iniciados por defecto.");
	}

	printf("Numero de pelotas: %d", numeroPelotas);

	esat::WindowInit(kwidth,kheight);
	WindowSetMouseVisibility(true);
  
	init();
	esat::DrawSetStrokeColor(200,0,0,250);
	
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

  	last_time = esat::Time(); 
  	esat::DrawBegin();
  	esat::DrawClear(0,0,0);

		for (int i = 0; i < numeroPelotas; i++)
		{
			moverPelota(&(*(bolas+i)));
			colisionPelota(&(*(bolas+i)),i);
			colisionMuro(&(*(bolas+i)));
			pintarPelota(&(*(bolas+i)),i);
		}
		
		for (int i = 0; i < numeroMuros; i++)
		{
			esat::DrawLine((*(muros+i)).inicio.x,(*(muros+i)).inicio.y,(*(muros+i)).final.x,(*(muros+i)).final.y);
		}
		
		if (esat::IsSpecialKeyPressed(kSpecialKey_Space))
		{
			system("cls");
		}

  	esat::DrawEnd();
  	//Control fps 
  	do{
  		current_time = esat::Time();
  	}while((current_time-last_time)<=1000.0/fps);
  	esat::WindowFrame();
  }
  
  freeMemory();
  esat::WindowDestroy();

  
  return 0;
    
}