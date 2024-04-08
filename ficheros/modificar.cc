#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

//SEEK_SET, SEEK_CUR, SEEK_END

FILE *f;
int buscar,valor,cambio,i;
bool encontrado=false;

void GenerarSecuencia(){
   f=fopen("numero.dat","wb");
  for(i=0;i<10;i++)
   {valor=rand()%100;
    fwrite(&valor,sizeof(valor),1,f);}
  fclose(f);
  f=fopen("numero.dat","rb");
  while(fread(&valor,sizeof(valor),1,f)){
    printf(" %d ",valor);
  }
  fclose(f);
}

void ModificarValor(int numero){

  f=fopen("numero.dat","r+b");
  while(fread(&numero,sizeof(valor),1,f)&&!encontrado){
    encontrado=numero==buscar;
    if(encontrado){
    
    printf("El valor buscado se ha localizado en el fichero.\n\n");
        fseek(f,-1*sizeof(numero),SEEK_CUR);
        fwrite(&cambio,sizeof(valor),1,f);
        fseek(f,1*sizeof(numero),SEEK_CUR);
        printf("Posicion modificada a nuevo valor.\n\n"); 
    }
  }
  fclose(f);
  if(encontrado){
  printf("Lectura de informacion del fichero:\n");
  f=fopen("numero.dat","rb");
  while(fread(&valor,sizeof(int),1,f)){
    printf(" %d ",valor);
  }
  fclose(f); 
  }else{
    printf("\n\nERROR: Valor no encontrado en fichero.\n");
  }
}

int main()
{
  GenerarSecuencia();
  printf("\nnumero? ");scanf("%d",&buscar);
  printf("\ncambio=");scanf("%d",&cambio);
  ModificarValor(cambio);


  
  

  return 0;
  

  //Puntos de salto: SEEL_SET, SEEK_CUR, SEEK_END
  //fseek(fichero,tamaño del salto en bytes,punto de salto)


}

