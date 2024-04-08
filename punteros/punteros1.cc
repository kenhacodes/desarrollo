#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *p, *q, numero1=10, numero2=20;

void function(){
  int local1 = 100, local2 = 200;
  p=&local1;
  q=&local2;
  printf("%p %p %d\n", p,&p,*p);
  printf("%p %p %d\n", q,&q,*q);
}

int main(){
  system("cls");
  
  printf("VARIABLES LOCALES EN FUNCION \n");
  function();
  printf("\nVariables globales \n");
  p = &numero1;
  q = &numero2;
  printf("%p %p %d\n", p,&p,*p);
  printf("%p %p %d\n", q,&q,*q);
  
  return 0;
}