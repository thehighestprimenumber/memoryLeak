#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "../socket/operacion.h"

int socket_planificador;

int enviar_mensaje(int socket, Message msg);
void* recibir_conexion(void* con);
int iniciar_servicio();

int socket_fd;

#endif
