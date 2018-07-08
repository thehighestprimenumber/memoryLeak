#ifndef LOGGER_COMUN_H
#define LOGGER_COMUN_H

#include "socket.h"
#include "operacion.h"
#include <commons/log.h>
char* nombres_modulos[5];
char* nombres_operacion[3];
void loguear_conexion(t_log* log, Message * m, char* id_del_socket);
void loguear_recepcion(t_log* log, Message * m, char* id_del_socket);
void loguear_desconexion(t_log* log, char* id_del_socket);
void loguear_desconexion_int(t_log* log, int id_socket);
void loguear_resultado(t_log* log, int resultado);
void loguear_error_envio(t_log* log, Message * m, char* id_del_socket);
void loguear_envio_OK(t_log* log, Message * m, char* id_del_socket);
void loguear_operacion_no_soportada(t_log* log, Message * m, int id_socket);

#endif
