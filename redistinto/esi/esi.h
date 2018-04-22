#ifndef ESI_H_
#define ESI_H_

#define puerto_planificador "puerto_planificador"
#define IP_planificador "IP_planificador"
#define puerto_coordinador "puerto_coordinador"
#define IP_coordinador "IP_coordinador"

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>

t_log * log_esi;

typedef struct {
	int identificador;
	char* ip_coord;
	int puerto_coord;
	char* ip_planif;
	int puerto_planif;
} proceso_esi;

#endif
