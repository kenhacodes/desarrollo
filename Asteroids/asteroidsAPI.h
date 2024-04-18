#include <stdio.h>
#include <stdlib.h>
#include "zorolib.h"

namespace ast{

  enum AsteroidSize{
    SMALL = 0,
    MID,
    BIG
  };
  
  struct rgb{
  	float r,g,b;
  };
  
  struct TColPoints
  {
    zoro::Vec2 points;
    TColPoints *next;
  };
  
  struct TAsteroidData{
    int kNPoints;
    zoro::Vec3 *g_points;
    zoro::Vec2 *dr_points; 
    TColPoints col;
  };

  struct TAsteroid{
    TAsteroid *next;
    AsteroidSize size;
    zoro::Vec2 pos = {400.0f, 400.0f};
    zoro::Vec2 dir = {2.5f, 1.5f};
    rgb color = {255, 255, 255};
    int type;
    float speed = 3.0f;
    float angle;
    double birthTime;
  };

  void InitList(TAsteroid **lista){
	  *(lista) = nullptr;
  };

  void GenerateAsteroidColPoints(TAsteroidData* data){
  
    for (int i = 0; i < data->kNPoints; i++)
    {
      
    }
}

  bool IsEmpty(TAsteroid *Lista){
	  return (Lista == nullptr);	
  }

  int ListLength(TAsteroid *lista){
	int l = 0;
	TAsteroid *p = lista;

	while ((p)->next != nullptr)
	{
		l++;
		p = (p)->next;
	}
	return l;
	
}

  void InsertList(TAsteroid **lista, TAsteroid* in_asteroid){
  
  TAsteroid* nuevo = (TAsteroid*) malloc(sizeof(TAsteroid));

  nuevo = in_asteroid;
  nuevo->next = nullptr;

  if (IsEmpty(*lista))
  {
    *lista = nuevo;
  }else{
    nuevo->next =*lista;
    *lista = nuevo;
  }

}

  void Delete(TAsteroid **lista, TAsteroid *asteroid){
    TAsteroid *p, *q;
    if (!IsEmpty(*lista)){
      p = *lista;
      if (p == asteroid){
        *lista = p->next;
        free(p);
      }
    }
  }
};