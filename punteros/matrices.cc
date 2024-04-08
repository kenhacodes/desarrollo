#include <stdio.h>
#include <stdlib.h>

int dimension=0,//Variable para manejar dimensión del bloque 
	*m1,*m2,*m3,*r;

int* CreaMatriz(int d){
  int *res = (int*)malloc((1+(d*d))*sizeof(int));
  
  for (int i = 0; i < 1+(d*d); i++)
  {
    (*(res+i)) = 0;
  }
  (*(res)) = d;
  return res;   
}

void InsertarMatriz(int *m,int row, int col, int num){
  if (m != nullptr)
  {
    (*(m+row * (*(m)) + col+1)) = num;
  }else{
    printf("\n - Error insercion dato matriz. -\n ");
  }
}

void MuestraMatriz(int *m){
  printf("\n-------------\n");
  for (int i = 0; i < (*(m)); i++)
  {
    for (int j = 0; j < (*(m)); j++)
    {
      printf("% 3d ",(*(m + (i* (*(m)) + j)+1)));
    } 
    printf("\n");
  }
  printf("-------------\n");
}

void SumaRestaMatrices(int oper, int *m1, int *m2, int *res){
  if (*(m1) == *(m2))
  {
    for (int i = 0; i < (*(m1))*(*(m1)); i++)
    {
      *(res + i + 1) = *(m1+i+1) + (oper * (*(m2+i+1)));
    }
    *(res) = *(m1);
  }else{
    printf("\n - Suma no posible. Distintas dimensiones -\n");
  }
}

void TrasponerMatriz(int *m, int *res){
  for (int row = 0; row < *(m1); row++)
  {
    for (int col = 0; col < *(m1); col++)
    { 
      *(res + 1 + (col * (*(m1)) + row)) = (*(m1 + 1 + (row * (*(m1)) + col)));
    }
  }
}

int ConsultaMatriz(int *m, int row, int col){
  return (*(m + (row * (*(m)) + col)));
}

void BorraMatriz(int *m){
  free(m);
  m = nullptr;
}

int main(){
	
	m1=CreaMatriz(3);
	m2=CreaMatriz(3);
	//m3 no se crea, es para comprobar la detección de errores al insertar elementos en matriz que no existe.
	r= CreaMatriz(3);
	
	InsertarMatriz(m1,0,0,6);
	InsertarMatriz(m1,0,1,5);
	InsertarMatriz(m1,0,2,10);
	InsertarMatriz(m1,1,0,8);
	InsertarMatriz(m1,1,1,7);
	InsertarMatriz(m1,1,2,4);
	InsertarMatriz(m1,2,0,3);
	InsertarMatriz(m1,2,1,1);
	InsertarMatriz(m1,2,2,2);


	InsertarMatriz(m2,2,0,30);
	InsertarMatriz(m2,2,1,10);
	InsertarMatriz(m2,1,2,5);
	InsertarMatriz(m2,1,0,18);

	InsertarMatriz(m3,2,2,20);

	
	printf("\nMatriz 1:");
	MuestraMatriz(m1);
  
	printf("\nMatriz 2:");
	MuestraMatriz(m2);
	printf("\nOperacion realizada sobre matrices:\n\n");
	SumaRestaMatrices(+1,m1,m2,r);
	printf("\nMatriz Resultado:");
	MuestraMatriz(r);
	
  
  printf("\nMatriz m1 Traspuesta:");
	TrasponerMatriz(m1,r);
	MuestraMatriz(r);
	printf("\nAcceso directo a elemento en posicion (1,2):");
	
	printf("\n%d \n",ConsultaMatriz(r,1,2));

	BorraMatriz(m1);
  BorraMatriz(m2);
  BorraMatriz(r);
}
