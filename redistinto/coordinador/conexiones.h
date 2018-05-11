#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "../socket/socket.h"
#include "../socket/operacion.h"
#include <unistd.h> // para el sleep

int socket_planificador;
int enviar_mensaje(int socket, tipoMensaje tipo, char* mensaje);
void* recibir_conexion(void* con);
int iniciar_servicio();

int socket_fd;

extern char* empaquetar_operacion(t_operacion* operacion);
extern t_operacion* desempaquetar_operacion(char* , t_operacion* );

#endif
