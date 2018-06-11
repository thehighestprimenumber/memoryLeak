#ifndef LOGGER_COORDINADOR_H_
#define LOGGER_COORDINADOR_H_

#include "coordinador.h"
#include "../socket/operacion.h"
#include <commons/log.h>
#include <commons/string.h>


typedef struct {
	int socket;
	char* nombre;
}t_socket_nombre;

char* buscar_id_conexion (int socket);
void registrar_conexion(Message * m, int socket);

t_log* log_coordinador;


void configurar_logger();
#endif /* LOGGER_COORDINADOR_H_ */
