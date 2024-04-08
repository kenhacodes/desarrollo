#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* normal;
FILE* encriptado;
FILE* desencriptado;

int encription = -33;

int main(){ 
  system("cls");

  if (fopen("./fich/prueba1.txt","r") == NULL)
  {
    normal = fopen("./fich/prueba1.txt","w");
    fclose(normal);
  }
  

  //LECTURA
  char c = ' ';
  
  encriptado = fopen("./fich/encriptado.txt","w");
  fclose(encriptado);

  normal = fopen("./fich/prueba1.txt","r");
  do
  {
    c = fgetc(normal);

    if (c != EOF)
    {
      c+=encription;
      encriptado = fopen("./fich/encriptado.txt","a");
      fputc(c,encriptado);
      fclose(encriptado);
    }

  } while (!feof(normal));
  fclose(normal);

  printf("Encriptado!");

  return 0;
}