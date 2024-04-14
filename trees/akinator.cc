#include <stdlib.h>
#include <stdio.h>

struct Ttree
{
  char *text; //64
  Ttree *left, *right;
};

Ttree* create_tree(){

  Ttree *tree = (Ttree*) malloc(sizeof(Ttree));
  tree->text = (char*) malloc(32*sizeof(char));
  tree->text = "Animal";
  
  tree->left = (Ttree*) malloc(sizeof(Ttree));
  tree->right = (Ttree*) malloc(sizeof(Ttree));

  tree->left->text =(char*) malloc(32*sizeof(char));
  tree->right->text =(char*) malloc(32*sizeof(char));

  tree->left->text = "Pajaro";
  tree->right->text = "Coche";

  tree->left->left = nullptr;
  tree->left->right = nullptr;

  tree->right->left = nullptr;
  tree->right->right = nullptr;
  return tree;
}

bool isEmpty(Ttree** tree){
  return *tree == nullptr;
}

void insert_leaf(Ttree** tree, char* value){

  Ttree *p = *tree;
  Ttree *res = (Ttree*) malloc(sizeof(Ttree));

  res->text = (char*) malloc(32*sizeof(char));
  res->left = nullptr;
  res->right = nullptr;

  if(isEmpty(tree)){
    *tree = res;
  } 
  else{
    
    do
    {
      if (p->text == value) return;

      if (p->text < value){
        if (p->right == nullptr){
          p->right = res;
          return;
        }
        p = p->right;
      
      }else if(p->text > value){
        if (p->left == nullptr){
          p->left = res;
          return;
        } 
        p = p->left;
      }
    } while (p != nullptr);
   }
}

void paintLeaf(Ttree *tree, int BaseSpace, int depth){
  Ttree *point = tree;
  if (point->right != nullptr) paintLeaf(point->right, BaseSpace, depth+1);
  
  for (int i = 1; i < BaseSpace + depth*3; i++) printf(" ");
  printf("%c\n", point->text);
  
  if(point->left != nullptr) paintLeaf(point->left, BaseSpace, ++depth);
}

void ShowTree(Ttree* tree, int BaseSpace){
  paintLeaf(tree, BaseSpace, 0);
  printf("Pintado.");
}


Ttree *tree, *p;

int main(){

  tree = create_tree();
  p = tree;
  char answer = '\0';
  bool found = false;
  do
  {
    printf("Has pensado un ");
    fputs(p->text, stdout);
    printf("?");
    
    answer = fgetc(stdin);
    getchar(); 
    
    //fputc(answer, stdout);

    if (answer == 'y' || answer == 'Y' || answer == 'S' || answer == 's')
    {  
      if (p->right == nullptr)
      {
        printf("Es un ");
        fputs(p->text, stdout);
        printf("?");
      }
      
    }else{
      p = p->right;
    }
    
        
  } while (p->right != nullptr);
  
  if (!found)
  {
    printf("Es un perro");
  }
  

  return 0;
}
