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

void loguear_desconexion(int socket);
void loguear_conexion(t_socket_nombre * dato_conexion);
void loguear_inst_op(char*, t_operacion *);
void loguear_resultado(int resultado);
void loguear_error_envio(Message * m, int socket);
void loguear_envio_OK(Message * m, int socket);
void loguear_recepcion(Message * m, int socket);

t_log* log_coordinador;


void configurar_logger();
#endif /* LOGGER_COORDINADOR_H_ */
