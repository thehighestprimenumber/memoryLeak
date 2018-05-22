#ifndef TABLA_INSTANCIAS_H_
#define TABLA_INSTANCIAS_H_

#include <commons/collections/list.h>
#include "coordinador.h"

#define MAX_LEN_INSTANCE_NAME 20

typedef struct {
	int socket_instancia;
	t_list* claves;
	int esta_activa;
	pthread_mutex_t lock;
	char nombre_instancia[MAX_LEN_INSTANCE_NAME];

} fila_tabla_instancias;

void free_fila_tabla_instancias(fila_tabla_instancias** fila);

int ultima_instancia_usada;


int cambiar_estado_instancia(fila_tabla_instancias* fila, int esta_activa);

fila_tabla_instancias* seleccionar_instancia(char* clave);

int registar_instancia(char* nombre_instancia, int socket_instancia);

int desconectar_instancia(char* nombre_instancia);

#endif
