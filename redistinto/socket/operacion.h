#ifndef OPERACION_H
#define OPERACION_H

#include <stdlib.h>
#include <commons/string.h>
#include <string.h>
#include "socket.h"

typedef enum tipoOperacion {op_GET, op_SET, op_STORE} tipoOperacion;

typedef struct {
	int largo_clave;
	int largo_valor;
	enum tipoOperacion tipo;
	char* clave;
	char* valor;
} __attribute__((packed)) t_operacion;

typedef struct {
	int largo_clave;
	int largo_valor;
	enum tipoOperacion tipo;
} __attribute__((packed)) OperacionHeader;

typedef struct {
	int cantEntradas;
	int tamEntrada;
} __attribute__((packed)) ConfigStorage;

void free_operacion(t_operacion ** operacion);

Message* empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente);
char* desempaquetar_texto(Message* msg);

t_operacion * crear_operacion(char* clave, int len_clave, char* valor, int len_valor, enum tipoOperacion tipo);

Message* empaquetar_op_en_mensaje(t_operacion * op, tipoRemitente remitente);
t_operacion* desempaquetar_operacion(Message* msg);

Message* empaquetar_conexion(tipoRemitente remitente, char* idRemitente);
char* desempaquetar_conexion(Message* msg);

Message* empaquetar_ack(tipoRemitente remitente);
tipoRemitente desempaquetar_ack(Message* m);

int desempaquetar_resultado(Message* msg);
Message* empaquetar_resultado(tipoRemitente remitente, int resultado);

Message* empaquetar_config_storage(tipoRemitente remitente, int cantEntradas, int tamEntrada);
ConfigStorage* desempaquetar_config_storage(Message *msg);

#endif
