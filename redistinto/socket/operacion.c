#include "operacion.h"

#define TAMANIO_INT 4 //se podria definir un tipo de dato...

char* empaquetar_operacion(t_operacion* operacion){
	int desplazamiento=0;
	char* output = malloc(TAMANIO_INT*3+operacion->long_clave+operacion->long_valor);
	//memcpy(&output, string_itoa(&operacion->tipo), TAMANIO_INT);
	desplazamiento+=TAMANIO_INT;

	memcpy(&output+desplazamiento, &operacion->long_clave, TAMANIO_INT);
	desplazamiento+=TAMANIO_INT;
	memcpy(&output+desplazamiento, &operacion->clave, operacion->long_clave);
	desplazamiento+=operacion->long_clave;

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

		memcpy(&operacion->long_clave, contenido_mensaje+desplazamiento, TAMANIO_INT);
			desplazamiento+=TAMANIO_INT;
		operacion->clave = malloc(operacion->long_clave);
			strcpy(contenido_mensaje+desplazamiento, operacion->clave);
			desplazamiento+=operacion->long_clave;

		memcpy(&operacion->long_valor, contenido_mensaje+desplazamiento, TAMANIO_INT);
						desplazamiento+=TAMANIO_INT;
		operacion->valor = malloc(operacion->long_valor);
			strcpy(contenido_mensaje+desplazamiento, operacion->valor);

		return operacion;
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
