#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Tloteria
{
  int year;
  int num[6];
};


FILE* loteriatxt;
FILE* loteriadat;

Tloteria listaLoterias[100];

void clean(char string[50]){
  for (int i = 0; i < strlen(string); i++)
  {
    string[i] = ' ';
  }
}

void importar(){
  int iloteria=0;
  int num;
  int counter = 0;
  int starpos=5;
  char c[50];
  char temp[8];
  loteriatxt = fopen("./fich/resultados.txt","r");
  do
  {
    fgets(c,50,loteriatxt);

    for (int i = 0; i < 4; i++)
    {
      temp[i] = c[i];
    }
    listaLoterias[iloteria].year = atoi(temp);
    

    for (int i = 5; i < strlen(c) && counter<=5; i++)
    {
      if (c[i] == ';')
      {
        for (int j = starpos; j < i; j++)
        {
          temp[j-starpos] = c[j];
        }
        listaLoterias[iloteria].num[counter] = atoi(temp);
        counter++;
        starpos=i+1;
      }
    }
    /*
    printf("\n->y:%d, n1:%d,n2:%d,n3:%d,n4:%d,n5:%d,n6:%d",
      listaLoterias[iloteria].year,listaLoterias[iloteria].num[0],
      listaLoterias[iloteria].num[0],listaLoterias[iloteria].num[2]
      listaLoterias[iloteria].num[3],listaLoterias[iloteria].num[4],
      listaLoterias[iloteria].num[5]);
    */
    printf("\n->y:%d, n1:%d",listaLoterias[iloteria].year,listaLoterias[iloteria].num[0]);
    iloteria++;
  } while (!feof(loteriatxt));
  
  fclose(loteriatxt);
}

int main(){ 

  system("cls");  
  importar();
  
  return 0;
}