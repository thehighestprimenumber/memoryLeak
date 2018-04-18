
#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#define puerto_planificador "puerto"
#define algoritmo_planificador "algoritmo"
#define estimacion_planificador "estimacion"
#define IPCoord_planificador "IP_coordinador"
#define puertoCoord_planificador "puerto_coordinador"
#define claves_bloqueadas "claves_bloqueadas"

#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <commons/txt.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>

char* arch_config;

t_log * log_planificador;

typedef enum {READY,RUNNING,BLOCKED,FINISHED} t_esi_estados;

typedef struct {
       //operacion* operacion, TODO crear struct
	   int puerto_planif;
	   	char* algoritmo_planif;
	   	int estimacion_inicial;
	   	char* IP_coordinador;
	   	int puerto_coordinador;
	   	t_list* esis;
	   	t_list* clavesBloqueadas;
} t_planificador;

typedef struct {
	int pid;
} cola_ready;

typedef struct {
	int pid;
} cola_running;

typedef struct {
	int pid;
} cola_blocked;

typedef struct {
	int pid;
} cola_finished;

t_planificador planificador;

int socket_coordinador;

void inicializar_logger();
void exit_proceso();

void estructura_planificador();

#endif /* PRUEBA_H_ */
