
//#include <stdint.h>

//http://probacodigos.blogspot.cl/2012/09/ejemplos-c-lista-enlazada.html
typedef struct NodoDir{
    char * nombre;
    char * ruta;
    int primerBloque;
    int size;
    struct NodoDir *siguiente;
    int indice;
}EntradaDirectorio;

typedef struct ListaDir{
    EntradaDirectorio *inicio;
    EntradaDirectorio *fin;
    int tamano;
}Directorio;

/*inicializa la lista*/
void incializacionDir(Directorio *lista);
/*En caso de error devuelve -1 sino 0*/
int InsercionEnListaVaciaDir(Directorio *lista, char *ruta, int primerBloque, int size, char *nombre);
/*insertar en fin de la lista*/
int InsercionFinListaDir(Directorio *lista, char * ruta, int primerBloque, int size, char *nombre);
 /*muestra la lista entera*/
void visualizacionDir(Directorio *lista);
//Una lista enlazada sin ningun elemento se llama lista vacía.
//Su puntero inicial tiene el valor NULL
//Si una lista es de 1 solo elemento, el campo siguiente apunta a NULL
void visualizacionIndiceDir(Directorio *lista, int i);

EntradaDirectorio* buscarEntradaDir(Directorio *lista, char * nombre);

EntradaDirectorio* buscarEntradaDirNombre(Directorio *lista, char * nombre);
/*libera memoria*/
void freeElementoListaDir(EntradaDirectorio *actual);
