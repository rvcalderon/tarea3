#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "disco.h"


/*Inicializar una lista*/
void incializacionDisco(Disco *lista){
    lista->inicio = NULL;
    lista->fin= NULL;
    lista->tamano = 0;
}

/*Inserción en una lista vacía */
int InsercionEnListaVaciaDisco(Disco *lista, char *metadata){
    EntradaDisco *nuevo_elemento;
    if((nuevo_elemento = (EntradaDisco *)malloc(sizeof(EntradaDisco)))==NULL)
        return -1;
    if((nuevo_elemento->metadata = (char *)malloc(50*sizeof(char)))==NULL)
        return -1;
    strcpy(nuevo_elemento->metadata, metadata);

    nuevo_elemento->referencia = "-1";
    nuevo_elemento->siguiente = NULL;
    lista->inicio = nuevo_elemento;
    lista->fin = nuevo_elemento;
    lista->tamano++;
    lista->inicio->indice = 0;
    return 0;
}

/*Insercion al final de la lista */
int InsercionFinListaDisco(Disco *lista, EntradaDisco *actual, char *metadata){
    EntradaDisco *nuevo_elemento;
    if((nuevo_elemento = (EntradaDisco *)malloc(sizeof(EntradaDisco)))==NULL)
        return -1;
    if((nuevo_elemento->metadata = (char *)malloc(50*sizeof(char)))==NULL)
        return -1;
    
    strcpy(nuevo_elemento->metadata,metadata);

    nuevo_elemento->referencia = NULL;
    actual->siguiente = nuevo_elemento;
    nuevo_elemento->siguiente = NULL;
    nuevo_elemento->indice = lista->tamano;
    lista->fin = nuevo_elemento;
    lista->tamano++;
    return 0;
}

/*visualizar lista entera*/
void visualizacionDisco(Disco *lista){
    EntradaDisco *actual;
    actual = lista->inicio;
    while(actual != NULL){
            //printf("%p - %s - %s - %d\n",actual,actual->metadata, actual->referencia, actual->indice);
            printf("[SIMDISK] metadata:%s referencia:%d bloque:%d\n",actual->metadata, actual->referencia, actual->indice);
            actual = actual->siguiente;
        }
}

/*visualizar lista entera*/
void visualizacionIndiceDisco(Disco *lista, int i){
    EntradaDisco *actual;
    actual = lista->inicio;
    while(actual != NULL){
            if (actual->indice == i) {
              //printf("[SIMDISK] %p - %s - %s\n",actual,actual->metadata, actual->referencia);
              printf("[SIMDISK] metadata:%s referencia:%d bloque:%d\n",actual->metadata, actual->referencia, actual->indice);
              break;
            }
            actual = actual->siguiente;
        }
}

EntradaDisco* buscarEntradaDiscoLibre(Disco *lista){
  EntradaDisco *actual;
  EntradaDisco *libre;
  actual = lista->inicio;
  while(actual != NULL){
          if (strcmp(actual->metadata, "00100000") == 0) {
            libre = actual;
            return libre;
          }
          actual = actual->siguiente;
      }
  printf("No quedan bloques libres\n");
  return actual;
}

void freeElementoListaDisco(EntradaDisco *actual) {
  free(actual->metadata);
  free(actual->referencia);
  free(actual);
}


EntradaDisco * ultimobloque(int primerBloque, int cantidad_bloques, Disco *lista){
  EntradaDisco *actual;
  actual = lista->inicio;
  int i = primerBloque;
  while(actual != NULL){
          if (actual->indice == i) {
            if(actual->referencia == "-1"){
              return actual;
            }
            else{
              i = actual->referencia;
              actual = lista->inicio;
            }
          }
          actual = actual->siguiente;
      }
}

