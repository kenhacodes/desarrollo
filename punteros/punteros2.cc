#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *v = nullptr;

int main(){
  system("cls");

  if ((v=(int*)malloc(10*sizeof(v)))==NULL)
  {
    printf("Error.");
  }else{

    
    for (int i = 0; i < 10; ++i)
    {
      *(v+i) = i+1;
    }
    
    for (int i = 0; i < 10; ++i)
    {
      printf("%p %d \n",v,*(v+i));
    }
    free(v);
    v = nullptr;

  }  

  return 0;
}