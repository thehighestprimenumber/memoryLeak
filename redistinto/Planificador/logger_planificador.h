/*
 * logger_planificador.h
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#ifndef LOGGER_PLANIFICADOR_H_
#define LOGGER_PLANIFICADOR_H_

#include <commons/log.h>
#include <commons/string.h>
#include "../socket/operacion.h"

void registrar_conexion(int socket);

t_log * log_planificador;

void configurar_logger();

#endif /* LOGGER_PLANIFICADOR_H_ */
