#ifndef OPERACION_H
#define OPERACION_H

#include <stdlib.h>
#include <commons/string.h>
#include <string.h>
#include "socket.h"

enum tipo_operacion {
	GET, SET, STORE
};

static char *tipo_operacion_nombres[] = { "GET", "SET", "STORE" };

typedef struct {
	enum tipo_operacion tipo;
	int long_clave;
	char* clave;
	int long_valor;
	char* valor;
} __attribute__((packed)) t_operacion;



char* empaquetar_operacion(t_operacion*);
t_operacion* desempaquetar_operacion(char* , t_operacion* );
Message* empaquetar_texto(char* texto, unsigned int length, tipoRemitente remitente);
char* desempaquetar_texto(Message* msg);

#endif
