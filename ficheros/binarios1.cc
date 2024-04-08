#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* normal;


int main(){ 

  system("cls");  
  
  normal = fopen("./fich/prueba.dat","ab");

  for (int i = 0; i < 5; i++)
  {
    int var;
    scanf("%d",&var);
    fwrite(&var, sizeof(var),1,normal);
  }
  
  fclose(normal);
  
  normal = fopen("./fich/prueba.dat","rb");
  
  int num;
  while (fread(&num,sizeof(num),1,normal))
  {
    printf("%d ", num);
  }
  
  fclose(normal);
  

  return 0;
}