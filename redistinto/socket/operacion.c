#include "operacion.h"

#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdlib.h"

#define TAMANIO_CLAVE 40

char* empaquetar_operacion(t_operacion* operacion) {

	operacion->long_valor = strlen(operacion->valor);
	char* output = malloc(
			1 + sizeof(int) + operacion->long_valor + TAMANIO_CLAVE );
	int posicion = output;
	char tipo = (operacion->tipo) + '0';
	output[0] = tipo;
	posicion += 1;

	char* len;
	asprintf(&len, "%i", operacion->long_valor);

	output[1] = len[0];
	posicion += 1;

	memcpy(posicion, operacion->valor, operacion->long_valor);
	posicion += operacion->long_valor;

	memcpy(posicion, &operacion->clave, TAMANIO_CLAVE);
	posicion += TAMANIO_CLAVE;
	return output;
}

t_operacion* desempaquetar_operacion(char* contenido_mensaje, t_operacion* operacion) {

	int desplazamiento = 0;

	operacion->tipo = contenido_mensaje[0] - '0';
	desplazamiento++;

	operacion->long_valor = contenido_mensaje[1] - '0';
	desplazamiento++;

	operacion->valor = (char*) calloc(1, operacion->long_valor);
	memcpy(operacion->valor, contenido_mensaje + desplazamiento, operacion->long_valor);
	desplazamiento += operacion->long_valor;

	strcpy(operacion->clave, contenido_mensaje + desplazamiento);
	desplazamiento += TAMANIO_CLAVE;

	return operacion;
}

void free_operacion(t_operacion ** operacion) {
	if (operacion != NULL && (*operacion) != NULL) {
		if ((*operacion)->valor != NULL)
			free((*operacion)->valor);
		free(*operacion);
	}
}

Message* empaquetar_texto(char* texto, tipoRemitente remitente, char* id_remitente) {
	int length = strlen(texto);
	if (texto == NULL || length < 1)
		return NULL;
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->remitente = remitente;
	strcpy(msg->header->id_remitente, id_remitente);
	msg->header->tipo_mensaje = TEXTO;
	msg->header->size = length + 1;
	msg->contenido = malloc(length + 1);
	memcpy(msg->contenido, texto, length);
	((char*) msg->contenido)[length] = '\0';
	return msg;
}

Message* empaquetar_op_en_mensaje(t_operacion * op, tipoRemitente remitente, char* id_remitente) {
	if (op == NULL)
		return NULL;
	char* paquete = empaquetar_operacion(op);

	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->remitente = remitente;
	strcpy(msg->header->id_remitente, id_remitente);
	msg->header->tipo_mensaje = OPERACION;
	msg->header->size = strlen(paquete)+1;
	msg->contenido = calloc(1, strlen(paquete)+1);
	strcpy(msg->contenido, paquete);
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
