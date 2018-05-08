
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
#include "time.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "consola.h"
#include "../socket/socket.h"

#define OK 0
#define ERROR -20

char* arch_config;
t_log * log_planificador;
pthread_t threadConsola;
int pidConsola;
int pidCoordinador;

typedef enum algorimoPrioridad {FIFO} algorimoPrioridad;
typedef enum {READY,RUNNING,BLOCKED,FINISHED} t_esi_estados;

typedef struct {
        //operacion* operacion, TODO crear struct
	    char* puerto_planif;
	   	char* algoritmo_planif;
	   	int estimacion_inicial;
	   	char* IP_coordinador;
	   	char* puerto_coordinador;
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

extern int iniciar();

int socket_coordinador;
int identificador;

char* configTxt = "./configPlanificador.txt";
char* configTxtDebug = "../configPlanificador.txt";
algorimoPrioridad algorimoEnUso;

void inicializar_logger();
void exit_proceso();

void estructura_planificador();

void puerto_planif_read(t_config* configuracion);
void algoritmo_read(t_config* configuracion);
void estimacion_read(t_config* configuracion);
void ip_coordinador_read(t_config* configuracion);
void puerto_coordinador_read(t_config* configuracion);
void clavesBloqueadas_read(t_config* configuracion);
void liberar_split(char** array);
void conectar_a_coordinador(t_planificador* pConfig);
int manejador_de_eventos(Conexion* con, Message* msj);
int realizar_evento(Conexion* con, Message* msj);
int enviar_mensaje(int socket, char* mensaje);

//Firmas de las funciones que irian más adelante
int abrirPlanificador(); //Abre la conexión del planificador y sus conexiones
void manejar_nueva_esi(Conexion *con); //Añade una esi a la lista de prioridades o cola o lo que diga el algoritmo(mirar var global)
int manejar_mensaje_esi(Conexion *con, Message *msg); //Se encarga de manejar el dato de la ESI y replanificar al respecto
int estimar_rafaga(char* algoritmo); //Estima la duración de la próxima ráfaga usando formula de la media exponencial


#endif /* PRUEBA_H_ */
