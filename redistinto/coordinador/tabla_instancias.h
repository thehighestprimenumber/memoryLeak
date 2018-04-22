#ifndef TABLA_INSTANCIAS_H_
#define TABLA_INSTANCIAS_H_

#include <commons/collections/list.h>
#include "coordinador.h"


typedef struct {
	int id;
} mock_instancia;

typedef struct {
	mock_instancia * instancia;
	t_list* claves;
} fila_tabla_instancias;

int ultima_instancia_usada;

enum resultado_coord eliminar_instancia(int id_instancia);

enum resultado_coord crear_instancia();

mock_instancia * seleccionar_instancia(char* clave);

#endif
