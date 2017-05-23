#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "disco.h"
#include "directorio.h"
#include <stdbool.h>
#include <errno.h>   // for errno
#include <limits.h>
#include <assert.h>
#include "colores.h"
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

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}



int main(int argc, char *argv[])
{


  printf("\n");
  printf("%s\n","################ INPUT ###################" );
      if(argc <= 1)
      {
        printf("%s\n","Faltan parametros");
        exit(1);
      }

      char *arg;
      char ruta_acciones[256];
      char ruta_disco[256];

      //####### Guardamos la catidad de pasos###########
      int pasos;
      errno = 0;
      long conv = strtol(argv[1], &arg, 10);
      if (conv > INT_MAX)
      {
        printf("%s\n","Error: la cantidad de pasos es mayor a la admitida");
        exit(10);
      }
      else
      {
          pasos = conv;
          printf("%i\n", pasos);
      }
      //###############################################

      strcpy(arg, argv[2]);
      //Caso base, solo crea n acciones y las ejecuta
      if(arg == NULL)
      {
        printf("Caso vacio con %i acciones\n",pasos);
      }

      //Lee las acciones del .txt, si hay menos de las exigidas, genera las que faltan
      else if(!strcmp(arg,"-au"))
      {
        printf("Caso -au, ejecuta %i acciones del txt, si no existen las crea\n",pasos);

        if(argv[3] == NULL)
        {
          printf("%s\n","Error al entregar ruta para acciones.txt");
          exit(10);
        }
        strcpy(ruta_acciones,argv[3]);
      }

      //Ejecuta todas las acciones del txt y luego n mas
      else if(!strcmp(arg,"-a"))
      {
        printf("Caso -a, ejecuta todas acciones del txt, y luego genera %i mas\n",pasos);
        if(argv[3] == NULL)
        {
          printf("%s\n","Error al entregar ruta para acciones.txt");
          exit(10);
        }
        strcpy(ruta_acciones,argv[3]);
      }

      //Procesa lo que hay en el disco y realiza las n acciones sobre este
      else if(!strcmp(arg, "-d"))
      {
        printf("Caso -d, ejecuta %i acciones generadas en el directorio\n",pasos);
        if(argv[3] == NULL)
        {
          printf("%s\n","Error al entregar ruta para disco.txt");
          exit(10);
        }
        strcpy(ruta_disco,argv[3]);
      }

      //Procesa el disco entregado y genera las acciones del txt
      else if(!strcmp(arg, "-da"))
      {
        printf("Caso -da, ejecuta %i acciones del txt, en el directorio.txt\n",pasos);

        if(argv[3] == NULL)
        {
          printf("%s\n","Error al entregar ruta para acciones.txt");
          exit(10);
        }
        strcpy(ruta_acciones, argv[3]);
        if(argv[4] == NULL)
        {
          printf("%s\n","Error al entregar ruta para disco.txt");
          exit(10);
        }
        strcpy(ruta_disco, argv[4]);
      }

      else
      {
        printf("%s\n","Error en la explicitacion de parámetros");
        exit(10);
      }
  printf("%s\n","############ Input Correcto ###################" );
//#############################################################################
  printf("\n");
  printf("%s\n","########## Inicializacion Estructuras###################" );
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


      // char * rutaActual = malloc(256*sizeof(char));
      // strcpy(rutaActual,"root/");

      char * rutaActual = "root/";
      char * accion = malloc(256*sizeof(char));
      strcpy(accion,"mkdir");

      char * rutaRelativa = malloc(256 * sizeof(char));
      strcpy(rutaRelativa,"");


      char * nombre = malloc(256*sizeof(char));
      strcpy(nombre,"...");

      int tamaño = 0; //tamaño en byte para ad y rd
  printf("%s\n","######## Inicializacio Correcta ###############" );
  printf("\n");

  printf("%s\n","######## COMIENZA SIMULACION  ###############" );

  int j = 0;
  /*Leemos el archivo acciones*/
  FILE* file = fopen(argv[3],"r");
  if(file  == NULL){
    printf("%s %c %s %c\n","Error de apertura del archivo ",'"',argv[2],'"');
    printf("%s\n","Revise que la direccion del archivo sea la correcta");
    exit(1);
  }
  while (!feof(file))
  {

    char buff[256];
    printf("%s",CLR_GRN );
    printf("Linea: %i del txt\n",j);
    printf("%s\n",CLR_NORM);


    //Escaneamos la Accion
    printf("%s",CLR_RED);
    fscanf(file, "%s", buff);
    strcpy(accion,buff);
    printf("-- Accion:  %s\n",accion);

    //Ruta completa entregada
    fscanf(file, "%s", buff);
    printf("---- ruta completa: %s \n", buff);

    char** tokens;
    tokens = str_split(buff, '/');
    strcpy(rutaRelativa, *tokens);
    if(*(tokens + 1) == NULL)
    {
      strcpy(nombre, *tokens);
    }
    else if(tokens)
    {
      int i = 1;
        if(*(tokens + i) != NULL)
        {
          for (i = 1; *(tokens + i); i++)
          {
            if((*(tokens + i+1))!=NULL)
            {
              strcat(rutaRelativa,*(tokens + i));
              free(*(tokens + i));
            }
            else
            {
              strcpy(nombre,*(tokens + i) );
              free(*(tokens + i));
            }
          }
          printf("\n");
          free(tokens);
        }
    }


    printf("--------- Ruta Relativa: %s\n",rutaRelativa);
    printf("-------------- Nombre o Ultimo directorio %s\n",nombre);
    printf("%s",CLR_NORM);
    //Ingresamos datos leidos a los parametros de la simulacion

    //Accion mkdir


      // strcpy(accion,"mkdir");
      // strcpy(rutaRelativa,"");
      // strcpy(nombre,"...");
      if(j== 0)
      {
        strcpy(rutaRelativa,"");
        tamaño = 0; //tamaño en byte para ad y rd
        printf("[LINEA COMANDO] %s %s %s\n", accion, rutaRelativa, nombre);
        ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
        printf("\n");

      }

      else
      {
        printf("%s\n",CLR_BLUE);

        printf("[LINEA COMANDO] %s %s %s\n", accion, rutaRelativa, nombre);
        ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
        // strcpy(rutaActual,ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño));
        //rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
        printf("%s\n",CLR_NORM);

      }
      // strcpy(accion,"mkdir");
      // strcpy(rutaRelativa,"...");
      // strcpy(nombre,"d1");
      // tamaño = 0; //tamaño en byte para ad y rd
      // printf("[LINEA COMANDO] %s %s %s\n", accion, rutaRelativa, nombre);
      // ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
      // printf("\n");
      //
      // strcpy(accion,"mkdir");
      // strcpy(rutaRelativa,"...d1");
      // strcpy(nombre,"d2");
      // tamaño = 0; //tamaño en byte para ad y rd
      // printf("[LINEA COMANDO] %s %s %s\n", accion, rutaRelativa, nombre);
      // ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
      // printf("\n");

    // printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);
    // ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
    // printf("\n");
    // accion = "mkdir";
    // rutaRelativa = "...";
    // nombre = "d1";
    // tamaño = 0; //tamaño en byte para ad y rd
    // printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);
    // ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
    // printf("\n");
    // accion = "mkdir";
    // rutaRelativa = "...d1";
    // nombre = "...d2";
    // tamaño = 0; //tamaño en byte para ad y rd
    // printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);
    // ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
    //
    // accion = "cd";
    // rutaRelativa = "...";
    // nombre = "";
    // printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);

    //
    //
    // rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
    //
    // printf("ruta actual: %s\n", rutaActual);
    //
    // accion = "mkfile";
    // rutaRelativa = "";
    // nombre = "fileaagrandar";
    // printf("[LINEA COMANDO] %s %s%s\n", accion, rutaRelativa, nombre);
    //
    //
    //
    // rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
    //
    // accion = "ad";
    // rutaRelativa = "";
    // nombre = "fileaagrandar";
    // tamaño = 6000;
    // printf("[LINEA COMANDO] %s %s%s %d\n", accion, rutaRelativa, nombre, tamaño);
    //
    //
    //
    // rutaActual = ejecutarAccion(accion, rutaRelativa, nombre, rutaActual, dir, simdisk, tamaño);
    //
    j++;
  } //end While
  /*Liberar memoria*/
  return 0;
}
