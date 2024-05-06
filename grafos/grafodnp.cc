#include <stdlib.h>
#include <stdio.h>
struct TGrafo
{
  int value;
  TGrafo *next;
  TGrafo *nodes;
};

void crear_grafo(TGrafo **grafo)
{
  *grafo = nullptr;
}

void insertarnodo(TGrafo **grafo, int new_value)
{

  TGrafo *p = (TGrafo *)malloc(1 * sizeof(TGrafo));
  p->value = new_value;
  p->next = *grafo;
  p->nodes = nullptr;
  
  *grafo = p;
}

void insertaadyacente(TGrafo **grafo, int searchValue, int new_value)
{

  TGrafo *p = *grafo;
  TGrafo *basic = nullptr, *newGraph = nullptr;
  bool found = false;

  while (p != nullptr || !found)
  {
    if (p->value == searchValue)
      basic = p;

    if (p->value == new_value)
      newGraph = p;

    if (basic != nullptr && newGraph != nullptr)
      found = true;

    p = p->next;
  }
  if (basic != nullptr && newGraph != nullptr)
  {
    TGrafo *wow = (TGrafo*) malloc(1*sizeof(TGrafo));
    wow->value = (int) newGraph;
    wow->next = basic->nodes;
    wow->nodes = newGraph->nodes;

    basic->nodes = wow;
  }
}

void mostrargrafo(TGrafo *grafo)
{
  TGrafo *p = grafo;
  TGrafo *t;

  while (p != nullptr)
  {
    printf("\n [%d]", p->value);
    t = p->nodes;
    while (t != nullptr)
    {
      printf(" %d", (*((TGrafo*) t->value)).value);
      t = t->next;
    }

    p = p->next;
  }
}