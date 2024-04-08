#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* donqui;

char word[25] = "\0";

char lwrc(char c){
  if (c >=65 && c<= 90)
  {
    c+=32;
  }
  return c;
}

int stringlen(char string[25]){

  int counter = 0;
  while (string[counter] != '\0')
  {
    counter++;
  }
  return counter;
}

bool compareStrings(char string1[25], char string2[25]){
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



int search(char word[20]){

  int repetitions = 0;

  char line[100];

  donqui = fopen("./fich/donqui.txt", "r");

  do
  {
    char refword[25];
    
    int startopointo = 0;

    fgets(line, 100, donqui);

    for (int i = 0; i < strlen(line); i++)
    {
      if (lwrc(line[i]) < 97 || lwrc(line[i]) > 122)
      {

        for (int j = 0; j < i - startopointo; j++)
        {
          refword[j] = line[startopointo+j];
        }
        
        if (compareStrings(refword, word))
        {
          repetitions++;
        }
        memset(refword,'\0',25);
        startopointo=i+1;
      }
      
    }
    
    

  } while (!feof(donqui));
  fclose(donqui);

  return repetitions;
}

int main(){
  system("cls"); 
  printf("\nEscribe la palabra a buscar: ");
  fgets(word,20,stdin);
  eliminasalto(word);

  printf("En el Don Quijote la palabra \"%s\" se repite %d veces. ;)\n",word,search(word));

  


  return 0;
}