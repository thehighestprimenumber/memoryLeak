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
	int largo_clave;
	int largo_valor;
	int real;
} __attribute__((packed)) StatusHeader;

typedef struct {
	int cantEntradas;
	int tamEntrada;
} __attribute__((packed)) ConfigStorage;

void free_operacion(t_operacion ** operacion);

void empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente, Message** msg);
void desempaquetar_texto(Message* msg, char** texto);

t_operacion * crear_operacion(char* clave, int len_clave, char* valor, int len_valor, enum tipoOperacion tipo);

void empaquetar_op_en_mensaje(t_operacion * op, tipoRemitente remitente, Message** msg);
void desempaquetar_operacion(Message* msg, t_operacion** operacion);

void empaquetar_conexion (char* texto, unsigned int length, tipoRemitente remitente, Message**  msg);
void desempaquetar_conexion(Message* msg, char** texto);

void empaquetar_resultado(tipoRemitente remitente, int resultado, Message** msg);
int desempaquetar_resultado(Message* msg);

void empaquetar_config_storage(tipoRemitente remitente, int cantEntradas, int tamEntrada, Message** msg);
void desempaquetar_config_storage(Message *msg, ConfigStorage** cs);

void empaquetar_STATUS(char* clave, char* nombre_instancia, int largo_clave, int largo_nombre_instancia, tipoRemitente remitente, unsigned int real, Message** msg);
int desempaquetar_status (Message* msg, char** clave, char** idInstancia);
#endif
