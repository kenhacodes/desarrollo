#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TContacto{
  char nombre[25],apellido1[25],apellido2[25], email[50], numero[11];
};

FILE* agendaBin;
FILE* tempBin;

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

void newContact(){

  TContacto contacto;

  printf("\nNew Contact.\nName:");
  fgets(contacto.nombre,25,stdin);
  eliminasalto(contacto.nombre);

  printf("1st Surname:");
  fgets(contacto.apellido1,25,stdin);
  eliminasalto(contacto.apellido1);

  printf("2nd Surname:");
  fgets(contacto.apellido2,25,stdin);
  eliminasalto(contacto.apellido2);

  printf("Email:");
  fgets(contacto.email,25,stdin);
  eliminasalto(contacto.email);

  printf("Phone Number:");
  fgets(contacto.numero,11,stdin);
  eliminasalto(contacto.numero);

  agendaBin = fopen("./fich/agenda.dat","ab");

  fwrite(&contacto,sizeof(contacto), 1, agendaBin);

  fclose(agendaBin);
  
  printf("\n\n - New contact created. - \nName: %s\nSurnames: %s %s\nEmail: %s\nNumber: %s",contacto.nombre,contacto.apellido1,contacto.apellido2,contacto.email, contacto.numero);
}

void deleteContact(){

  TContacto contacto;
  bool found = false;

  char deleteNum[25] = "\0";

  printf("\nDelete Contact.\nNumber:");
  
  fgets(deleteNum,11,stdin);
  eliminasalto(deleteNum);

  agendaBin = fopen("./fich/agenda.dat","r+b");
  tempBin = fopen("./fich/temp.dat","wb");
  while (fread(&contacto, sizeof(contacto),1,agendaBin))
  {
    if (!compareStrings(contacto.numero, deleteNum))
    {
      fwrite(&contacto,sizeof(contacto), 1, tempBin);
    }else{
      found = true;
      printf("\n %s (%d)!= \n %s (%d)", contacto.numero,strlen(contacto.numero), deleteNum,strlen(deleteNum));
    }
  }
  fclose(agendaBin);
  fclose(tempBin);
  if (found)
  {
    remove("./fich/agenda.dat");
    rename("./fich/temp.dat", "./fich/agenda.dat");
    printf("Contacto eliminado.");
  }else{
    printf("Contacto no encontrado.");
    remove("./fich/temp.dat");
  }
  
}

void modifyContact(){
  TContacto contacto;
  bool changed = false;
  char ModifyNum[25] = "\0";
  char selection[1];
  printf("\nModify Contact.\nNumber:");
  
  fgets(ModifyNum,11,stdin);
  eliminasalto(ModifyNum);
  agendaBin = fopen("./fich/agenda.dat","r+b");
  while (fread(&contacto, sizeof(contacto),1,agendaBin) && !changed)
  {
    if (compareStrings(contacto.numero, ModifyNum))
    {
      printf("What do you want to modify?\n 1. Name ...\n 2. Surname1 ...\n 3. Surname2 ...\n 4. Email ...\n 5. Number ...\n - ");

      //fgets(selection,2,stdin);
      
      
    
      changed = true;
      
    }
    
  }
  
}

void showList(){
  
  TContacto contacto;
  printf("           LIST:\n-----------------------------\n");
  agendaBin = fopen("./fich/agenda.dat","rb");
  while (fread(&contacto, sizeof(contacto),1,agendaBin))
  {
    printf(" Name:     %s\n Surnames: %s %s\n Email:    %s\n Number:   %s\n-----------------------------\n",contacto.nombre,contacto.apellido1,contacto.apellido2,contacto.email, contacto.numero);
  }
  fclose(agendaBin);
}

void menu(){
  
  char selection[1];
  do
  {
    system("cls");
    
    printf("\nAGENDA DE CONTACTOS\n1 ... New contact.\n2 ... Delete contact.\n3 ... Modify Contact (No terminado).\n4 ... Show List.\n5 ... Quit.\n\n");
    fgets(selection,2,stdin);
    switch (selection[0])
    {
    case '1':
      fgets(selection,2,stdin);
      newContact();
      break;
    case '2':
      fgets(selection,2,stdin);
      deleteContact();
      break;
    case '3':
      fgets(selection,2,stdin);
      modifyContact();
      break;
    case '4':
      fgets(selection,2,stdin);
      showList();
      break;
    case '5':
      printf("Bye!");
      break;
    default:
      printf("ERROR");
    break;
  }
  fgets(selection,2,stdin);
  } while (selection[0] != '5');
  
  
}

int main(){

  if (fopen("./fich/agenda.dat","rb") == NULL)
  {
    agendaBin = fopen("./fich/agenda.dat","wb");
    fclose(agendaBin);
  } 
  
  menu();

  return 0;
}