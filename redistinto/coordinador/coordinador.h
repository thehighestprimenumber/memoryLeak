#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <stdbool.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>

#include "logger_coordinador.h"
#include "../esi/esi.h"
#include "../socket/operacion.h"
#include "tabla_instancias.h"
#include "./conexiones.h"
#include  <signal.h>
#include <semaphore.h>

#define OK 0
#define ERROR_COORDINADOR -20
#define NO_HAY_INSTANCIAS -21
#define CLAVE_BLOQUEADA -22


void salir(int return_nr);



enum algoritmo_coordinador {
	EQUITATIVE_LOAD, LEAST_SPACE_USED, KEY_EXPLICIT
};



typedef struct {
	t_list* operaciones;
	t_list* tabla_instancias;
	char* puerto_escucha;
	enum algoritmo_coordinador algoritmo;
	int cantidad_entradas;
	int tamanio_entrada;
	int retardo;
	t_operacion * operacion_global_threads;
	int socket_planificador;
	sem_t lock_operaciones;
	sem_t lock_planificador;
	int resultado_global;
} t_coordinador;
t_coordinador coordinador;

typedef struct {
	int identificador;
} proceso_esi;

char* arch_config;


int manejar_operacion(Message * msg, int socket);
void manejar_desconexion(int socket);
int manejar_conexion(Message * m, int socket);

#endif
