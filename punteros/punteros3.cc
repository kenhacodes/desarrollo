#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *v = nullptr;
int num;

int main(){
  system("cls");
  
  printf("Cuantos numeros?");
  scanf("%d",&num);



  if ((v=(int*)malloc(num*sizeof(v)))==NULL)
  {
    printf("Error.");
  }else{
    for (int i = 0; i < num; ++i)
    {
      *(v+i) = i+1;
    }
    
    for (int i = 0; i < num; ++i)
    {
      printf("%p %d \n",v,*(v+i));
    }
    free(v);
    v = nullptr;

  }  

  return 0;
}