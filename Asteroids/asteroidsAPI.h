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
  
  struct TAsteroidData{
    int kNPoints;
    zoro::Vec3 *g_points;
    zoro::Vec2 *dr_points; 
  };

  struct TAsteroid{
    TAsteroid *next;
    AsteroidSize size;
    zoro::Vec2 pos = {400.0f, 400.0f};
    zoro::Vec2 dir = {2.5f, 1.5f};
    rgb color = {255, 255, 255};
    int type;
    float speed = 3.3f;
    float angle;
    double birthTime;
  };

  void InitList(TAsteroid **lista){
	  *(lista) = nullptr;
  };

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

  int Insert(TAsteroid **lista, TAsteroid asteroid){
  TAsteroid *nuevo;

  if ((nuevo = (TAsteroid*) malloc(1*sizeof(TAsteroid))) == nullptr)return 0; else{
    nuevo = &asteroid;
    nuevo->next = NULL;
    printf("\nPos x:%f y:%f", nuevo->pos.x,nuevo->pos.y);
    if (IsEmpty(*lista)){
      *lista = nuevo;
    }else{
      nuevo->next = *lista;
      *lista = nuevo;
    }
    return 1;
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