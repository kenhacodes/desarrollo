#include <stdio.h>
#include "barbol.cc"

Tbarbol *barbol1,*buscar;


int main(){

  crear_barbol(&barbol1);
  
  //Creaci�n arbol binario.
  
  barbol1=insertar_barbol(3,insertar_barbol(7,NULL,NULL),insertar_barbol(10,NULL,NULL));
  barbol1=insertar_barbol(15,barbol1,insertar_barbol(9,insertar_barbol(8,NULL,NULL),insertar_barbol(10,NULL,NULL)));
  
  //o bien
  //barbol2=insertar_barbol(20,insertar_barbol(12,NULL,NULL),insertar_barbol(13,NULL,NULL));
  //y luego barbol1=inserta_barbol(15,barbol1,barbol2);
  

  printf("\n------------ ARBOL GENERADO ----------------\n");
  MostrarArbol( barbol1, 3);
  printf("--------------------------------------------\n\n\n");

 





return 0;}