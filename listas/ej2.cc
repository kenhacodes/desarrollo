#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "TListas.cc"

#define CrearConjunto CrearLista
#define EscacioConjunto isEmptyList
#define MuestraConjunto MuestraLista


#define TConjunto TLista

TConjunto *conjunto1,*conjunto2,*resultadoU,*resultadoI;

bool PerteneceConjunto(TConjunto *c, int elemento){
  return BuscarEnLista(c, elemento, 0) != nullptr;
}

int IncluirConjunto(TConjunto **c, int num){
  if (!PerteneceConjunto(*c,num)){
    InsertarLista(c, num);
  }
}

void UnionConjunto(TConjunto *c1, TConjunto *c2, TConjunto **resultado){

      TConjunto *p, *q;

      if (PerteneceConjunto(p, q->info) && PerteneceConjunto(p, q->info)){
        /* code */
      }
      

}

void InterseccionConjunto(TConjunto *c1, TConjunto *c2, TConjunto **resultado){
  


}

int main(){
    CrearConjunto(&conjunto1);
    CrearConjunto(&conjunto2);
    CrearConjunto(&resultadoU);
    CrearConjunto(&resultadoI);

    IncluirConjunto(&conjunto1,3);
    IncluirConjunto(&conjunto1,10);
    IncluirConjunto(&conjunto1,3);
    IncluirConjunto(&conjunto1,15);
    IncluirConjunto(&conjunto1,3);
    IncluirConjunto(&conjunto1,30);
    
    printf("\n\nVolcado conjunto 1\n");
    MuestraConjunto(conjunto1);

    IncluirConjunto(&conjunto2,18);
    IncluirConjunto(&conjunto2,10);
    IncluirConjunto(&conjunto2,35);
    IncluirConjunto(&conjunto2,1);
    IncluirConjunto(&conjunto2,3);
    IncluirConjunto(&conjunto2,45);

    printf("\n\nVolcado conjunto 2\n");
    MuestraConjunto(conjunto2);
    
    UnionConjunto(conjunto1,conjunto2,&resultadoU);

    printf("\n\nVolcado Union\n");
    MuestraConjunto(resultadoU);

    InterseccionConjunto(conjunto1,conjunto2,&resultadoI);

    printf("\n\nVolcado Interseccion\n");
    MuestraConjunto(resultadoI);
    
	return 0;
}
