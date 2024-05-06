#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "grafodnp.cc"

TGrafo *grafo1;

int main(){
    crear_grafo(&grafo1);
    insertarnodo(&grafo1,1);
    insertarnodo(&grafo1,2);
    insertarnodo(&grafo1,3);
    insertarnodo(&grafo1,4);

    insertaadyacente(&grafo1,1,2);
    insertaadyacente(&grafo1,1,3);
    insertaadyacente(&grafo1,2,3);
	  insertaadyacente(&grafo1,3,2);
    insertaadyacente(&grafo1,3,4);
    insertaadyacente(&grafo1,4,1);
    insertaadyacente(&grafo1,4,2);
    
    
    mostrargrafo(grafo1);




return 0;
}
