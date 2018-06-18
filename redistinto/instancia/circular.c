#include "circular.h"

void inicializar_circular(){
	entradaActual = 0;
}

int espacio_sobrante(){
	return (instancia.cantEntradas-entradaActual)*instancia.tamEntrada;
}

int guardar_circular(t_clave_valor *entrada, char *valor){
	if(entrada->largo_clave < 1) return -1;
	if(entrada->largo_clave > espacio_sobrante()){
		entradaActual = 0;
		return guardar_circular(entrada, valor);
	}
	int nroEntrada = entradaActual;
	entradaAEliminar = entradaActual;
	//Esto hace lo que haria basicamente ceil(1)
	double d = (double)entrada->largo_valor/(double)instancia.tamEntrada;
	int i = d;
	entradaActual += i==d ? i:i+1;

	while(entradaAEliminar<entradaActual){
		list_remove_and_destroy_by_condition(instancia.tabla_entradas,eliminable, eliminar_entrada);
		entradaAEliminar++;
	}
	entrada->nroEntrada = nroEntrada;
	memcpy(storage+nroEntrada*instancia.tamEntrada, valor, entrada->largo_valor);
	if(entradaActual >= instancia.cantEntradas) entradaActual=0;
	return 0;
}

bool eliminable(void *contenido){
	t_clave_valor *entrada = contenido;
	if(entrada->nroEntrada==entradaAEliminar) return true;
	return false;
}
