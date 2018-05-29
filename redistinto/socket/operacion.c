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
	msg->header->size = sizeof(OperacionHeader) + op->opHeader->largo_clave + op->opHeader->largo_valor;

	//Preparo el contenido del mensaje
	msg->contenido = malloc(msg->header->size);
	memcpy(msg->contenido, op->opHeader, sizeof(OperacionHeader));
	memcpy(msg->contenido + sizeof(OperacionHeader), op->clave, op->opHeader->largo_clave);
	memcpy(msg->contenido + sizeof(OperacionHeader) + op->opHeader->largo_clave, op->valor, op->opHeader->largo_valor);

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

	//Copiamos el header de la operacion desde el contenido del mensaje
	operacion->opHeader = malloc(sizeof(OperacionHeader));
	memcpy(operacion->opHeader, msg->contenido, sizeof(OperacionHeader));

	//Reservamos el espacio necesario para clave y valor
	operacion->clave = malloc(operacion->opHeader->largo_clave);
	operacion->valor = malloc(operacion->opHeader->largo_valor);

	//Copiamos la clave y el valor del contenido del mensaje
	memcpy(operacion->clave, msg->contenido + sizeof(OperacionHeader), operacion->opHeader->largo_clave);
	memcpy(operacion->valor, msg->contenido + sizeof(OperacionHeader) + operacion->opHeader->largo_clave, operacion->opHeader->largo_valor);

	return operacion;
}

Message* empaquetar_conexion(tipoRemitente remitente, char* idRemitente) {
	Message *msg = empaquetar_texto(idRemitente, strlen(idRemitente), remitente);
	msg->header->tipo_mensaje = CONEXION;
	return msg;
}
