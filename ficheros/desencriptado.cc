#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE* encriptado;
FILE* desencriptado;

int encription = -33;

void desencripcion(){
  //LECTURA
  char c = ' ';
  
  desencriptado = fopen("./fich/desencriptado.txt","w");
  fclose(desencriptado);

  encriptado = fopen("./fich/encriptado.txt","r");
  do
  {
    c = fgetc(encriptado);

    if (c != EOF)
    {
      c-=encription;
      desencriptado = fopen("./fich/desencriptado.txt","a");
      fputc(c,desencriptado);
      fclose(desencriptado);
    }

  } while (!feof(encriptado));
  fclose(encriptado);

  printf("Desencriptado!");
}

int main(){ 
  system("cls");

  if (fopen("./fich/encriptado.txt","r") == NULL)
  {
    printf("Error.");
  }else{
    desencripcion();
  }
  

 

  return 0;
}