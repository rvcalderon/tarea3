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
      primerBloque->referencia = -1;
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
      primerBloque->referencia = -2;
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
  //printf("ruta buscada: %s\n", concat);
  if(buscarEntradaDir(dir, concat) != NULL){
    //printf("%s existe\n", concat);
    return true;
  }
  printf("%s no existe\n", concat);
  return false;
}

char * cd(char * rutaRelativa, char * rutaActual){
  char * nuevarutaactual = malloc(strlen(rutaRelativa) + strlen(rutaActual) + 1);
  strcpy(nuevarutaactual, rutaActual);
  strcat(nuevarutaactual, rutaRelativa);

  return nuevarutaactual;
}

void ad(char * nombre, int tamaño, Directorio *dir, Disco *simdisk){
  EntradaDirectorio *entrada;
  entrada = buscarEntradaDirNombre(dir, nombre);
  if(entrada!=NULL){
    printf("size de la entrada: %d\n", entrada->size);
    int cantidad_bloques = entrada->size/4000;
    if(entrada->size%4000>0){
      cantidad_bloques =cantidad_bloques +1;
    }
    entrada->size = entrada->size + tamaño;
    int nueva_cantidad_bloques = entrada->size/4000;
    if(entrada->size%4000>0){
      nueva_cantidad_bloques =nueva_cantidad_bloques +1;
    }
    int nuevos_bloques = nueva_cantidad_bloques - cantidad_bloques;
    printf("se necesitan %d bloques nuevos\n", nuevos_bloques);
    int primerBloque = entrada->primerBloque;
    EntradaDisco * anterior;
    anterior = ultimobloque(primerBloque, cantidad_bloques, simdisk);
    for (int i = 0; i < nuevos_bloques; ++i)
    {
      /* agregar nuevos bloques-> ir al ultimo y poner el siguiente*/

      EntradaDisco *libre = buscarEntradaDiscoLibre(simdisk);
      libre->metadata = "00001000"; //conenido

      if (i == nuevos_bloques-1){
        libre->referencia = -1;
      }
      else{



        anterior->referencia = libre->indice;
        anterior = libre;
      }
      printf("Agregado nuevo bloque: indice=%d metadata=%s referencia=%d\n", libre->indice, libre->metadata, libre->referencia);
    }
    }

}

char * ejecutarAccion(char * accion, char * rutaRelativa, char * nombre, char * rutaActual, Directorio *dir, Disco *simdisk, int tamaño){
    char * nueva_ruta = malloc(strlen(rutaRelativa) + strlen(rutaActual) + strlen(nombre) + 2);
    strcpy(nueva_ruta, rutaActual);
    strcat(nueva_ruta, rutaRelativa);
    //strcat(nueva_ruta, "/");
    strcat(nueva_ruta, nombre);
    //printf("nueva ruta a guardar (si es correcta): %s\n", nueva_ruta);

  if (strcmp(accion, "mkfile") == 0) {
      /*revisar si la ruta relativa esta bien*/
      if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
        mkfile(dir, simdisk, nueva_ruta, nombre);
      }
    } else if (strcmp(accion, "mkdir") == 0) {
      /*revisar si la ruta relativa esta bien*/
      if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
        mkdir(dir, simdisk, nueva_ruta, nombre);
      }
    } else if (strcmp(accion, "cd") == 0) {
      /*mantener una ruta actual*/
      if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
        rutaActual = cd(rutaRelativa, rutaActual);
      }

    } else if (strcmp(accion, "mv") == 0) {
      /*cambiar la ruta relativa*/
      // if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
      //   mv(rutaRelativa, rutaActual);
      // }
    } else if (strcmp(accion, "rm") == 0) {
      /*borrar un archivo: dejar libre todos sus bloques y borrarlo del directorio*/
      // if(rutaCorrecta(rutaRelativa, rutaActual, dir)){
      //   rm(rutaRelativa, rutaActual);
      // }

    } else if (strcmp(accion, "ad") == 0) {
      /*agrega size->actualiza cantidad de bloques, tanto en dir como disco
      podria necesitar mas bloques->cambia la referencia del ultimo bloque y la metadata si se ocupan nuevos bloques*/
      ad(nombre, tamaño, dir, simdisk);

    } else if (strcmp(accion, "rd") == 0) {
      /*disminuye size->actualiza cantidad de bloques, tanto en dir como disco
      podria necesitar menos bloques->cambia la referencia del ultimo y la metadata si se liberan bloques*/
    }
    return rutaActual;
}

int main(int argc, char *argv[])
{

  /*Leemos el archivo acciones*/
  FILE* file = fopen(argv[1],"r");
  if(file  == NULL){
    printf("%s %c %s %c\n","Error de apertura del archivo ",'"',argv[2],'"');
    printf("%s\n","Revise que la direccion del archivo sea la correcta");
    exit(1);
  }



  /*Poblamos el simdisk*/
  Disco *simdisk;
  simdisk = (Disco *) malloc (sizeof (Disco));
  incializacionDisco(simdisk);
  int cant_bloques = pow(2, 20);
  poblarDisco(simdisk, cant_bloques);
  visualizacionIndiceDisco(simdisk,0);

  /*Creamos el directorio*/
  Directorio *dir;
  dir = (Directorio *) malloc (sizeof (Directorio));
  incializacionDir(dir);
  InsercionEnListaVaciaDir(dir, "root/", 0, 4096, "root");
  visualizacionDir(dir);


  char * rutaActual = "root/";

  /*Por cada accion leida*/
  //while (!feof(file)){
    /*Guardamos la accion, nombre, rutaRelativa[CORREGIR QUE NO ENTREGUEN RUTA RELATIVA]*/
    // char *nombre;
    // char buff[256];
    // char rutaRelativa[256];
    // char accion[256];
    // //fscanf(file,"%s",buff);
    //strcpy(accion,buff);
    //printf("accion: %s\n",accion);
    //fscanf(file,"%s",rutaRelativa);
    //strtok_r (rutaRelativa, "/", &nombre);

    char * accion = "mkdir";
    char * rutaRelativa = "";
    char * nombre = "...";
    int tamaño = 0; //tamaño en byte para ad y rd
    printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);



    rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);

    accion = "cd";
    rutaRelativa = "...";
    nombre = "";
    printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);



    rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);

    printf("ruta actual: %s\n", rutaActual);

    accion = "mkfile";
    rutaRelativa = "";
    nombre = "fileaagrandar";
    printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);



    rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);

    accion = "ad";
    rutaRelativa = "";
    nombre = "fileaagrandar";
    tamaño = 6000;
    printf("[LINEA COMANDO] %s %s%s %d\n", accion, rutaRelativa, nombre, tamaño);



    rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);


  //} end While
  /*Liberar memoria*/
  return 0;
}
