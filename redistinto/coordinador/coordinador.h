#ifndef COORDINADOR_H
#define COORDINADOR_H

#define NULL 0

#include <stdbool.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include "../esi/esi.h"
#include "../operacion/operacion.h"
#include "tabla_instancias.h"
#include "./conexiones.h"

void configurar_logger();
void salir(int return_nr);

t_log* logger_coordinador;

enum resultado_coord {
	OK, ERROR, NO_HAY_INSTANCIAS, CLAVE_BLOQUEADA
};

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

enum resultado_coord procesarOperacion(t_operacion op_a_realizar, proceso_esi solicitante);

enum resultado_coord abrirCoordinador();

#endif
