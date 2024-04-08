#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char **list = nullptr;

int numStrings;

char* getWord(){
  char *string = nullptr;
  string = (char*)malloc(50*sizeof(char));
  printf("->");
  fgets(string,50,stdin);
  return string;
}

int main(){

  printf("Cuantos string quieres escribir?");
  scanf("%d",&numStrings);
  scanf(" ");
 
  
  

  
  


  return 0;
}