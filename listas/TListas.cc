struct TLista
{
  int info;
  TLista *prox;
};

void CrearLista(TLista **lista){
  //*(lista) = (TLista*)malloc(sizeof(TLista));
  *(lista) = nullptr;
}

bool isEmptyList(TLista *l){
  return (l==NULL);
}

int InsertarLista(TLista **lista, int num){
  TLista *nuevo;

  if ((nuevo = (TLista*) malloc (sizeof(TLista))) == nullptr){
    return 0;
  }else{
    nuevo->info = num;
    nuevo->prox = nullptr;
    if (isEmptyList(*lista))
    {
      *lista = nuevo;
    }else{
      nuevo->prox =*lista;
      *lista = nuevo;
    }
    return 1;
  }
}

void MuestraLista(TLista *lista){
  
  TLista *pointer = lista;
  while (!isEmptyList(pointer))
  {
    printf("%d, ", pointer->info);
    pointer = pointer->prox; 
  } 
  
}

TLista* IndexaLista(TLista *lista, int num){
  return lista;
}

TLista* BuscarEnLista(TLista *lista, int valor, int option){
  TLista *p;
  bool found = false;

  if (!isEmptyList(lista)){
  p = lista;
  do
  {
    switch (option)
    {
    case  0: found = (p->info == valor); break;
    case -1: found = (p->prox->info == valor); break;
    }
    if (!found)
    {
      p = p->prox;
    }
  } while (!found && p != nullptr);
  return p;
  }
  

  return lista;
}

void EliminaEnLista(TLista **lista, int num){
  TLista *p, *q;
  if (!isEmptyList(*lista)){
    p = *lista;
    if (p ->info == num){
      *lista = p->prox;
      free(p);
    }else{
      p = BuscarEnLista(*lista, num, -1);
      q = p->prox;
      if (p!=nullptr){
        p->prox = q->prox;
        free(q);
      }
    }
  }
}