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

Message* empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente){
	if (texto == NULL || length < 1)
		return NULL;
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = TEXTO;
	msg->header->remitente = remitente;
	msg->header->size = length + 1;
	msg->contenido = malloc(length + 1);
	memcpy(msg->contenido, texto, length);
	((char*) msg->contenido)[length] = '\0';
	return msg;
}

Message* empaquetar_ack(tipoRemitente remitente){
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = ACK;
	msg->header->size = 0;
	msg->header->remitente = remitente;
	return msg;
}

Message* empaquetar_resultado(tipoRemitente remitente, int resultado){
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->tipo_mensaje = RESULTADO;
	msg->header->size = sizeof(int);
	msg->header->remitente = remitente;
	msg->contenido = calloc(1, sizeof(int));
	memcpy(msg->contenido, &resultado, sizeof(int));
	return msg;
}

int desempaquetar_resultado(Message* msg){
	if (msg == NULL || msg->header == NULL || msg->header->size < 1|| msg->contenido == NULL)
		return 0;
	int* resultado = calloc (1, sizeof(int));
	memcpy (resultado, (int*) msg->contenido,  sizeof(int));
	return *resultado;
}


tipoRemitente desempaquetar_ack(Message* m){
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	return msg->header->remitente;
}

t_operacion * crear_operacion(char* clave, int len_clave, char* valor, int len_valor, enum tipoOperacion tipo){
	t_operacion * op = (t_operacion *) malloc(sizeof(t_operacion));
	op->tipo = tipo;

	op->largo_clave=len_clave+1;

	if (len_clave >0 ){
	op->clave = calloc(1, len_clave+1);
	memcpy(op->clave, clave, len_clave);
	}

	op->largo_valor=len_valor+1;
	if (len_valor >0 ){
	op->valor= calloc(1, len_valor+1);
	memcpy(op->valor, valor, len_valor);
	}
	return op;

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
	if (op->largo_clave>1)
	memcpy(msg->contenido + sizeof(OperacionHeader), op->clave, op->largo_clave);

	if (op->largo_valor>1)
	memcpy(msg->contenido + sizeof(OperacionHeader) + op->largo_clave, op->valor, op->largo_valor);

	//Liberamos lo que ya no nos sirve
	free(opHeader);

	return msg;

}

char* desempaquetar_texto(Message* msg) {
	if (msg == NULL || msg->header == NULL
			|| msg->header->size < 1|| msg->contenido == NULL)
		return NULL;
	char* texto = calloc(1, msg->header->size+1);
	memcpy(texto, msg->contenido, msg->header->size);
	return texto;
}

char* desempaquetar_conexion(Message* msg) {
	if (msg == NULL || msg->header == NULL
			|| msg->header->size < 1|| msg->contenido == NULL)
		return NULL;
	char* texto = calloc(1, msg->header->size+1);
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

	return operacion;
}

Message* empaquetar_conexion(tipoRemitente remitente, char* idRemitente) {
	Message *msg = empaquetar_texto(idRemitente, strlen(idRemitente), remitente);
	msg->header->tipo_mensaje = CONEXION;
	return msg;
}

Message* empaquetar_config_storage(tipoRemitente remitente, int cantEntradas, int tamEntrada){
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	ConfigStorage *config = malloc(sizeof(ConfigStorage));

	msg->header->remitente = remitente;
	msg->header->size = sizeof(ConfigStorage);
	msg->header->tipo_mensaje = CONFSTORAGE;

	config->cantEntradas = cantEntradas;
	config->tamEntrada = tamEntrada;
	msg->contenido = config;

	return msg;
}

ConfigStorage* desempaquetar_config_storage(Message *msg){
	ConfigStorage *cs = malloc(sizeof(ConfigStorage));
	memcpy(cs,msg->contenido, sizeof(ConfigStorage));
	return cs;
}
