#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Tdistancias{
  char ciudad1[20] = {'\0'};
  char ciudad2[20] = {'\0'};
  int km;
};

struct TListaCiudades{
  char origen[20] = {'\0'};
  char destinos[50][20] = {{'\0'},{'\0'}};
  int numDestinos = 0;
};

struct V2{
  int n1 = -1,n2 = -1;
};

FILE* kmtxt;
FILE* kmbin;

TListaCiudades listaCiudades[50];

int cantidadCiudades = 0;

char lwrc(char c){
  if (c >=65 && c<= 90)
  {
    c+=32;
  }
  return c;
}

int stringlen(char string[20]){

  int counter = 0;
  while (string[counter] != '\0')
  {
    counter++;
  }
  return counter;
}

bool compareStrings(char string1[20], char string2[20]){
  bool same = false;

  if (stringlen(string1) == stringlen(string2))
  {
    for (int i = 0; i < stringlen(string1); i++)
    {
      if (lwrc(string1[i])  != lwrc(string2[i]))
      {
        return false;
      }
      same = true;
    }
  }
  return same;
}

void eliminasalto(char c[50]){
    c[strlen(c)-1] = '\0';
}

V2 comprobarLista(Tdistancias distancias){

  V2 res;
  bool found = false;
  for (int i = 0; i < cantidadCiudades; i++)
  {
    if (compareStrings(listaCiudades[i].origen, distancias.ciudad1))
    {
      found = true;
      res.n1 = i;
    }
    if (compareStrings(listaCiudades[i].origen, distancias.ciudad2))
    {
      res.n2 = i;
    }
    
  }
  if (!found)
  {
    strcpy(listaCiudades[cantidadCiudades].origen, distancias.ciudad1);
    res.n1 = cantidadCiudades;
    cantidadCiudades++;
  }
  
  return res;
}

bool nuevaEntradaEnCiudad(Tdistancias distancias, V2 dat){
  
  bool found = false;
  if (0 <= dat.n2)
  {
    
    for (int i = 0; i < listaCiudades[dat.n2].numDestinos; i++)
    {
      if (compareStrings(listaCiudades[dat.n2].destinos[i], distancias.ciudad1))
      {
        found = true;
      }
      
    }
    
  }
  
  if (!found)
  {
    strcpy(listaCiudades[dat.n1].destinos[listaCiudades[dat.n1].numDestinos], distancias.ciudad2);
    listaCiudades[dat.n1].numDestinos++;
    return true;

  }
  
  
  return false;


}


void importar(){
  int debugPrintLines = 0;
  char c[50];
  char firstWord[20] = "\0";
  int wordcounter;
  int startopointo;
  int line = 0;
  int normalLineToIgnore = 47;
  int linesToIgnoreLess=0;
  int sectionOfLines=1;

  kmtxt = fopen("./fich/TablaKM.txt","r");
  kmbin = fopen("./fich/TablaKMB.dat","wb");

  do
  {
    Tdistancias prueba;

    wordcounter = 0;
    startopointo = 0;
    char temp[5] = "\0";
    
    fgets(c,100,kmtxt);
    line++;
    
    for (int i = 0; i < strlen(c) && wordcounter <3; i++)
    {
      if (c[i] == ';')
      {
        for (int j = 0; j < i-startopointo; j++)
        {
          switch (wordcounter)
          {
          case 0:
            prueba.ciudad1[j] = c[startopointo+j];
            break;
          case 1:
            prueba.ciudad2[j] = c[startopointo+j];
            break;
          case 2:
            temp[j] = c[startopointo+j];
            break;
          }
        }
        startopointo = i+1;
        wordcounter++;
      }
      prueba.km = atoi(temp);
      memset(temp,'\0',5);
    
    }
    V2 dat = comprobarLista(prueba);
    
    if (nuevaEntradaEnCiudad(prueba,dat))
    {
      fwrite(&prueba, sizeof(prueba),1,kmbin);
    }

    

  } while (!feof(kmtxt));
    fclose(kmtxt);
  

}

void preguntar(){
  Tdistancias pregunta;
  Tdistancias respuesta;

  printf("Origen: ");
  fgets(pregunta.ciudad1,20,stdin);
  eliminasalto(pregunta.ciudad1);

  printf("Destino: ");
  fgets(pregunta.ciudad2,20,stdin);
  eliminasalto(pregunta.ciudad2);

  kmbin = fopen("./fich/TablaKMB.dat","rb");
  bool found = false;

  while (!found && fread(&respuesta, sizeof(respuesta),1,kmbin))
  {
    if ((compareStrings(pregunta.ciudad1, respuesta.ciudad1) && compareStrings(pregunta.ciudad2, respuesta.ciudad2)) ||
        (compareStrings(pregunta.ciudad1, respuesta.ciudad2) && compareStrings(pregunta.ciudad2, respuesta.ciudad1))) {
        
        found = true;
        printf("\n%s y %s estan a %dkm",respuesta.ciudad1,respuesta.ciudad2,respuesta.km);
    }
  }
  if (!found)
  {
    printf("\nNo se ha encontrado coincidencias.");
  }
}

int main(){
  system("cls");  

  importar();

  preguntar();

  return 0;
}