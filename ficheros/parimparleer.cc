#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE* par;
FILE* impar;

int main(){ 
 
  system("cls");  

  int num;
  printf("Par->\n");
  par = fopen("./fich/par.dat","rb");
  while (fread(&num,sizeof(num),1,par))
  {
    printf("%d ",num);
  }
  fclose(par);
  printf("\n\nImPar->\n");
  impar = fopen("./fich/impar.dat","rb");
  while (fread(&num,sizeof(num),1,par))
  {
    printf("%03d ",num);
  }
  fclose(par);

  return 0;
}