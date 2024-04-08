#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int v1, v2;

int main(int argc, char **argv){
  if(argc == 3){
    v1 = atoi(*(argv+1));
    v2 = atoi(*(argv+2));
    printf("%d + %d = %d", v1,v2,v1+v2);
  }else{
    printf("WoW");
  }
}