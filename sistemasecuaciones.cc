#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> 

struct Tecuacion{
    float x, y, ti;
};
struct Tvalorxy{
    float x, y;
    bool isIndeterminado;
    bool isIncompatible;
};


Tecuacion e1 = {6, 3, 9};
Tecuacion e2 = {4, 2, 12};
Tvalorxy valor;

Tvalorxy resolver2x2(Tecuacion re1, Tecuacion re2){
    
    Tecuacion auxiliar = re1;
    float x, y;
    bool isIndeterminado = false;
    bool isIncompatible = false;
    if(re1.x == 0 || re2.x == 0){
        re1.x*=re2.y;
        re1.y*=re2.y;
        re1.ti*=re2.y;

        re2.x*=auxiliar.y*-1;
        re2.y*=auxiliar.y*-1;
        re2.ti*=auxiliar.y*-1;
    }else{
        re1.x*=re2.x;
        re1.y*=re2.x;
        re1.ti*=re2.x;

        re2.x*=auxiliar.x*-1;
        re2.y*=auxiliar.x*-1;
        re2.ti*=auxiliar.x*-1;
    }
    
    if(re1.x - re2.x == 0 && re1.y - re2.y==0 && re1.ti - re2.ti != 0){
        x=0;
        y=0;
        isIncompatible = true;
    }
    else if(re1.x - re2.x*-1 == 0 && re1.y - re2.y*-1==0 && re1.ti - re2.ti*-1==0){
        x=0;
        y=0;
        isIndeterminado = true;
    }else{
        y = (re1.ti+re2.ti)/(re1.y+re2.y);
        x = (auxiliar.ti + auxiliar.y*-1*y)/(auxiliar.x);
    }
    Tvalorxy res = {x,y, isIndeterminado, isIncompatible}; 
    return res;
}

int main(){
   
    
    valor = resolver2x2(e1, e2);
    printf("x: %f \ny: %f", valor.x,valor.y);
    if(valor.isIndeterminado){
        printf("\nMisma linea. Indeterminado.");
    }
    if(valor.isIncompatible){
        printf("\nParalelas. Incompatible.");
    }


    return 0;
}