#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* normal;
FILE* uppercasetxt;

int main(){ 
  system("cls");

  if (fopen("./fich/prueba1.txt","r") == NULL)
  {
    normal = fopen("./fich/prueba1.txt","w");
    fclose(normal);
  }
  

  //LECTURA
  char c = ' ';
  
  uppercasetxt = fopen("./fich/uppercase.txt","w");
  fclose(uppercasetxt);

  normal = fopen("./fich/prueba1.txt","r");
  do
  {
    c = fgetc(normal);

    if (c>=97 && c<=122)
    {
      c-=32;
      uppercasetxt = fopen("./fich/uppercase.txt","a");
      fputc(c,uppercasetxt);
      fclose(uppercasetxt);
    }else if (c != EOF)
    {
      uppercasetxt = fopen("./fich/uppercase.txt","a");
      fputc(c,uppercasetxt);
      fclose(uppercasetxt);
    }

  } while (!feof(normal));
  fclose(normal);

  return 0;
}