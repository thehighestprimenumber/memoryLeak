#ifndef OPERACION_H
#define OPERACION_H

#include <stdlib.h>
#include <commons/string.h>
#include <string.h>
#include "socket.h"

typedef struct {
	int largo_clave;
	int largo_valor;
	enum tipoMensaje tipo;
	char* clave;
	char* valor;
} __attribute__((packed)) t_operacion;

void free_operacion(t_operacion ** operacion);

Message* empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente);
char* desempaquetar_texto(Message* msg);

Message* empaquetar_op_en_mensaje(t_operacion * op, tipoRemitente remitente);
t_operacion* desempaquetar_operacion(Message* msg);

Message* empaquetar_conexion(tipoRemitente remitente);

#endif
