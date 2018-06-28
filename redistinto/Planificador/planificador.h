
#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#define NO_USE_FSEEK 0

#include <commons/log.h>
#include "time.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include "consola.h"
#include "configuracionPlanificador.h"
#include "../socket/socket.h"
#include "../socket/operacion.h"
#include "../socket/operacion.h"
#include "logger_planificador.h"
#include "../socket/logger_comun.h"

#define OK 0

#define ERROR -20

char* arch_config;
//pthread_t threadConsola;
int pidConsola;
int pidCoordinador;

bool flag_instruccion;
int nroComando;

typedef enum algorimoPrioridad {FIFO} algorimoPrioridad;
typedef enum {READY,RUNNING,BLOCKED,FINISHED} t_esi_estados;

typedef struct {
	int pid;
} struct_ready;

int esiRunning = 0;
int esi_a_eliminar = 0;

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

typedef struct {
	int enumTipo;
} tipo_mensaje;

extern int iniciar();

int identificador;

char* scriptTxtDebug = "../../scripts/";
char* scriptTxt = "../scripts/";

char* contenidoScript;

algorimoPrioridad algorimoEnUso;

t_operacion* operacionEnMemoria;

FILE* script_a_procesar;

void inicializar_logger();
void exit_proceso();

char* armarPathScript(char* cadenaPath,char* nombreScript);
void leer_script_completo(char* nombreScript);
void liberar_split(char** array);
int conectar_a_coordinador(t_planificador* pConfig);
int manejador_de_eventos(int socket, Message* msj);
int realizar_evento(Conexion* con, Message* msj);
int enviar_y_loguear_mensaje(int socket, Message msg, char* destinatario) ;
int manejar_resultado(int socket,Message* msg);
int envio_ejecutar(int socket);
int envio_desconexion(int socket);

//Firmas de las funciones para agregar a las listas
void agregar_ready(int idEsi);
void agregar_esi_blocked(int idEsi,char* clave);

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
void ejecutar_comando(int nroComando);
void reanudar_ejecucion();
void listar_esis_porClave();
int obtener_status();
void consola_bloquear();
void consola_desbloquear();
void consola_kill();
void buscar_y_correr_comando();

int obtener_id_esi(struct_blocked* elemento);
bool clave_ya_bloqueada_config(char*clave1);
bool clave_ya_bloqueada(struct_blocked* elemento);
bool clave_set_disponible(struct_blocked* elemento);
bool buscar_esi_ready(struct_ready* elemento);
bool buscar_esi_a_bloquear(struct_ready* elemento);
bool buscar_esi_a_desbloquear(struct_blocked* elemento);
bool buscar_esi_kill(struct_blocked* elemento);

#endif /* PRUEBA_H_ */
