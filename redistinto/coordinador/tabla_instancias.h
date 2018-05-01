#ifndef TABLA_INSTANCIAS_H_
#define TABLA_INSTANCIAS_H_

#include <commons/collections/list.h>
#include "coordinador.h"

typedef struct {
	int socket_inst;
	t_list* claves;
	int esta_activa;
} fila_tabla_instancias;

int ultima_instancia_usada;

int desactivar_instancia(int id_instancia);

int activar_instancia(int socket_instancia);

void cambiar_estado_instancia(int id_instancia, int esta_activa);

void seleccionar_instancia(char* clave, fila_tabla_instancias *  instancia);

#endif
