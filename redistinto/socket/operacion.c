#include "operacion.h"

#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdlib.h"


void free_operacion(t_operacion ** operacion) {
	if (operacion != NULL && (*operacion) != NULL) {
		if ((*operacion)->valor != NULL){
			free((*operacion)->valor);
			(*operacion)->valor = NULL;
		}
		if ((*operacion)->clave != NULL){
			free((*operacion)->clave);
			(*operacion)->clave = NULL;
		}
		free(*operacion);
		*operacion = NULL;
	}
}

void empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente, Message**  output){
	if (texto == NULL || length < 1)
		return;
	Message*  msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = TEXTO;
	msg->header->remitente = remitente;
	msg->header->size = length + 1;
	msg->contenido = calloc(1, length + 1);
	memcpy(msg->contenido, texto, length);
	*output = msg;
}

void empaquetar_conexion (char* texto, unsigned int length, tipoRemitente remitente, Message**  output){
	if (texto == NULL || length < 1)
		return;
	Message*  msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = CONEXION;
	msg->header->remitente = remitente;
	msg->header->size = length + 1;
	msg->contenido = calloc(1, length + 1);
	memcpy(msg->contenido, texto, length);
	*output = msg;
}

void empaquetar_resultado(tipoRemitente remitente, int resultado, Message** output){
	Message*  msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = RESULTADO;
	msg->header->size = sizeof(int);
	msg->header->remitente = remitente;
	msg->contenido = calloc(1, sizeof(int));
	memcpy(msg->contenido, &resultado, sizeof(int));
	*output = msg;
}

int desempaquetar_resultado(Message* msg){
	int output;
	if (msg == NULL || msg->header == NULL || msg->header->size < 1|| msg->contenido == NULL)
		output=ERROR_DE_RECEPCION;
	output = *((int*) msg->contenido);
	return output;
}


t_operacion * crear_operacion(char* clave, int len_clave, char* valor, int len_valor, enum tipoOperacion tipo){
	t_operacion * op = (t_operacion *) malloc(sizeof(t_operacion));
	op->tipo = tipo;

	op->largo_clave=len_clave;

	if (len_clave >0 ){
	op->clave = calloc(1, len_clave);
	memcpy(op->clave, clave, len_clave);
	}

	op->largo_valor=len_valor;
	if (len_valor >0 ){
	op->valor= calloc(1, len_valor+1);
	memcpy(op->valor, valor, len_valor);
	}
	return op;

}

void empaquetar_op_en_mensaje(t_operacion * op, tipoRemitente remitente, Message** output) {
	Message*  msg = malloc(sizeof(Message));

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
	if (op->largo_clave>1)
	memcpy(msg->contenido + sizeof(OperacionHeader), op->clave, op->largo_clave);

	if (op->largo_valor>1)
	memcpy(msg->contenido + sizeof(OperacionHeader) + op->largo_clave, op->valor, op->largo_valor);

	//Liberamos lo que ya no nos sirve
	free_memory(&opHeader);
	*output = msg;

}

void desempaquetar_texto(Message* msg, char** texto) {
	if (msg == NULL || msg->header == NULL
			|| msg->header->size < 1|| msg->contenido == NULL) {
		texto = calloc(1, strlen("Error al desempaquetar texto\0")+1);
		memcpy(texto, msg->contenido, strlen("Error al desempaquetar texto")+1);
	} else {
		texto = calloc(1, msg->header->size+1);
		memcpy(texto, msg->contenido, msg->header->size);
	}
}

void desempaquetar_conexion(Message* msg, char** texto) {
	if (msg == NULL || msg->header == NULL
			|| msg->header->size < 1|| msg->contenido == NULL)
		return;
	*texto = calloc(1, msg->header->size+1);
	memcpy(*texto, msg->contenido, msg->header->size);
}

