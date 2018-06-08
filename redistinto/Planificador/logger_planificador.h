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

void loguear_desconexion(int socket);
void loguear_conexion(int socket);
void loguear_resultado(int resultado);
void loguear_error_envio(Message * m, int socket);
void loguear_envio_OK(Message * m, int socket);
void loguear_recepcion(Message * m, int socket);

t_log * log_planificador;

void configurar_logger();

#endif /* LOGGER_PLANIFICADOR_H_ */
