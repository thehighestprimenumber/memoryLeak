/*
 * configuracionPlanificador.h
 *
 *  Created on: 27 jun. 2018
 *      Author: utnso
 */

#ifndef CONFIGURACIONPLANIFICADOR_H_
#define CONFIGURACIONPLANIFICADOR_H_

#define puerto_planificador "puerto"
#define ip_planificador "ip_planificador"
#define algoritmo_planificador "algoritmo"
#define estimacion_planificador "estimacion"
#define IPCoord_planificador "IP_coordinador"
#define puertoCoord_planificador "puerto_coordinador"
#define alfa_planificador "alfa_planificador"
#define claves_bloqueadas "claves_bloqueadas"

#include <commons/config.h>
#include <commons/collections/list.h>
#include <string.h>
#include <stdlib.h>
#include <commons/txt.h>
#include <commons/string.h>

typedef struct {
        char* puerto_planif;
	    char* IP_planificador;
	   	char* algoritmo_planif;
	   	int estimacion_inicial;
	   	char* IP_coordinador;
	   	char* puerto_coordinador;
	   	int alfaPlanificacion;
	   	t_list* clavesBloqueadas;
} t_planificador;

t_planificador planificador;

void estructura_planificador();

void puerto_planif_read(t_config* configuracion);
void algoritmo_read(t_config* configuracion);
void estimacion_read(t_config* configuracion);
void ip_coordinador_read(t_config* configuracion);
void puerto_coordinador_read(t_config* configuracion);
void alfaPlanificacion_read(t_config* configuracion);
void clavesBloqueadas_read(t_config* configuracion);


#endif /* CONFIGURACIONPLANIFICADOR_H_ */
