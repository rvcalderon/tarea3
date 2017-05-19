#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "directorio.h"


/*Inicializar una lista*/
void incializacionDir(Directorio *lista){
    lista->inicio = NULL;
    lista->fin= NULL;
    lista->tamano = 0;
}

/*Inserción en una lista vacía */
int InsercionEnListaVaciaDir(Directorio *lista, char * ruta, int primerBloque, int size, char *nombre){
    EntradaDirectorio *nuevo_elemento;
    if((nuevo_elemento = (EntradaDirectorio *)malloc(sizeof(EntradaDirectorio)))==NULL)
        return -1;
    if((nuevo_elemento->nombre = (char *)malloc(50*sizeof(char)))==NULL)
        return -1;
    if((nuevo_elemento->ruta = (char *)malloc(50*sizeof(char)))==NULL)
        return -1;
    strcpy(nuevo_elemento->nombre, nombre);
    strcpy(nuevo_elemento->ruta, ruta);

    nuevo_elemento->primerBloque = primerBloque;
    nuevo_elemento->size = size;
    nuevo_elemento->siguiente = NULL;
    lista->inicio = nuevo_elemento;
    lista->fin = nuevo_elemento;
    lista->tamano++;
    lista->inicio->indice = 0;
    return 0;
}

/*Insercion al final de la lista */
int InsercionFinListaDir(Directorio *lista, char* ruta, int primerBloque, int size, char *nombre){
    EntradaDirectorio *nuevo_elemento;
    EntradaDirectorio *actual;
    actual = lista->fin;
    if((nuevo_elemento = (EntradaDirectorio *)malloc(sizeof(EntradaDirectorio)))==NULL)
        return -1;
    if((nuevo_elemento->nombre = (char *)malloc(50*sizeof(char)))==NULL)
        return -1;
    if((nuevo_elemento->ruta = (char *)malloc(50*sizeof(char)))==NULL)
        return -1;
    strcpy(nuevo_elemento->nombre, nombre);
    strcpy(nuevo_elemento->ruta, ruta);

    nuevo_elemento->primerBloque = primerBloque;
    nuevo_elemento->size = size;
    actual->siguiente = nuevo_elemento;
    nuevo_elemento->siguiente = NULL;
    nuevo_elemento->indice = lista->tamano;
    lista->fin = nuevo_elemento;
    lista->tamano++;
    return 0;
}

/*visualizar lista entera*/
void visualizacionDir(Directorio *lista){
    EntradaDirectorio *actual;
    actual = lista->inicio;
    while(actual != NULL){
            printf("[DIR] ruta:%s primerBloque:%d size:%d\n",actual->ruta, actual->primerBloque, actual->size);
            actual = actual->siguiente;
        }
}

/*visualizar un elemento de la lista*/
void visualizacionIndiceDir(Directorio *lista, int i){
    EntradaDirectorio *actual;
    actual = lista->inicio;
    while(actual != NULL){
            if (actual->indice == i) {
              printf("[DIR] ruta:%s primerBloque:%d size:%d\n",actual->ruta, actual->primerBloque, actual->size);
              break;
            }
            actual = actual->siguiente;
        }
}

EntradaDirectorio* buscarEntradaDir(Directorio *lista, char * ruta){
  EntradaDirectorio *encontrado;
  encontrado = NULL;
  EntradaDirectorio *actual = lista->inicio;
  while(actual!= NULL){
    if (strcmp(ruta, actual->ruta)==0) {
      encontrado = actual;
      break;
    }
    actual = actual->siguiente;
  }
  return encontrado;
}

EntradaDirectorio* buscarEntradaDirNombre(Directorio *lista, char * nombre){
  EntradaDirectorio *encontrado;
  encontrado = NULL;
  EntradaDirectorio *actual = lista->inicio;
  while(actual!= NULL){
    if (strcmp(nombre, actual->nombre)==0) {
      encontrado = actual;
      break;
    }
    actual = actual->siguiente;
  }
  return encontrado;
}

void free_elemento_listaDir(EntradaDirectorio *actual) {
  free(actual->nombre);
  free(actual);
}
