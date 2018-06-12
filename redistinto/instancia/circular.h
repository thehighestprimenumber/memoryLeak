#ifndef INSTANCIA_CIRCULAR_H_
#define INSTANCIA_CIRCULAR_H_

#include "instancia.h"


int entradaActual;
int entradaAEliminar;


void inicializar_circular();
int guardar_circular(t_clave_valor *entrada, char *valor);
int espacio_sobrante();
bool eliminable(void *contenido);

#endif /* CIRCULAR_H_ */
