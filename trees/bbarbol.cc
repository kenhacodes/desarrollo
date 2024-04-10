#include <stdlib.h>
#include <stdio.h>

struct tbbarbol
{
  int info;
  tbbarbol *left, *right;
};

struct pila{
  tbbarbol* value;
  pila* next;
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
      if (p->info == value) return;

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

void paintLeaf(tbbarbol *tree, int BaseSpace, int depth){
  tbbarbol *point = tree;
  if (point->right != nullptr) paintLeaf(point->right, BaseSpace, depth+1);
  
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d\n", point->info);
  
  if(point->left != nullptr) paintLeaf(point->left, BaseSpace, ++depth);
}

void MostrarArbol(tbbarbol* tree, int BaseSpace){
  paintLeaf(tree, BaseSpace, 0);
  printf("Pintado.");
}

void inorden(tbbarbol* tree){

  int depth = 0;
  bool end = false;

}

void preorden(tbbarbol* tree){

}

void postorden(tbbarbol* tree){

}

tbbarbol* buscar_barbol(tbbarbol* tree, int value){
  return tree;
}