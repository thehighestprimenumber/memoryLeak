#include "lru.h"

void guardar_eliminando_lru(t_clave_valor *entrada, char *valor);
void eliminar_lru_entrada();

void inicializar_lru(){
	cirularFinalizo = false;
	inicializar_circular();
}

int guardar_lru(t_clave_valor *entrada, char *valor){
	if(entrada->largo_clave < 1) return -1;

	//Si todavia puedo usar circular
	if(!cirularFinalizo && entrada->largo_clave < espacio_sobrante()){
		entrada->datos = malloc(sizeof(t_info_lru));
		((t_info_lru*) entrada->datos)->instant = get_time_millisec();
		guardar_circular(entrada, valor);
		return 0;
	}
	cirularFinalizo = true;
	guardar_eliminando_lru(entrada, valor);
	return 0;
}


void guardar_eliminando_lru(t_clave_valor *entrada, char *valor){

	while(cantidad_entradas_libres() < tam_min_entrada(entrada->largo_valor)){
		eliminar_lru_entrada();
	}
	compactar();
	int posEntrada = obtener_ultima_entrada_libre();
	memcpy(storage + posEntrada, valor, entrada->largo_valor);
	entrada->nroEntrada = posEntrada;
	entrada->datos = malloc(sizeof(t_info_lru));
	((t_info_lru*) entrada->datos)->instant = get_time_millisec();
	list_add(instancia.tabla_entradas, entrada);
}


void eliminar_lru_entrada(){
	int cantEntradas = list_size(instancia.tabla_entradas);
	int indexEntradaEliminable = 0;
	for(int i = 1; i < cantEntradas; i++){
		t_clave_valor *masAntiguo = list_get(instancia.tabla_entradas, indexEntradaEliminable);
		t_clave_valor *aComparar = list_get(instancia.tabla_entradas, i);
		if(((t_info_lru*) masAntiguo->datos)->instant > ((t_info_lru*) aComparar->datos)->instant )
			indexEntradaEliminable = i;
	}
	t_clave_valor *aEliminar = list_get(instancia.tabla_entradas, indexEntradaEliminable);
	list_remove(instancia.tabla_entradas, indexEntradaEliminable);
	free(aEliminar->datos);
	eliminar_entrada(aEliminar);
}

void registrar_set_lru(char *clave){
	t_clave_valor *entrada = buscar_clave_valor(clave);
	((t_info_lru*) entrada->datos)->instant = get_time_millisec();
}


