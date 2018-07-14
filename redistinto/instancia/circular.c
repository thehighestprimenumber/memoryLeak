#include "circular.h"

void inicializar_circular(){
	entradaActual = 0;
}

int entradas_hasta_el_final(){
	return instancia.cantEntradas-entradaActual;
}

int guardar_circular(t_clave_valor *entrada, char *valor){
	if(entrada->largo_clave < 1) return -1;
	if(entradas_que_ocuparia(entrada) > entradas_hasta_el_final()){
		entradaActual = 0;
	}
	int nroEntrada = entradaActual;
	entradaAEliminar = entradaActual;
	log_debug(log_inst, "Guardado CIRC de %s en la entrada nro %d", valor, nroEntrada);
	entradaActual += entradas_que_ocuparia(entrada);

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
