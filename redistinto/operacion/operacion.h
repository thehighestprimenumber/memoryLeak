#ifndef OPERACION_H
#define OPERACION_H

enum tipo_operacion {
	GET, SET, STORE
};

static char *tipo_operacion_nombres[] = { "GET", "SET", "STORE" };

typedef struct {
	enum tipo_operacion tipo;
	char* clave;
	char* valor;
} t_operacion;

#endif
