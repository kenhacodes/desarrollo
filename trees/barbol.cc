#include <stdlib.h>
#include <stdio.h>

struct Tbarbol
{
  int info;
  Tbarbol *left, *right;
};

void crear_barbol(Tbarbol **tree){
  (*tree) = nullptr;
}

Tbarbol* insertar_barbol(int value, Tbarbol *tleft, Tbarbol *tright){
  Tbarbol *res = (Tbarbol*) malloc(sizeof(Tbarbol));
  res->info = value;
  res->left = tleft;
  res->right = tright;
  return res;

}

void printLeaf(Tbarbol *leaf, int BaseSpace, int depth){
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d\n", leaf->info);
}

void paintLeaf(Tbarbol *tree, int BaseSpace, int depth){
  
  Tbarbol *point = tree;
  if (point->right != nullptr) paintLeaf(point->right, BaseSpace, depth+1);
  
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d\n", point->info);
  
  if(point->left != nullptr) paintLeaf(point->left, BaseSpace, ++depth);
}

void MostrarArbol(Tbarbol *tree, int BaseSpace){
  
  paintLeaf(tree, BaseSpace, 0);
  printf("\n------------ Other version ----------------\n");

  int maxDepth = 0;
  int depth = 0;
  bool end = false;
  int i = 0;
  int x = 0;
  Tbarbol* endPoint = tree;
  Tbarbol* point = tree;
  
  Tbarbol* level = (Tbarbol*) malloc(maxDepth * sizeof(Tbarbol));

  while (endPoint->left != nullptr){
    maxDepth++;                                     // Only works if the max depth its the same for everything
    endPoint = endPoint->left;
  } 
  printf("->%d\n",maxDepth);
  while (!end)
  {    
    while (point->right != nullptr)
    {
      *(level + depth) = *point;
      depth++;
      point = point->right;
    };
    printLeaf(point, BaseSpace, depth);

    depth--;
    point = (level+depth);
    printLeaf(point, BaseSpace, depth);

    depth++;
    point = point->left;
    printLeaf(point, BaseSpace, depth);

    if (depth-(2+i)  < 0 || point == endPoint) end = true;
    else{
      depth-=2+i;
      i++;
      if (depth == i-x){
        i = 0;
        x++;
      } 
      
      point = (level+depth);
      printLeaf(point, BaseSpace, depth);

      depth++;
      point = point->left;
    } 
  }
}

