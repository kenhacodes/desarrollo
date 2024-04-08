#include <stdio.h>
#include <stdlib.h>
#include "zorolib.h"

namespace ast{

  enum AsteroidBigness{
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
    int type;
    AsteroidBigness size;
    zoro::Vec2 pos = {400.0f, 400.0f};
    zoro::Vec2 dir = {2.5f, 1.5f};
    float speed = 3.3f;
    float angle;
    rgb color = {255, 255, 255};
    double birthTime;
  };

  TAsteroid *InitList(TAsteroid **lista){
	  return *(lista) = nullptr;
  };

  bool IsEmpty(TAsteroid *Lista){
	  return Lista == nullptr;	
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

  if ((nuevo = (TAsteroid*) malloc (sizeof(TAsteroid))) == nullptr){
    return 0;
    
  }else{
    nuevo = &asteroid;
    nuevo->next = nullptr;
    if (IsEmpty(*lista))
    {
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