#ifndef ESI_H_
#define ESI_H_

#define puerto_planificador "puerto_planificador"
#define IP_planificador "IP_planificador"
#define puerto_coordinador "puerto_coordinador"
#define IP_coordinador "IP_coordinador"

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include "../socket/socket.h"
#include <stdlib.h>

t_log * log_esi;

int socket_planificador;
int cliente_coordinador;

int identificador;

int pidplanificador;
int pidcoordinador;

pthread_t threadPlanificador;
pthread_t threadCoordinador;

typedef struct {
	char* coordinador_ip;
	char* coordinador_puerto;
	char* planificador_ip;
	char* planificador_puerto;
} esi_configuracion;

char* leer_propiedad_string (t_config *configuracion, char* propiedad);
void conectar_a_planificador(esi_configuracion* pConfig);
void conectar_a_coordinador(esi_configuracion* pConfig);
int interpretar(int argc, char **argv);

#endif
