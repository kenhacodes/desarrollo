#include <stdio.h>
#include <stdlib.h>

FILE* my_file;

int main(){ 
  system("cls");
  
  //COMPROBACION (En este caso lo crea pero normalmente seria mala idea)
  if (fopen("./fich/prueba1.txt","r") == NULL)
  {
    my_file = fopen("./fich/prueba1.txt","w");
    fclose(my_file);
  }

  //Escribir
  
  char txt[50] = "\0";
  fgets(txt,50,stdin);

  //ESCRITURA
  my_file = fopen("./fich/prueba1.txt","a");
  fputs(txt,my_file);
  fclose(my_file);
  
  //LECTURA
  char c = ' ';
  my_file = fopen("./fich/prueba1.txt","r");
  do
  {
    c = fgetc(my_file);
    printf("%c",c);
  } while (!feof(my_file));
  fclose(my_file);
  
  return 0;
}