#include <stdio.h>
#include <stdlib.h>
#include "zorolib.h"

namespace ast
{


  enum AsteroidSize
  {
    SMALL = 0,
    MID,
    BIG
  };

  struct rgb
  {
    float r, g, b;
  };

  struct TColPoints
  {
    zoro::Vec2 *points;
    TColPoints *next;
    int NumColPoints;
  };

  struct TPaintColData
  {
    int kNPoints;
    zoro::Vec3 *g_points;
    zoro::Vec2 *dr_points;
    TColPoints *col;
  };

  struct TAsteroid
  {
    TAsteroid *next;
    TAsteroid *prev;
    AsteroidSize size;
    zoro::Vec2 pos = {400.0f, 400.0f};
    zoro::Vec2 dir = {2.5f, 1.5f};
    rgb color = {255, 255, 255};
    int type;
    float speed = 3.0f;
    float angle;
    double birthTime;
    zoro::Mat3 M;
  };

  void InitList(TAsteroid **lista)
  {
    *(lista) = nullptr;
  };

  void insertNewShape(TPaintColData *data, int count, int offset)
  {

    TColPoints *p = data->col;

    // Go to last shape in list
    while (p->next != nullptr)
    {
      p = p->next;
    }

    p->next = (TColPoints *)malloc(sizeof(TColPoints));
    p = p->next;
    p->next = nullptr;

    p->NumColPoints = 3 + offset;
    p->points = (zoro::Vec2 *)malloc((p->NumColPoints) * sizeof(zoro::Vec2));

    for (int i = -1; i < (3 + offset - 1); i++)
    {
      if (i + count < 0)
      {
        (*(p->points + i + 1)).x = (*(data->g_points + (data->kNPoints - 1))).x;
        (*(p->points + i + 1)).y = (*(data->g_points + (data->kNPoints - 1))).y;
      }
      else if (count + i >= data->kNPoints - 1)
      {
        (*(p->points + i + 1)).x = (*(data->g_points + i + count - data->kNPoints)).x;
        (*(p->points + i + 1)).y = (*(data->g_points + i + count - data->kNPoints)).y;
      }
      else
      {
        (*(p->points + i + 1)).x = (*(data->g_points + i + (count))).x;
        (*(p->points + i + 1)).y = (*(data->g_points + i + (count))).y;
      }
    }
    // Add shape
  }

  void addToShape(TPaintColData *data, zoro::Vec2 newP)
  {
    TColPoints *p = data->col;
    *(p->points + p->NumColPoints) = newP;
    p->NumColPoints += 1;
  }

