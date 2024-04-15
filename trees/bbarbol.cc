#include <stdlib.h>
#include <stdio.h>

struct tbbarbol
{
  int info;
  int contador = 1;
  tbbarbol *left, *right;
};

struct pile{
  tbbarbol* value;
  pile* next;
};

tbbarbol* crear_barbol(){
  tbbarbol* tree = nullptr;
  return tree;
}

bool isEmpty(tbbarbol** tree){
  return *tree == nullptr;
}

void insertar_barbol(tbbarbol** tree, int value){
  tbbarbol *p = *tree;

  tbbarbol *res = (tbbarbol*) malloc(1*sizeof(tbbarbol));
  res->info = value;
  res->contador = 1;
  res->left = nullptr;
  res->right = nullptr;


  if(isEmpty(tree)){
    printf("!");
    *tree = res;
  } 
  else{
    printf(".");
    do
    {
      if (p->info == value) {
        p->contador++;
        return;
      }

      if (p->info < value){
        if (p->right == nullptr){
          p->right = res;
          return;
        }
        p = p->right;
      
      }else if(p->info > value){
        if (p->left == nullptr){
          p->left = res;
          return;
        } 
        p = p->left;
      }
    } while (p != nullptr);
   }
}

void printLeaf(tbbarbol *leaf, int BaseSpace, int depth){
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d (%02d)\n", leaf->info, leaf->contador);

}

void paintLeaf(tbbarbol *tree, int BaseSpace, int depth){
  tbbarbol *point = tree;
  if (point->right != nullptr) paintLeaf(point->right, BaseSpace, depth+1);
  
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d (%02d)\n", point->info, point->contador);
  
  if(point->left != nullptr) paintLeaf(point->left, BaseSpace, ++depth);
}

void MostrarArbol(tbbarbol* tree, int BaseSpace){
  paintLeaf(tree, BaseSpace, 0);
  printf("Pintado.");
}

bool isEmptyPile(pile *p){
  return (p==NULL);
}

void addToPile(pile **pila, tbbarbol* tree){

  pile* nuevo = (pile*) malloc(1*sizeof(pile));
  
  nuevo->value = tree;
  nuevo->next = nullptr;
  if (isEmptyPile(*pila))
  {
    *pila = nuevo;
  }else{
    nuevo->next =*pila;
    *pila = nuevo;
  }
}

void printPile(pile* pila){
  
  pile* p = pila;
  printf("(");
  while(p != nullptr){
    printf("%d.",p->value->info);
    p = p->next;
  }
  printf(")");
  
  
}

pile* PopPile(pile** pila){
  pile *p, *res;
  res = *pila;
  if (!isEmptyPile(*pila))
  {
    p = *pila;
    *pila = p->next;
    free(p);
  }
  return res;
  
}

void inorden(tbbarbol* tree){

  tbbarbol* p = tree;
  tbbarbol* temp = tree;
  pile* pila = nullptr;

  while (true)
  {
    if (p->left != nullptr)
    {
      addToPile(&pila, p);
      p = p->left;
      //printf("(%d)", pila->value->info);

    }else{
      printf("%d ", p->info);
      p = pila->value;
      pila = pila->next;
      if (p == nullptr) break;
      
      p->left = nullptr;
      printf("%d ", p->info);

      temp = p;
      p = nullptr;
      p = temp->right;
    }
    
  }
  
  
    
  
}

void preorden(tbbarbol* tree){

}

void postorden(tbbarbol* tree){

}

tbbarbol* buscar_barbol(tbbarbol* tree, int value){
  return tree;
}