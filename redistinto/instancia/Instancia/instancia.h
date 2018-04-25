#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#define IP_coordinador "IP_coordinador"
#define puerto_coordinador "puerto_coordinador"
#define algoritmo "algoritmo"
#define punto_montaje "punto_montaje"
#define nombre_instancia "nombre_instancia"
#define dump "dump"

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>

t_log * log_inst;

typedef struct {
	int identificador;
	char* ip_coordinador;
	int puerto_coord;
	char* ip_planif;
	char* nombre_inst;
	int int_dump;
	char* path;
} instancia;


#endif /* INSTANCIA_INSTANCIA_H_ */
