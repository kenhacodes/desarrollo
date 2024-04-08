#include <stdio.h>
#include <stdlib.h>

#include "_TListaDoble.cc"

TLista2 *Lista1,*Lista2,*l;

int main(){

	Lista1=CrearLista();
	Lista2=CrearLista();
	InsertarLista(&Lista1,4);
	InsertarLista(&Lista1,15);
	InsertarLista(&Lista1,20);
	InsertarLista(&Lista1,25);
	InsertarLista(&Lista1,10);
	InsertarLista(&Lista2,14);
	InsertarLista(&Lista2,25);
	InsertarLista(&Lista2,30);
	InsertarLista(&Lista2,45);
	InsertarLista(&Lista2,80);
	printf("Lista 1          : ");MostrarLista(Lista1); printf("\n");
	printf("Lista 2          : ");MostrarLista(Lista2); printf("\n");
    
    if((l=BuscarEnLista(Lista2,45))!=NULL){
    	printf("Localizado y apuntado: %d\n",l->info);
    }else{
    	printf("No encontrado en lista.");
    }
    
    printf("\n-----------------\n");
    printf("Lista 1 Invertida: ");InvertidoMostrarLista(Lista1);printf("\n"); //Comprobación de recorrido invertido;
    printf("Lista 2 Invertida: ");InvertidoMostrarLista(Lista2);printf("\n"); //Comprobación de recorrido invertido;

    l=ExtraerLista(&Lista1);printf("Numero Extraido  : %d\n",l->info);free(l);
    printf("Lista 1          : ");MostrarLista(Lista1); printf("\n");
    printf("Lista 1 Invertida: ");InvertidoMostrarLista(Lista1);printf("\n"); //Comprobación de recorrido invertido;

    printf(" >>> Eliminado valor 20 en lista 1\n");EliminarElemento(&Lista1,20);
    printf("Lista 1          : ");MostrarLista(Lista1);printf("Long: %d\n",LongitudLista(Lista1));
    printf("Lista 2          : ");MostrarLista(Lista2);printf("Long: %d\n",LongitudLista(Lista2));
    printf("\n-----------------\n");
    EliminarElemento(&Lista1,20);printf("Lista 1          : ");MostrarLista(Lista1); printf("\n");
    EliminarElemento(&Lista1,4); printf("Lista 1          : ");MostrarLista(Lista1); printf("\n");
    EliminarElemento(&Lista2,30);printf("Lista 2          : ");MostrarLista(Lista2); printf("\n");
    EliminarElemento(&Lista2,14);printf("Lista 2          : ");MostrarLista(Lista2); printf("\n");
    EliminarElemento(&Lista2,45);printf("Lista 2          : ");MostrarLista(Lista2); printf("\n");
    printf("\n-----------------\n");
    printf("Lista 1          : ");MostrarLista(Lista1); printf("\n");
    printf("Lista 2          : ");MostrarLista(Lista2); printf("\n");

    return 0;
}

