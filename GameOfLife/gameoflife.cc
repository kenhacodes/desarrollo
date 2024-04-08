#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct Tcell
{
	bool isAlive;
	int adjCounter = 0;
};

Tcell *cells;
int cellDimension;

int *survivingRate;
int *birthRate;
int survRateNum = 0, birthRateNum = 0;

float kwidth = 900, kheight = 900;
float cellwidth;

float view_min_x, view_min_y, view_max_x, view_max_y; //old experiment doesnt really zoom 
float zoomheight = 1.0f, zoomwidth = 1.0f;
float zoom_offset_x = 0.0f, zoom_offset_y = 0.0f;
esat::Vec2 originMouse, endMouse;
bool clickDown = false;
float cameraSpeed = 4.5f;

bool isPlaying = false;

unsigned char fps=30; //Control de frames por segundo
double current_time,last_time;

esat::Vec2 globalToLocal(esat::Vec2 input){
	esat::Vec2 res;
	res.x = input.x / kwidth;
	res.y = input.y / kheight;
	return res;
}

esat::Vec2 localToGlobal(esat::Vec2 input){
	esat::Vec2 res;
	res.x = view_min_x + input.x * (view_max_x - view_min_x); 
	res.y = view_min_y + input.y * (view_max_y - view_min_y); 
	return res;
}

void initParams(char *param, char *dimension){
	int i = 0;
	
	bool separator = false;
	
	birthRate = (int*)calloc(3, sizeof(int));
	survivingRate = (int*)calloc(3, sizeof(int));

	while (*(param+i) != '\0')
	{
		if(*(param+i) == '/'){
			separator = true;
			printf("\n-------");
		}else{
			if (separator)
			{
				//Birth Rate
				*(birthRate + birthRateNum) = atoi((char*)(&(*(param+i))));
				while (*(birthRate + birthRateNum) >=10) *(birthRate + birthRateNum)/=10;				
				printf("\n Birth -> %d", *(birthRate + birthRateNum));
				birthRateNum++;
			}else{
				//Surviving Rate
				*(survivingRate + survRateNum) = atoi((char*)&(*(param+i)));
				while (*(survivingRate + survRateNum) >=10) *(survivingRate + survRateNum)/=10;	
				printf("\n Surv  -> %d", *(survivingRate + survRateNum));
				survRateNum++;
			}
		}
		i++;

		view_min_x = 0;
		view_max_x = kwidth;
		view_min_y = 0;
		view_max_y = kheight;
	}

	printf("\n-------");
	printf("\n Dimension-> %d * %d",atoi(dimension),atoi(dimension));
	cellDimension = atoi(dimension);
	cellwidth = kwidth/cellDimension;
	printf("\n-------");
	printf("\n params initialized\n");

}

void startCells(){
	Tcell cell;
	cells = (Tcell*)malloc(cellDimension*cellDimension*sizeof(Tcell));
	for (int i = 0; i < cellDimension * cellDimension; i++)
	{
		cell.isAlive = false;
		if(rand()%2 == 0){
			cell.isAlive = true;
		}
		(*(cells+i)) = cell;
	}
}

void paintCell(int row, int col){
	
	esat::DrawSetFillColor(200,33,33,255);
	esat::DrawSetStrokeColor(0,0,0,0);

	esat::Vec2 *pos;
	pos = (esat::Vec2*)malloc(4*sizeof(esat::Vec2));
	
	(*(pos+0)).x = (cellwidth*zoomwidth)*col;
	(*(pos+0)).y = (cellwidth*zoomheight)*row;
	(*(pos+0)).x -= zoom_offset_x;
	(*(pos+0)).y -= zoom_offset_y;
	//(*(pos+0)) = localToGlobal(globalToLocal( (*(pos+0))  ));

	(*(pos+1)).x = (cellwidth*zoomwidth)*col+(cellwidth*zoomwidth);
	(*(pos+1)).y = (cellwidth*zoomheight)*row;
	(*(pos+1)).x -= zoom_offset_x;
	(*(pos+1)).y -= zoom_offset_y;
	//(*(pos+1)) = localToGlobal(globalToLocal( (*(pos+1)) ));

	(*(pos+2)).x = (cellwidth*zoomwidth)*col+(cellwidth*zoomwidth);
	(*(pos+2)).y = (cellwidth*zoomheight)*row+(cellwidth*zoomheight);
	(*(pos+2)).x -= zoom_offset_x;
	(*(pos+2)).y -= zoom_offset_y;
	//(*(pos+2)) = localToGlobal(globalToLocal( (*(pos+2))) );

	(*(pos+3)).x = (cellwidth*zoomwidth)*col;
	(*(pos+3)).y = (cellwidth*zoomheight)*row+(cellwidth*zoomheight);
	(*(pos+3)).x -= zoom_offset_x;
	(*(pos+3)).y -= zoom_offset_y;
	//(*(pos+3)) = localToGlobal(globalToLocal( (*(pos+3))) );

	esat::DrawSolidPath(&pos[0].x, 4);

	free(pos);
	pos = nullptr;
}

