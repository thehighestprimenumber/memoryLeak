
#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#define NO_USE_FSEEK 0

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

typedef enum algorimoPrioridad {FIFO} algorimoPrioridad;
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

t_list* cola_esi_blocked;

t_planificador planificador;

typedef struct {
	int enumTipo;
} tipo_mensaje;

extern int iniciar();

int socket_coordinador;
int identificador;

char* configTxt = "./configPlanificador.txt";
char* configTxtDebug = "../configPlanificador.txt";

char* scriptTxtDebug = "../../scripts/";
char* scriptTxt = "../scripts/";

char* contenidoScript;

algorimoPrioridad algorimoEnUso;

t_operacion* operacionEnMemoria;

FILE* script_a_procesar;

void inicializar_logger();
void exit_proceso();

void estructura_planificador();

char* armarPathScript(char* cadenaPath,char* nombreScript);
void leer_script_completo(char* nombreScript);
void puerto_planif_read(t_config* configuracion);
void algoritmo_read(t_config* configuracion);
void estimacion_read(t_config* configuracion);
void ip_coordinador_read(t_config* configuracion);
void puerto_coordinador_read(t_config* configuracion);
void alfaPlanificacion_read(t_config* configuracion);
void clavesBloqueadas_read(t_config* configuracion);
void liberar_split(char** array);
int conectar_a_coordinador(t_planificador* pConfig);
int manejador_de_eventos(int socket, Message* msj);
int realizar_evento(Conexion* con, Message* msj);
int enviar_mensaje(int socket, Message msg);

//Firmas de las funciones para agregar a las listas
void agregar_ready(int idEsi);

struct_ready* seleccionar_esi_ready_fifo();

//Firmas de las funciones que irian más adelante
int planificar_esis(); //Replanifica los esis y devuelve el identificador del próximo a ejecutar
int abrirPlanificador(); //Abre la conexión del planificador y sus conexiones
int manejar_nueva_esi_fifo(int socket); //Añade una esi a la lista de prioridades o cola o lo que diga el algoritmo(mirar var global)
int manejar_mensaje_esi_fifo(int socket, Message *msg); //Se encarga de manejar el dato de la ESI y replanificar al respecto
void manejar_desconexion_esi_fifo(int socket); //Elimina a la esi segun plantea fifo
int estimar_rafaga(char* algoritmo); //Estima la duración de la próxima ráfaga usando formula de la media exponencial
void aceptar_conexion(int socket, char* nombreScript);
int validar_clave(char* clave);
int manejar_operacion(int socket,Message* msg);
void finalizar_esi(int socket_esi);
int validar_operacion_get();
int validar_operacion_set();
int validar_operacion_store();
void ejecutar_nueva_esi();
void desbloquear_esi();
void kill_esi();

bool clave_ya_bloqueada_config(char*clave1);
bool clave_ya_bloqueada(struct_blocked elemento);
bool clave_set_disponible(struct_blocked elemento);

#endif /* PRUEBA_H_ */
