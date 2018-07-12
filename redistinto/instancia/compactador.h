#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include "instancia.h"
#include <commons/collections/list.h>


int espacioUsado;
int entradasOcupadas;



void compactar();
int cantidad_entradas_libres();
int obtener_ultima_entrada_libre();


#endif
