#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "../socket/socket.h"
#include "../socket/operacion.h"
#include <unistd.h> // para el sleep

int socket_planificador;

int enviar_mensaje(int socket, tipoMensaje , void* contenido, tipoRemitente , char* idRemitente,	Message * (*empaquetador)(void*, tipoRemitente, char* id_remitente));
void* recibir_conexion(void* con);
int iniciar_servicio();

int socket_fd;

#endif
