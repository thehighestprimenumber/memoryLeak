#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include "instancia.h"
#include <commons/collections/list.h>


int espacioUsado;
int entradasOcupadas;


int entradas_que_ocuparia(t_clave_valor *entrada);
int tam_min_entrada(t_clave_valor *entrada);
void loguearEntradas();
void compactar();
int cantidad_entradas_libres();
int obtener_ultima_entrada_libre();
void ordenar_lista_entradas();


#endif
