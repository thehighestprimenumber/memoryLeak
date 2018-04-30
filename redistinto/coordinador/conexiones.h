#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "../socket/socket.h"
#include "../socket/operacion.h"
#include <unistd.h> // para el sleep

int socket_planificador;
int enviar_mensaje(int socket, char* mensaje);
void* recibir_mensaje(void* con);
int iniciar_servicio();

extern char* empaquetar_operacion(t_operacion* operacion);
extern t_operacion* desempaquetar_operacion(char* , t_operacion* );

#endif
