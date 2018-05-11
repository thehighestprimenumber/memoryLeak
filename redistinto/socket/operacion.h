#ifndef OPERACION_H
#define OPERACION_H

#include <stdlib.h>


enum tipo_operacion {
	GET, SET, STORE
};

static char *tipo_operacion_nombres[] = { "GET", "SET", "STORE" };

typedef struct {
	enum tipo_operacion tipo;
	char clave[40];
	int long_valor;
	char* valor;
} __attribute__((packed)) t_operacion;



char* empaquetar_operacion(t_operacion*);
t_operacion* desempaquetar_operacion(char* , t_operacion* );

void free_operacion(t_operacion ** operacion);
#endif
