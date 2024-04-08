#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* normal;
FILE* par;
FILE* impar;

void inputpar(int num){
  
  fwrite(&num, sizeof(num),1,par);
  
}

void inputimpar(int num){
  
  fwrite(&num, sizeof(num),1,impar);
  
}

int main(){ 
 
  system("cls");  
  
  

  normal = fopen("./fich/numeros.dat","rb");
  
  int num;
  par = fopen("./fich/par.dat","wb");
  impar = fopen("./fich/impar.dat","wb");
  while (fread(&num,sizeof(num),1,normal))
  {
    if(num%2==0){
      inputpar(num);
    }else{
      inputimpar(num);
    }
  }
  
  fclose(normal);
  fclose(par);
  fclose(impar);  
  return 0;
}