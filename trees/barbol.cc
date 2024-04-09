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

  int maxDepth = 2;
  int depth = 0;
  bool lookRight = true;
  Tbarbol* point = tree;
  
  Tbarbol* level = (Tbarbol*) malloc(maxDepth * sizeof(Tbarbol));

  while (point->left != nullptr && depth != maxDepth+1)
  {    
    while (point->right != nullptr)
    {
      *(level + depth) = *point;
      depth++;
      point = point->right;
    };

    for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
    printf("%03d\n", point->info);

    depth--;
    point = (level+depth);
    
    for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
    printf("%03d\n", point->info);

    depth-=2;
    point = (level+depth)->left;
  }
  

  


  
}

