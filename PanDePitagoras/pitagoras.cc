#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <math.h>

#include <stdio.h>

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;

float rad;
float h;
float co;
float cc;
float diff;

const int kNPoints = 50;
float g_circle[kNPoints * 2];

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);

	rad = 100.0f;
	h = 1.0f;
	co = 1.0f;
	cc = 0.0f;
	
	diff = (h*2.0f)/ (float) kNPoints;

	for (int i = 0; i < kNPoints; ++i)
	{
		cc = sqrt((h*h)-(co*co));
		
		//Y height -> co
		g_circle[i*2] =  sqrt((h*h)-(cc*cc)) *rad + 400.0f;
		
		//X width -> cc
		g_circle[i*2+1] = cc *rad + 300.0f; 
		
		g_circle[i*2+kNPoints/2] = -(sqrt((h*h)-(cc*cc)) *rad + 400.0f);

		g_circle[i*2+1+kNPoints/2] = -cc*rad + 300.0f;

		

		co-=diff;
		
		printf("\n%02d-> x:%3.2f y:%3.2f cc:%3.2f co:%3.2f addition:%1.2f",i,g_circle[i*2],g_circle[i*2+1],cc,co,diff);
		printf("\n%02d-> x:%3.2f y:%3.2f cc:%3.2f co:%3.2f addition:%1.2f",i,g_circle[i*2+kNPoints/2],g_circle[i*2+1+kNPoints/2],cc,co,diff);
	}
	

    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);

			esat::DrawSetStrokeColor((cos(esat::Time() * 0.01f) * 0.5f + 0.5f) * 255.0f,
                             		(cos(esat::Time() * 0.02f) * 0.5f + 0.5f) * 255.0f,
                             		(cos(esat::Time() * 0.03f) * 0.5f + 0.5f) * 255.0f, 255);
    	esat::DrawSetFillColor(255, 255, 255, 0);
    	esat::DrawSolidPath(g_circle, kNPoints*2);

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
