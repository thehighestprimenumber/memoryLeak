#ifndef ESI_H_
#define ESI_H_

#define puerto_planificador "puerto_planificador"
#define IP_planificador "IP_planificador"
#define puerto_coordinador "puerto_coordinador"
#define IP_coordinador "IP_coordinador"

#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include "../socket/socket.h"
#include "../socket/logger_comun.h"
#include <stdlib.h>
#include "../socket/operacion.h"
#include "../parsi/src/parsi/parser.h"
#include  <signal.h>
#include <semaphore.h>

t_log * log_esi;

int socket_planificador;
int cliente_coordinador;

int identificador;

int pidplanificador;
int pidcoordinador;

char* path_script;

bool esi_correr;

typedef struct {
	char* script_contenido;
	int script_largo;
} esi_script;

typedef struct {
	char* coordinador_ip;
	char* coordinador_puerto;
	char* planificador_ip;
	char* planificador_puerto;
} esi_configuracion;

esi_script script;
t_list * operaciones;

char* leer_propiedad_string (t_config *configuracion, char* propiedad);
void conectar_a_planificador(esi_configuracion* pConfig);
void conectar_a_coordinador(esi_configuracion* pConfig);
t_operacion * convertir_operacion(t_esi_operacion operacionOriginal);
void enviar_operacion_a_coordinador(t_operacion* operacion);
void enviar_ruta_script_al_planificador(char* path);
int ejecutar_proxima_operacion();
void manejar_mensajes(Message mensaje);
int armar_estructura_script(char* contenidoScript);

#endif
