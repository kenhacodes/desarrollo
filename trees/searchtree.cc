#include <stdio.h>
#include <stdlib.h>
#include "bbarbol.cc"

tbbarbol *barbol1,*buscado; 

int main(){

  barbol1=crear_barbol();
  
  //Creación arbol binario.

  insertar_barbol(&barbol1,8);
  
  insertar_barbol(&barbol1,10);
  insertar_barbol(&barbol1,9);
  insertar_barbol(&barbol1,11);
  insertar_barbol(&barbol1,13);
  insertar_barbol(&barbol1,12);
  insertar_barbol(&barbol1,14);
  
  insertar_barbol(&barbol1,4);
  insertar_barbol(&barbol1,2);
  insertar_barbol(&barbol1,6);
  insertar_barbol(&barbol1,1);
  insertar_barbol(&barbol1,3);
  insertar_barbol(&barbol1,5);
  insertar_barbol(&barbol1,7);

  

  //No deberán insertarse, no error pero no duplicados.
  insertar_barbol(&barbol1,5);
  insertar_barbol(&barbol1,7);

  
  printf("\n------------ ARBOL GENERADO ----------------\n");
  MostrarArbol( barbol1, 0);
  printf("--------------------------------------------\n\n\n");

  printf("In  : ");inorden(barbol1);printf("\n");
  printf("Pre : ");preorden(barbol1);;printf("\n");
  printf("Post: ");postorden(barbol1);;printf("\n");
  
  buscado=buscar_barbol(barbol1,7);
  if(buscado!=NULL) printf("\nEncontrado. Nodo apuntado %d",buscado->info);
  else printf("\nNo encontrado");
  
return 0;}