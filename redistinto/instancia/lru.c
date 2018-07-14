#include "lru.h"

void guardar_eliminando_lru(t_clave_valor *entrada, char *valor);
void eliminar_lru_entrada();

void inicializar_lru(){
	cirularFinalizo = false;
	inicializar_circular();
}

int guardar_lru(t_clave_valor *entrada, char *valor){
	if(entrada->largo_valor < 1) return -1;

	int cantEntradasLibres = cantidad_entradas_libres();
	int tamMinEntrada = entradas_que_ocuparia(entrada);

	//Si todavia puedo usar circular
	if(!cirularFinalizo && tamMinEntrada <= cantEntradasLibres){
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
	ordenar_lista_entradas();
	while(cantidad_entradas_libres() < entradas_que_ocuparia(entrada)){
		eliminar_lru_entrada();
	}
	compactar();
	int posEntrada = obtener_ultima_entrada_libre();
	log_debug(log_inst, "Guardado LRU de %s en la entrada nro %d", valor, posEntrada);
	memcpy(storage + posEntrada*instancia.tamEntrada, valor, entrada->largo_valor);
	entrada->nroEntrada = posEntrada;
	entrada->datos = malloc(sizeof(t_info_lru));
	((t_info_lru*) entrada->datos)->instant = get_time_millisec();
	//list_add(instancia.tabla_entradas, entrada);
}


void eliminar_lru_entrada(){
	int cantEntradas = list_size(instancia.tabla_entradas);//Porque hay una que es una mentirita piadosa
	int indexEntradaEliminable = 1;
	t_clave_valor *masAntiguo, *aComparar;
	for(int i = 1; i < cantEntradas; i++){
		masAntiguo = list_get(instancia.tabla_entradas, indexEntradaEliminable);
		aComparar = list_get(instancia.tabla_entradas, i);
		if(((t_info_lru*) masAntiguo->datos)->instant > ((t_info_lru*) aComparar->datos)->instant )
			indexEntradaEliminable = i;
	}
	t_clave_valor *aEliminar = list_get(instancia.tabla_entradas, indexEntradaEliminable);
	char *valor = calloc(aEliminar->largo_valor+1,1);
	memcpy(valor, storage + aEliminar->nroEntrada*instancia.tamEntrada, aEliminar->largo_valor);
	log_debug(log_inst, "Borrado LRU de %s", valor);
	free(valor);
	list_remove(instancia.tabla_entradas, indexEntradaEliminable);
	free(aEliminar->datos);
	eliminar_entrada(aEliminar);
}

void registrar_set_lru(char *clave){
	t_clave_valor *entrada = buscar_clave_valor(clave);
	if(entrada == NULL) return;
	((t_info_lru*) entrada->datos)->instant = get_time_millisec();
}


