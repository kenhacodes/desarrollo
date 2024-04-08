//Declaración tipo lista doblemente enlazada y 
//prototipos de funciones para realizar el ejercicio planteado en clase.
#include <stdio.h>
#include <stdlib.h>

struct TNodo{
	int info;
	TNodo *anterior,*proximo;
};


#define TLista2 TNodo 

TNodo *CrearLista();
bool EsVaciaLista(TNodo *Lista);
int LongitudLista(TNodo *lista);
void InsertarLista(TNodo **Lista, int valor);
void MostrarLista(TNodo *Lista);
TNodo *BuscarEnLista(TNodo *Lista, int valor);
TNodo *ExtraerLista(TNodo **Lista);
void EliminarElemento(TNodo **Lista, int valor);
TNodo *UltimoElemento(TNodo *Lista);
void InvertidoMostrarLista(TNodo *Lista);

TNodo *CrearLista(){
	TNodo *node = (TNodo*) malloc(sizeof(TNodo));
	node = nullptr;
	return node;
};

bool EsVaciaLista(TNodo *Lista){

	return Lista == nullptr;
	
}

int LongitudLista(TNodo *lista){
	int l = 0;
	TNodo *p = lista;
	while ((p)->anterior != nullptr)
	{
		p = (p)->anterior;
	}
	while ((p)->proximo != nullptr)
	{
		l++;
		p = (p)->proximo;
	}
	return l;
	
}

void InsertarLista(TNodo **Lista, int valor){
	
	TNodo *newNode = (TNodo*)malloc(sizeof(TNodo));
	TNodo *p = (*Lista);

	(newNode)->info = valor;
	(newNode)->anterior = nullptr;	
	(newNode)->proximo = p;
	
	if (!EsVaciaLista(p)) (p)->anterior = newNode;

	(*Lista) = newNode;	
}

void MostrarLista(TNodo *Lista){
	TNodo *p = Lista;
	while (p != nullptr)
	{
		printf(" %0d", (p)->info);
		p = (p)->proximo;
	}
	
}

TNodo *BuscarEnLista(TNodo *Lista, int valor){
	TNodo *p = Lista;

	while (p != nullptr)
	{
		if ((p)->info == valor) return p;
		p = (p)->proximo;
	}
	return nullptr;
}

TNodo *ExtraerLista(TNodo **Lista){
	TNodo *p = (*Lista);
	TNodo *l = (p)->proximo;
	(*Lista) = l;
	return p;
}

void EliminarElemento(TNodo **Lista, int valor){
	
	TNodo *p = (*Lista);
	
	while(p != nullptr){
		
		if ((p)->info == valor)
		{
			if ((p)->anterior != nullptr) (p)->anterior->proximo = (p)->proximo;
			if ((p)->proximo  != nullptr) (p)->proximo->anterior = (p)->anterior;
			
			free(p);
			p = (*Lista);
		}else{
			p = (p)->proximo;
		}
		
	}
}

TNodo *UltimoElemento(TNodo *Lista){
	TNodo *p = Lista;
	while (p!= nullptr)
	{
		p = (p)->proximo;
	}
	return (p)->anterior;
}

void InvertidoMostrarLista(TNodo *Lista){
	TNodo *p = Lista;

	while((p)->proximo != nullptr){
		p = (p)->proximo;
	}

	while (p != NULL)
	{
		printf(" %d", (p)->info);
		p = (p)->anterior;
	}
	

}




