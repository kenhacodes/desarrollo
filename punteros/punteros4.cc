#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int *v = nullptr;
int num;
int len;
int contador = 0;

void generate(){
  for (int i = 0; i < len; ++i)
  {
    *(v+i) = 1+rand()%99;
  }
}

void show(){
  for (int i = 0; i < len; ++i)
  {
    printf("%p %d \n",v,*(v+i));
  }
}

void askAndAnswer(){
   printf("Que numero? -> ");
    scanf("%d",&num);

    for (int i = 0; i < len; ++i)
    {
      if(num==*(v+i)){
        contador++;
      }
    }

    printf("De %d numeros el %d se repite %d", len,num,contador);
}

int main(){
  system("cls");
  srand(time(NULL));
  len = rand()%999;

  if ((v=(int*)malloc(len*sizeof(v)))==NULL)
  {
    printf("Error.");
  }else{
    
    generate();
    //show();
    askAndAnswer();

    free(v);
    v = nullptr;
  }  

  return 0;
}