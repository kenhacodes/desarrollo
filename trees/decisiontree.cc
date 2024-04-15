#include <stdlib.h>
#include <stdio.h>

struct tbbarbol
{
  int info;
  char *text; //32
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

void printLeaf(tbbarbol *leaf, int BaseSpace, int depth){
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d\n", leaf->info);

}

void paintLeaf(tbbarbol *tree, int BaseSpace, int depth){
  tbbarbol *point = tree;
  if (point->right != nullptr) paintLeaf(point->right, BaseSpace, depth+1);
  
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%03d (%02d)\n", point->info);
  
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
  /*
  pile* p = pila;
  printf("(");
  while(p != nullptr){
    printf("%d.",p->value->info);
    p = p->next;
  }
  printf(")");
  */
  
}

void PopPile(pile** pila){
  pile *p;
  if (!isEmptyPile(*pila))
  {
    p = *pila;
    *pila = +p->next;
    free(p);
  }
}

void inorden(tbbarbol* tree){

  tbbarbol* p = tree;
  tbbarbol* endPoint = tree;
  pile* pila = nullptr;

  while (endPoint->left != nullptr) endPoint = endPoint->left;

  while (p != endPoint)
  {
    //printf(" ! ");
    while (p->left != nullptr)
    {
      addToPile(&pila, p);
      p = p->left;
    };
    

    printf(" %d", p->info);   
    p = pila->value;
    PopPile(&pila);  
    
    printPile(pila);
    
    printf(" %d", p->info);  
     
    PopPile(&pila);     
        
    p = p->right;             
    addToPile(&pila,p); 
    
      

    if(p->left == nullptr){

      printf(" %d", p->info); 
      PopPile(&pila);         
      
      if (p->right != nullptr)
      {
        p = p->right;
        addToPile(&pila,p);
      }
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