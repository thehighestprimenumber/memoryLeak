
#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#define puerto_planificador "puerto"
#define algoritmo_planificador "algoritmo"
#define estimacion_planificador "estimacion"
#define IPCoord_planificador "IP_coordinador"
#define puertoCoord_planificador "puerto_coordinador"
#define alfa_planificador "alfa_planificador"
#define claves_bloqueadas "claves_bloqueadas"

#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <commons/txt.h>
#include <commons/config.h>
#include <commons/string.h>
#include "time.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "consola.h"
#include "../socket/socket.h"
#include "../socket/operacion.h"

#define OK 0
#define ERROR -20

char* arch_config;

t_log * log_planificador;

pthread_t threadConsola;
int pidConsola;
int pidCoordinador;

typedef enum {READY,RUNNING,BLOCKED,FINISHED} t_esi_estados;

typedef struct {
        //operacion* operacion, TODO crear struct
	    char* puerto_planif;
	   	char* algoritmo_planif;
	   	int estimacion_inicial;
	   	char* IP_coordinador;
	   	char* puerto_coordinador;
	   	int alfaPlanificacion;
	   	t_list* clavesBloqueadas;
} t_planificador;

typedef struct {
	int pid;
} struct_ready;

int esiRunning = 0;

typedef struct {
	int pid;
	char* clave;
} struct_blocked;

typedef struct {
	int pid;
} struct_finished;

t_list* cola_ready;
t_list* cola_blocked;
t_list* cola_finished;


t_planificador planificador;

typedef struct {
	int enumTipo;
} tipo_mensaje;

extern int iniciar();

int socket_coordinador;
int identificador;

char* configTxt = "./configPlanificador.txt";
char* configTxtDebug = "../configPlanificador.txt";

void inicializar_logger();
void exit_proceso();

void estructura_planificador();

void puerto_planif_read(t_config* configuracion);
void algoritmo_read(t_config* configuracion);
void estimacion_read(t_config* configuracion);
void ip_coordinador_read(t_config* configuracion);
void puerto_coordinador_read(t_config* configuracion);
void alfaPlanificacion_read(t_config* configuracion);
void clavesBloqueadas_read(t_config* configuracion);
void liberar_split(char** array);
void conectar_a_coordinador(t_planificador* pConfig);
int recibir_mensaje(Conexion* con,Message* msj);
int realizar_evento(Conexion* con, Message* msj);
int enviar_mensaje(int socket, char* mensaje, tipo_mensaje tipo);

//Firmas de las funciones para agregar a las listas
void agregar_ready(int idEsi);

struct_ready* seleccionar_esi_ready_fifo();

//Firmas de las funciones que irian más adelante
int planificar_esis(); //Replanifica los esis y devuelve el identificador del próximo a ejecutar
int estimar_rafaga(char* algoritmo); //Estima la duración de la próxima ráfaga usando formula de la media exponencial
void finalizar_esi(int socket_esi);

#endif /* PRUEBA_H_ */