void desempaquetar_operacion(Message* msg, t_operacion** output) {
	t_operacion * operacion = malloc(sizeof(t_operacion));

	//Copiamos el header de la operacion
	OperacionHeader *opHeader = malloc(sizeof(OperacionHeader));
	memcpy(opHeader, msg->contenido, sizeof(OperacionHeader));

	//Lo copiamos a la operacion y lo liberamos
	operacion->largo_clave = opHeader->largo_clave;
	operacion->clave = calloc(1, operacion->largo_clave);
	operacion->largo_valor = opHeader->largo_valor;
	operacion->valor = calloc(1, operacion->largo_valor);
	operacion->tipo = opHeader->tipo;
	free(opHeader);

	//Copiamos la clave y el valor
	if (operacion->largo_clave>1)
	memcpy(operacion->clave, msg->contenido + sizeof(OperacionHeader), operacion->largo_clave);

	if (operacion->largo_valor>1)
	memcpy(operacion->valor, msg->contenido + sizeof(OperacionHeader) + operacion->largo_clave, operacion->largo_valor);

	*output = operacion;
}

void empaquetar_config_storage(tipoRemitente remitente, int cantEntradas, int tamEntrada, Message** output){
	Message* msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	ConfigStorage *config = malloc(sizeof(ConfigStorage));

	msg->header->remitente = remitente;
	msg->header->size = sizeof(ConfigStorage);
	msg->header->tipo_mensaje = CONFSTORAGE;

	config->cantEntradas = cantEntradas;
	config->tamEntrada = tamEntrada;
	msg->contenido = config;
	*output = msg;

}

void desempaquetar_config_storage(Message *msg, ConfigStorage** cs){
	*cs = malloc(sizeof(ConfigStorage));
	memcpy(*cs,msg->contenido, sizeof(ConfigStorage));
}

void empaquetar_STATUS(char* clave, char* nombre_instancia, int largo_clave, int largo_nombre_instancia, tipoRemitente remitente, unsigned int real, Message** output) {
	Message* msg = malloc(sizeof(Message));

		//Preparo el ContentHeader
		msg->header = malloc(sizeof(ContentHeader));
		msg->header->remitente = remitente;
		msg->header->tipo_mensaje = STATUS_CLAVE;
		msg->header->size = sizeof(StatusHeader) + largo_clave + largo_nombre_instancia;

		//Preparo el OperacionHeader
		StatusHeader *opHeader = malloc(sizeof(StatusHeader));
		opHeader->largo_clave = largo_clave;
		opHeader->largo_valor = largo_nombre_instancia;
		opHeader->real = real;

		//Preparo el contenido del mensaje
		msg->contenido = malloc(msg->header->size);
		memcpy(msg->contenido, opHeader, sizeof(StatusHeader));
		if (largo_clave>1)
		memcpy(msg->contenido + sizeof(StatusHeader), clave, largo_clave);

		if (largo_nombre_instancia>1)
		memcpy(msg->contenido + sizeof(StatusHeader) + largo_clave, nombre_instancia, largo_nombre_instancia);

		//Liberamos lo que ya no nos sirve
		free(opHeader);
		*output = msg;
}


int desempaquetar_status (Message* msg, char** clave, char** idInstancia) {
	//Copiamos el header de la operacion
	StatusHeader *statusHeader = malloc(sizeof(StatusHeader));
	memcpy(statusHeader, msg->contenido, sizeof(StatusHeader));

	*clave = calloc(1, statusHeader->largo_clave+1);
	idInstancia = calloc(1, statusHeader->largo_valor+1);
	int real = statusHeader->real;
	//Copiamos la clave y el valor
	if (statusHeader->largo_clave>1)
	memcpy(*clave, msg->contenido + sizeof(StatusHeader), statusHeader->largo_clave);
	if (statusHeader->largo_valor>1)
	memcpy(idInstancia, msg->contenido + sizeof(StatusHeader)+ statusHeader->largo_clave, statusHeader->largo_valor);
	free(statusHeader);

	return real;
}
