#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "disco.h"
#include "directorio.h"
#include <stdbool.h>
//#include <conio.h>

void poblarDisco(Disco *simdisk, int cant_bloques){
  /*la primera posicion en el disco es la carpeta root*/
  InsercionEnListaVaciaDisco(simdisk, "00010000");
  int i = 0;
  while (i < cant_bloques) {
    InsercionFinListaDisco(simdisk, simdisk->fin, "00100000");
    i++;
  }
}

void mkfile(Directorio *dir, Disco *simdisk, char * ruta, char * nombre){
  EntradaDirectorio *entrada;
  entrada = buscarEntradaDir(dir, ruta);
  if (entrada) {
    printf("El archivo %s ya existe\n", ruta);
  }
  else{
    EntradaDisco *primerBloque;
    primerBloque = buscarEntradaDiscoLibre(simdisk);
    if(!primerBloque){
      printf("No queda ningun bloque disponible\n");
    }
    else{
      int size = 1;
      //creamos la entrada en directorio
      if (dir->tamano == 0){
        InsercionEnListaVaciaDir(dir, ruta, primerBloque->indice, size, nombre);
      }
      else{
        InsercionFinListaDir(dir, ruta, primerBloque->indice, size, nombre);
      }

      //metadata cambia a contenido y refencia a EOF
      primerBloque->metadata = "00001000"; // contenido
      primerBloque->referencia = "EOF";
      visualizacionIndiceDisco(simdisk, primerBloque->indice);
      visualizacionDir(dir);
    }
}
}

void mkdir(Directorio *dir, Disco *simdisk, char * ruta, char * nombre){

  EntradaDirectorio *entrada;
  entrada = buscarEntradaDir(dir, ruta);
  if (entrada) {
    printf("El directorio %s ya existe\n", ruta);
  }
  else{
    EntradaDisco *primerBloque;
    primerBloque = buscarEntradaDiscoLibre(simdisk);
    if(!primerBloque){
      printf("No queda ningun bloque disponible\n");
    }
    else{
      int size = 4096;
      //creamos la entrada en directorio
      if (dir->tamano == 0){
        InsercionEnListaVaciaDir(dir, ruta, primerBloque->indice, size, nombre);
      }
      else{
        InsercionFinListaDir(dir, ruta, primerBloque->indice, size, nombre);
      }

      //metadata cambia a contenido y refencia a EOF
      primerBloque->metadata = "00010000";
      primerBloque->referencia = "ni idea que va aca";
      visualizacionIndiceDisco(simdisk, primerBloque->indice);
      visualizacionDir(dir);
    }
}
}

bool rutaCorrecta(char * rutaRelativa, char * rutaActual, Directorio *dir){
  /*verificar que la ruta relativa exista-->buscar el "1er directorio", "2do directorio", etc...
  si existen todos -> return true*/
  char * concat = malloc(strlen(rutaRelativa) + strlen(rutaActual) + 1);
  strcpy(concat, rutaActual);
  strcat(concat, rutaRelativa);
  printf("rutaActual: %s\n", concat);
  if(buscarEntradaDir(dir, concat) != NULL){
    printf("la ruta ingresada existe\n");
    return true;
  }
  printf("la ruta ingresada no es correcta\n");
  return false;
}

char * cd(char * rutaRelativa, char * rutaActual){
  char * nuevarutaactual = malloc(strlen(rutaRelativa) + strlen(rutaActual) + 1);
  strcpy(nuevarutaactual, rutaActual);
  strcat(nuevarutaactual, rutaRelativa);

  return nuevarutaactual;
}

int main(int argc, char *argv[])
{
  FILE* file = fopen(argv[1],"r");

   //Caso de error en lectura del archivo
   if(file  == NULL)
   {
       printf("%s %c %s %c\n","Error de apertura del archivo ",'"',argv[2],'"');
       printf("%s\n","Revise que la direccion del archivo sea la correcta");
       exit(1);
   }
  char * rutaActual = "root/";
  while (!feof(file)){

  /*Poblamos el simdisk*/
  Disco *simdisk;
  simdisk = (Disco *) malloc (sizeof (Disco));
  incializacionDisco(simdisk);
  int cant_bloques = pow(2, 20);


  //se guarda dir.txt
  poblarDisco(simdisk, cant_bloques);
  visualizacionIndiceDisco(simdisk,0);


  /*Creamos el directorio*/
  Directorio *dir;
  dir = (Directorio *) malloc (sizeof (Directorio));
  incializacionDir(dir);
  visualizacionDir(dir);
  char *nombre;
  char buff[256];
  char rutaRelativa[256];
  char accion[256];


  fscanf(file,"%s",buff);
  strcpy(accion,buff);
  printf("%s\n",accion);
  // /*Acciones*/
  fscanf(file,"%s",rutaRelativa);

  strtok_r (rutaRelativa, "/", &nombre);

  printf("ruta relativa %s\n",rutaRelativa);
  printf("nombre %s\n",nombre);

  char * nueva_ruta = malloc(strlen(rutaRelativa) + strlen(rutaActual) + strlen(nombre) + 1);
  strcpy(nueva_ruta, rutaActual);
  strcat(nueva_ruta, rutaRelativa);
  strcat(nueva_ruta, nombre);
  printf("ruta a buscar: %s\n", nueva_ruta);

  if (strcmp(accion, "mkfile") == 0) {
    printf("[ACCION] %s %s\n", accion, nombre);
    /*revisar si la ruta relativa esta bien*/
    if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
      mkfile(dir, simdisk, nueva_ruta, nombre);
    }
  } else if (strcmp(accion, "mkdir") == 0) {
    /*revisar si la ruta relativa esta bien*/
    printf("[ACCION] %s %s\n", accion, nombre);
    if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
      mkdir(dir, simdisk, nueva_ruta, nombre);
    }
  } else if (strcmp(accion, "cd") == 0) {
    /*mantener una ruta actual*/
    printf("[ACCION] %s %s\n", accion, nombre);
    if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
      rutaActual = cd(rutaRelativa, rutaActual);
    }

  } else if (strcmp(accion, "mv") == 0) {
    /*cambiar la ruta relativa*/
    printf("[ACCION] %s %s\n", accion, nombre);
  } else if (strcmp(accion, "rm") == 0) {
    /*borrar un archivo: dejar libre todos sus bloques y borrarlo del directorio*/
    printf("[ACCION] %s %s\n", accion, nombre);
    if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
      //rm(rutaRelativa, rutaActual);
    }

  } else if (strcmp(accion, "ad") == 0) {
    /*agrega size->actualiza cantidad de bloques, tanto en dir como disco
    podria necesitar mas bloques->cambia la referencia del ultimo bloque y la metadata si se ocupan nuevos bloques*/
    printf("[ACCION] %s %s\n", accion, nombre);
  } else if (strcmp(accion, "rd") == 0) {
    /*disminuye size->actualiza cantidad de bloques, tanto en dir como disco
    podria necesitar menos bloques->cambia la referencia del ultimo y la metadata si se liberan bloques*/
    printf("[ACCION] %s %s\n", accion, nombre);
  }
  /*Liberamos memoria*/
  //EntradaDisco *actual;
  //actual = simdisk->inicio;
  //while (actual->siguiente != NULL) {
    //freeElementoListaDisco(actual);
    //actual = actual->siguiente;
  //}
}
  return 0;
}
