#ifndef ESI_H_
#define ESI_H_

#define puerto_planificador "puerto_planificador"
#define IP_planificador "IP_planificador"
#define puerto_coordinador "puerto_coordinador"
#define IP_coordinador "IP_coordinador"

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include "sockets/socket.h"
#include <stdlib.h>

t_log * log_esi;
int cliente_planificador;
int identificador;

typedef struct {
	char* coordinador_ip;
	char* coordinador_puerto;
	char* planificador_ip;
	char* planificador_puerto;
} esi_configuracion;

esi_configuracion* leer_configuracion_esi(char *ruta_config);
void conectar_a_planificador(esi_configuracion* pConfig);

#endif
