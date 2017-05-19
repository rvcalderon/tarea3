
//#include <stdint.h>

//http://probacodigos.blogspot.cl/2012/09/ejemplos-c-lista-enlazada.html
typedef struct NodoDisco{
    char * metadata;
    char * referencia;
    struct NodoDisco *siguiente;
    int indice;
}EntradaDisco;

typedef struct ListaDisco{
    EntradaDisco *inicio;
    EntradaDisco *fin;
    int tamano;
}Disco;

/*inicializa la lista*/
void incializacionDisco(Disco *lista);
/*En caso de error devuelve -1 sino 0*/
int InsercionEnListaVaciaDisco(Disco *lista, char *metadata);
/*insertar en fin de la lista*/
int InsercionFinListaDisco(Disco *lista, EntradaDisco *actual, char *metadata);
 /*muestra la lista entera*/
void visualizacionDisco(Disco *lista);
//Una lista enlazada sin ningun elemento se llama lista vacía.
//Su puntero inicial tiene el valor NULL
//Si una lista es de 1 solo elemento, el campo siguiente apunta a NULL
void visualizacionIndiceDisco(Disco *lista, int i);
EntradaDisco* buscarEntradaDiscoLibre(Disco *lista);

/*libera memoria*/
void freeElementoListaDisco(EntradaDisco *actual);