void checkAdj(int row, int col){
	(*(cells + (row * cellDimension + col))).adjCounter = 0;
	for (int rowi = -1; rowi < 2; rowi++)
	{
		for (int coli = -1; coli < 2; coli++)
		{	
			if( (row * cellDimension + col) != ((row+rowi) * cellDimension + (col+coli)) && 
				(((row+rowi) * cellDimension) + (col+coli)) > 0 &&
				(((row+rowi) * cellDimension) + (col+coli)) <= cellDimension*cellDimension)
			{	
				if ((*(cells + ((row+rowi) * cellDimension + (col+coli)))).isAlive)
				{
					(*(cells + (row * cellDimension + col))).adjCounter+=1;
				}
			}
		}
	}
	
	//printf("\n -> id:%d adj: %d",(row * cellDimension + col), (*(cells + (row * cellDimension + col))).adjCounter);
}

void edit(){
	float mousex = esat::MousePositionX();
	float mousey = esat::MousePositionY();
	int row, col;
	if (esat::MouseButtonPressed(0) && !esat::IsSpecialKeyPressed(esat::kSpecialKey_Shift)) //Click IZQ PAINT
	{
		row = (cellDimension - 1) / (kheight / (mousey + (cellwidth/2)));
		col = (cellDimension - 1) / (kwidth / (mousex + (cellwidth/2)));

		(*(cells + (row * cellDimension + col))).isAlive = true;
	}
	if (esat::MouseButtonPressed(1) && !esat::IsSpecialKeyPressed(esat::kSpecialKey_Shift)) //Click IZQ DELETE
	{
		row = (cellDimension - 1) / (kheight / (mousey + (cellwidth/2)));
		col = (cellDimension - 1) / (kwidth / (mousex + (cellwidth/2)));

		(*(cells + (row * cellDimension + col))).isAlive = false;
	}
}

void play(){
	for (int row = 0; row < cellDimension; row++)
	{
		for (int col = 0; col < cellDimension; col++)
		{
			if ((*(cells + (row*cellDimension+col))).isAlive)
			{
				paintCell(row, col);
			}
			if (isPlaying)
			{
				checkAdj(row, col);
			}
		}
	}
}

void deathAndRebirth(){
	bool getsToLiveAnotherDay;

	for (int i = 0; i < cellDimension*cellDimension; i++)
	{
		if((*(cells + i)).isAlive){
			getsToLiveAnotherDay = false;
			for (int j = 0; j < survRateNum; j++)
			{
				if ((*(cells + i)).adjCounter == (*(survivingRate + j))) 
				{
					getsToLiveAnotherDay = true;	
				}
			}
			if (!getsToLiveAnotherDay){
				(*(cells + i)).isAlive = false;
			}
		}else{
			for (int j = 0; j < birthRateNum; j++)
			{
				if ((*(cells + i)).adjCounter == (*(birthRate + j)))
				{
					(*(cells + i)).isAlive = true;
				}
			}
		}
	}
}

void zoom(){
	esat::DrawSetFillColor(33,33,255,50);
	esat::DrawSetStrokeColor(0,0,255,255);

	esat::Vec2 *pos;

	if (esat::MouseButtonDown(0))
	{
		clickDown = true;
		originMouse.x = esat::MousePositionX();
		originMouse.y = esat::MousePositionY();
	}

	if (esat::MouseButtonDown(1)){
		view_min_x = 0;
		view_max_x = kwidth;
		view_min_y = 0;
		view_max_y = kheight;
		zoomheight = 1.0f;
		zoomwidth = 1.0f;
		zoom_offset_x = 0.0f;
		zoom_offset_y = 0.0f;
	}

	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left)) zoom_offset_x-=cameraSpeed;
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right)) zoom_offset_x+=cameraSpeed;
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up)) zoom_offset_y-=cameraSpeed;
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down)) zoom_offset_y+=cameraSpeed;

	if (esat::MouseButtonPressed(0))
	{
		pos = (esat::Vec2*)malloc(4*sizeof(esat::Vec2));
	
		(*(pos+0)).x = originMouse.x;
		(*(pos+0)).y = originMouse.y;

		(*(pos+1)).x = esat::MousePositionX();
		(*(pos+1)).y = originMouse.y;
	
		(*(pos+2)).x = esat::MousePositionX();
		(*(pos+2)).y = esat::MousePositionY();

		(*(pos+3)).x = originMouse.x;
		(*(pos+3)).y = esat::MousePositionY();
		
		esat::DrawSolidPath(&pos[0].x, 4);
	}
	
	if (esat::MouseButtonUp(0) && clickDown)
	{
		clickDown = false;
		endMouse.x = esat::MousePositionX();
		endMouse.y = esat::MousePositionY();

		view_min_x = originMouse.x;
		view_max_x = endMouse.x;
		view_min_y = originMouse.y;
		view_max_y = endMouse.y;

		zoom_offset_x += originMouse.x;
		zoom_offset_y += originMouse.y;

		zoomwidth += (endMouse.x - originMouse.x) / kwidth;
		zoomheight += (endMouse.y - originMouse.y) / kheight;
		zoomheight++;
		zoomwidth++;
	}
}

int esat::main(int argc, char **argv) {

	srand(time(NULL));

	if(argc == 3){
		
		initParams(*(argv+1), *(argv+2));
	
	}else{
		printf("\nNo se han introducido los valores correctamente. \n Valores iniciados por defecto.");
	}

	startCells();

	//printf("\nParams: Cells:%d\n",cellDimension*cellDimension);

	esat::WindowInit(kwidth,kheight);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

			if (esat::IsSpecialKeyUp(kSpecialKey_Space)) isPlaying = !isPlaying;
			play();
			if (isPlaying) deathAndRebirth();
			edit();

			if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Shift)) zoom();
			if (esat::MouseButtonUp(0)) clickDown = false;

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