  void GenerateAsteroidColPoints(TPaintColData *data)
  {
    printf("\n---\n");
    printf("(%d)\n", data->kNPoints);

    zoro::Vec2 q, p, d;
    int offset;
    bool found;

    for (int i = 0; i < data->kNPoints; i++)
    {

      offset = 0;
      found = false;

      // q = p-1
      // p = i
      // d = p+1

      p.x = (*(data->g_points + i)).x;
      p.y = (*(data->g_points + i)).y;

      if (i == 0)
      {
        q.x = (*(data->g_points + data->kNPoints - 1)).x;
        q.y = (*(data->g_points + data->kNPoints - 1)).y;
      }
      else
      {
        q.x = (*(data->g_points + i - 1)).x;
        q.y = (*(data->g_points + i - 1)).y;
      }

      if (i == data->kNPoints - 1)
      {
        d.x = (*(data->g_points + 0)).x;
        d.y = (*(data->g_points + 0)).y;
      }
      else
      {
        d.x = (*(data->g_points + i + 1)).x;
        d.y = (*(data->g_points + i + 1)).y;
      }

      // printf("[%d](%f) ",i, zoro::angleBetween(p, q, d));

      if (zoro::angleBetween(q, p, d) <= zoro::PI)
      {
        printf("[%d](%f) ", i, zoro::angleBetween(p, q, d));
        // printf("u ");
        do
        {
          // q = p - 1
          // p = i + offset + 1
          // d = p + 1

          if (i + offset + 1 >= data->kNPoints)
          {
            p.x = (*(data->g_points + (i + offset + 1) - data->kNPoints)).x;
            p.y = (*(data->g_points + (i + offset + 1) - data->kNPoints)).y;
          }
          else
          {
            p.x = (*(data->g_points + i + offset + 1)).x;
            p.y = (*(data->g_points + i + offset + 1)).y;
          }

          if (i + offset >= data->kNPoints)
          {
            q.x = (*(data->g_points + (i + offset) - data->kNPoints)).x;
            q.y = (*(data->g_points + (i + offset) - data->kNPoints)).y;
          }
          else
          {
            q.x = (*(data->g_points + i + offset)).x;
            q.y = (*(data->g_points + i + offset)).y;
          }

          if (i + offset + 2 >= data->kNPoints)
          {
            d.x = (*(data->g_points + (i + offset + 2) - data->kNPoints)).x;
            d.y = (*(data->g_points + (i + offset + 2) - data->kNPoints)).y;
          }
          else
          {
            d.x = (*(data->g_points + i + offset + 2)).x;
            d.y = (*(data->g_points + i + offset + 2)).y;
          }

          if (zoro::angleBetween(q, p, d) > zoro::PI)
          {
            found = true;
          }
          else
          {
            offset++;
          }

        } while (!found);

        // Insert new shape
        insertNewShape(data, i, offset);

        i += offset;
      }
      else
      {
        zoro::Vec2 newPoint;
        // Add to main shape

        if (i == 0)
        {
          newPoint.x = (*(data->g_points + data->kNPoints - 1)).x;
          newPoint.y = (*(data->g_points + data->kNPoints - 1)).y;
        }
        else
        {
          newPoint.x = (*(data->g_points + i - 1)).x;
          newPoint.y = (*(data->g_points + i - 1)).y;
        }
        newPoint.x = (*(data->g_points + i)).x;
        newPoint.y = (*(data->g_points + i)).y;
        addToShape(data, newPoint);
      }
    }
    printf("\n---");
  }

  bool IsEmpty(TAsteroid *Lista)
  {
    return (Lista == nullptr);
  }

  int ListLength(TAsteroid *lista, bool weighted)
  {
    int l = 0;
    TAsteroid *p = lista;

    while (p != nullptr)
    {
      if (weighted)
      {
        switch (p->size)
        {
        case SMALL:
          l++;
          break;
        case MID:
          l+=2;
        case BIG:
          l+=4;
        default:
          break;
        }
      }else{
        l++;  
      }
      
      
      p = (p)->next;
    }
    return l;
  }

  void InsertList(TAsteroid **lista, TAsteroid *in_asteroid)
  {
    TAsteroid *nuevo = (TAsteroid *)malloc(sizeof(TAsteroid));

    nuevo->size = in_asteroid->size;
    nuevo->pos = in_asteroid->pos;
    nuevo->dir = in_asteroid->dir;
    nuevo->color = in_asteroid->color;
    nuevo->type = in_asteroid->type;
    nuevo->speed = in_asteroid->speed;
    nuevo->angle = in_asteroid->angle;
    nuevo->birthTime = in_asteroid->birthTime;

    nuevo->next = nullptr;
    nuevo->prev = nullptr;

    if (IsEmpty(*lista))
      *lista = nuevo;
    else
    {
      nuevo->next = *lista;
      (*lista)->prev = nuevo;
      *lista = nuevo;
    }
    //free(in_asteroid);
    in_asteroid = nullptr;
  }

  void Delete(TAsteroid **lista, TAsteroid *nodeToDelete)
  {
    if (*lista == nullptr || nodeToDelete == nullptr)
      return;

    if (*lista == nodeToDelete)
      *lista = nodeToDelete->next;
    
    if (nodeToDelete->next != nullptr)
      nodeToDelete->next->prev = nodeToDelete->prev;


    if (nodeToDelete->prev != nullptr)
      nodeToDelete->prev->next = nodeToDelete->next;
    nodeToDelete = nullptr;
    free(nodeToDelete);
    
  }
};