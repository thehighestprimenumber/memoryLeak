#include "operacion.h"

#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdlib.h"

#define TAMANIO_CLAVE 40

void free_operacion(t_operacion ** operacion) {
	if (operacion != NULL && (*operacion) != NULL) {
		if ((*operacion)->valor != NULL)
			free((*operacion)->valor);
		free(*operacion);
	}
}

Message* empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente){
	if (texto == NULL || length < 1)
		return NULL;
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = TEXTO;
	msg->header->size = length + 1;
	msg->contenido = malloc(length + 1);
	memcpy(msg->contenido, texto, length);
	((char*) msg->contenido)[length] = '\0';
	return msg;
}

Message* empaquetar_op_en_mensaje(t_operacion * op, tipoRemitente remitente) {
	if (op == NULL)
		return NULL;
		Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = op->tipo;
	msg->header->remitente = remitente;
	msg->header->sizeClave = op->largo_clave;
	msg->header->sizeValor = op->largo_valor;
	msg->header->size = op->largo_clave + op->largo_valor;
	msg->contenido = calloc(1, msg->header->size);
	char* desp = msg->contenido;
	memcpy(desp, op->clave, op->largo_clave );
		desp += op->largo_clave;
	memcpy(desp, op->valor, op->largo_valor );
		desp += op->largo_valor;
	return msg;
}

char* desempaquetar_texto(Message* msg) {
	if (msg == NULL || msg->header == NULL
			|| msg->header->size < 1|| msg->contenido == NULL)
		return NULL;
	char* texto = malloc(msg->header->size);
	memcpy(texto, msg->contenido, msg->header->size);
	return texto;
}

t_operacion* desempaquetar_operacion(Message* msg) {
	t_operacion* operacion = calloc(1,sizeof(operacion));
	operacion->tipo = msg->header->tipo_mensaje;
	operacion->largo_clave = msg->header->sizeClave;
	operacion->largo_valor = msg->header->sizeValor;
	operacion->clave = (char*) calloc(1, operacion->largo_clave);
	operacion->valor = (char*) calloc(1, operacion->largo_valor);

	memcpy(operacion->clave, msg->contenido, operacion->largo_clave);
	memcpy(operacion->valor, msg->contenido+operacion->largo_clave, operacion->largo_valor);
	return operacion;
}
Message* empaquetar_conexion(tipoRemitente remitente) {
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->remitente = remitente;
	msg->header->tipo_mensaje = CONEXION;
	msg->header->size = 0;
	return msg;
}
