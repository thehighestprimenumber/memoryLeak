#ifndef LOGGER_COORDINADOR_H_
#define LOGGER_COORDINADOR_H_

#include "coordinador.h"
#include "../socket/operacion.h"
#include <commons/log.h>
#include <commons/string.h>
#include "../socket/logger_comun.h"

char* nombres_algoritmos_coordinador[3];

typedef struct {
	int socket;
	char* nombre;
}t_socket_nombre;

char* buscar_id_conexion (int socket);
void registrar_conexion(Message * m, int socket);
void loguear_inst_op(t_log* log, char* nombre, t_operacion* op);

t_log* log_coordinador;


void configurar_logger();
#endif /* LOGGER_COORDINADOR_H_ */
