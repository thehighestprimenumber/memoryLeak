#include "operacion.h"

#include <commons/string.h>
#include <string.h>
#define TAMANIO_INT 4 //se podria definir un tipo de dato...
#define TAMANIO_CLAVE 4

char* empaquetar_operacion(t_operacion* operacion){
	int desplazamiento=0;
	char* output = malloc(TAMANIO_INT*2+TAMANIO_CLAVE+operacion->long_valor);
	memcpy(&output, string_itoa(&operacion->tipo), TAMANIO_INT);
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
