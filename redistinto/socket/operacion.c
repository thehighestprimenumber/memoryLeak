#include "operacion.h"

#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdlib.h"


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

	Message *msg = malloc(sizeof(Message));

	//Preparo el ContentHeader
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->remitente = remitente;
	msg->header->tipo_mensaje = OPERACION;
	msg->header->size = sizeof(OperacionHeader) + op->largo_clave + op->largo_valor;

	//Preparo el OperacionHeader
	OperacionHeader *opHeader = malloc(sizeof(OperacionHeader));
	opHeader->largo_clave = op->largo_clave;
	opHeader->largo_valor = op->largo_valor;
	opHeader->tipo = op->tipo;

	//Preparo el contenido del mensaje
	msg->contenido = malloc(msg->header->size);
	memcpy(msg->contenido, opHeader, sizeof(OperacionHeader));
	memcpy(msg->contenido + sizeof(OperacionHeader), op->clave, op->largo_clave);
	memcpy(msg->contenido + sizeof(OperacionHeader) + op->largo_clave, op->valor, op->largo_valor);

	//Liberamos lo que ya no nos sirve
	free(opHeader);

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
	t_operacion* operacion = malloc(sizeof(t_operacion));

	//Copiamos el header de la operacion
	OperacionHeader *opHeader = malloc(sizeof(OperacionHeader));
	memcpy(opHeader, msg->contenido, sizeof(OperacionHeader));

	//Lo copiamos a la operacion y lo liberamos
	operacion->largo_clave = opHeader->largo_clave;
	operacion->clave = malloc(operacion->largo_clave);
	operacion->largo_valor = opHeader->largo_valor;
	operacion->valor = malloc(operacion->largo_valor);
	operacion->tipo = opHeader->tipo;
	free(opHeader);

	//Copiamos la clave y el valor
	memcpy(operacion->clave, msg->contenido + sizeof(OperacionHeader), operacion->largo_clave);
	memcpy(operacion->valor, msg->contenido + sizeof(OperacionHeader) + operacion->largo_clave, operacion->largo_valor);

	return operacion;
}

Message* empaquetar_conexion(tipoRemitente remitente, char* idRemitente) {
	Message *msg = empaquetar_texto(idRemitente, strlen(idRemitente), remitente);
	msg->header->tipo_mensaje = CONEXION;
	return msg;
}
