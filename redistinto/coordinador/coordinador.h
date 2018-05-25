#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <stdbool.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "../esi/esi.h"
#include "../socket/operacion.h"
#include "tabla_instancias.h"
#include "./conexiones.h"
#include  <signal.h>

#define OK 0
#define ERROR_COORDINADOR -20
#define NO_HAY_INSTANCIAS -21
#define CLAVE_BLOQUEADA -22

void configurar_logger();
void salir(int return_nr);

t_log* logger_coordinador;

enum algoritmo_coordinador {
	EQUITATIVE_LOAD, LEAST_SPACE_USED, KEY_EXPLICIT
};

static char *algoritmo_coordinador_nombres[] = { "EQUITATIVE_LOAD", "LEAST_SPACE_USED", "KEY_EXPLICIT" };

typedef struct {
	t_list* operaciones;
	t_list* tabla_instancias;
	char* puerto_escucha;
	enum algoritmo_coordinador algoritmo;
	int cantidad_entradas;
	int tamanio_entrada;
	int retardo;
} t_coordinador;
t_coordinador coordinador;

typedef struct {
	int identificador;
} proceso_esi;

char* arch_config;
t_log * log_operaciones;


int manejar_operacion(Message * msg, int socket);
int manejar_desconexion(int socket);
int manejar_conexion(Message * m, int socket);

#endif
