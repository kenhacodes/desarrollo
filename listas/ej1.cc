#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "TListas.cc"

TLista *lista1;
TLista *lista2;

int main(int argc, char **argv){

    CrearLista(&lista1);
    CrearLista(&lista2);

    InsertarLista(&lista1,3);
    InsertarLista(&lista1,5);
    InsertarLista(&lista1,15);
    InsertarLista(&lista1,28);
    InsertarLista(&lista1,25);
    InsertarLista(&lista1,45);
    InsertarLista(&lista1,105);
    InsertarLista(&lista1,280);

    printf("\n\n");
    printf(" Lista >>> ");
    MuestraLista(lista1);

    printf("\n\n Acceso indexado a lista por valor que ocupa posicion 0. \n");
    lista2=IndexaLista(lista1,0);
    printf(" Elemento apuntado de la lista: %d \n",lista2->info);

    printf("\n\n Buscar y apuntar. \n");
    if((lista2=BuscarEnLista(lista1,3,0))==NULL){
        printf("\nNo encontrado.");
    } else {
        printf("\n Localizado y apuntado: %d",lista2->info);
    }

    printf("\n\n Localizar y eliminar. valores 28, 3 y 280\n");
    EliminaEnLista(&lista1,28);
    printf("\n Eliminado valor 28 de la lista\n");
    printf(" Lista >>> ");
    MuestraLista(lista1);
    EliminaEnLista(&lista1,3);
    printf("\n Eliminado valor 3 de la lista\n");
    printf(" Lista >>> ");
    MuestraLista(lista1);
    EliminaEnLista(&lista1,280);
    printf("\n Eliminado valor 280 de la lista\n");
    printf(" Lista >>> ");
    MuestraLista(lista1);

	return 0;
}