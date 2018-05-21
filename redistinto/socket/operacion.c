#include "operacion.h"

#include <commons/string.h>
#include <string.h>
#define TAMANIO_INT 4 //se podria definir un tipo de dato...
#define TAMANIO_CLAVE 4

char* empaquetar_operacion(t_operacion* operacion){
	int desplazamiento=0;
	char* output = malloc(TAMANIO_INT*2+TAMANIO_CLAVE+operacion->long_valor);
	memcpy(&output, (&operacion->tipo)-0, TAMANIO_INT);
	desplazamiento+=TAMANIO_INT;

	memcpy(&output+desplazamiento, &operacion->clave, TAMANIO_CLAVE);
	desplazamiento+=TAMANIO_CLAVE;

	memcpy(&output+desplazamiento, &operacion->long_valor, TAMANIO_INT);
	desplazamiento+=TAMANIO_INT;
	memcpy(&output+desplazamiento, &operacion->valor, operacion->long_valor);
	desplazamiento+=operacion->long_valor;

	return output;
}

t_operacion* desempaquetar_operacion(char* contenido_mensaje, t_operacion* operacion){

		int desplazamiento=0;
		memcpy(&operacion->tipo, contenido_mensaje, TAMANIO_INT);
			desplazamiento+=TAMANIO_INT;


		strcpy(contenido_mensaje+desplazamiento, operacion->clave);
			desplazamiento+=TAMANIO_CLAVE;


		memcpy(&operacion->long_valor, contenido_mensaje+desplazamiento, TAMANIO_INT);
						desplazamiento+=TAMANIO_INT;
		operacion->valor = malloc(operacion->long_valor);
			strcpy(contenido_mensaje+desplazamiento, operacion->valor);

		return operacion;
}


void free_operacion(t_operacion ** operacion){
	if(operacion != NULL && (*operacion) != NULL){
			if( (*operacion)->valor != NULL) free((*operacion)->valor);
			free(*operacion);
	}
}

Message* empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente){
	if(texto == NULL || length < 1) return NULL;
	Message *msg = malloc(sizeof(Message));
	msg->header = malloc(sizeof(ContentHeader));
	msg->header->remitente = remitente;
	msg->header->tipo_mensaje = TEXTO;
	msg->header->size = length+1;
	msg->contenido = malloc(length+1);
	memcpy(msg->contenido, texto, length);
	( (char*) msg->contenido )[length] = '\0';
	return msg;
}

char* desempaquetar_texto(Message* msg){
	if(msg == NULL || msg->header == NULL || msg->header->tipo_mensaje != TEXTO || msg->header->size < 1 || msg->contenido ==  NULL) return NULL;
	char* texto = malloc(msg->header->size);
	memcpy(texto, msg->contenido, msg->header->size);
	return texto;

}
