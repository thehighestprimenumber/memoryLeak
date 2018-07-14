#include "bsu.h"

void eliminar_bsu_entrada();
void guardar_eliminando_bsu(t_clave_valor *entrada, char *valor);


void inicializar_bsu(){
	cirularFinalizo2 = false;
	inicializar_circular();
}

int guardar_bsu(t_clave_valor *entrada, char *valor){
	if(entrada->largo_clave < 1) return -1;

	//Si todavia puedo usar circular
	if(!cirularFinalizo2 && entradas_que_ocuparia(entrada) < cantidad_entradas_libres()){
		guardar_circular(entrada, valor);
		return 0;
	}
	cirularFinalizo2 = true;
	guardar_eliminando_bsu(entrada, valor);
	return 0;
}

void guardar_eliminando_bsu(t_clave_valor *entrada, char *valor){

	ordenar_lista_entradas();
	while(cantidad_entradas_libres() < tam_min_entrada(entrada)){
		eliminar_bsu_entrada();
	}
	compactar();
	int posEntrada = obtener_ultima_entrada_libre();
	memcpy(storage + posEntrada, valor, entrada->largo_valor);
	entrada->nroEntrada = posEntrada;
	list_add(instancia.tabla_entradas, entrada);
}

void eliminar_bsu_entrada(){
	int cantEntradas = list_size(instancia.tabla_entradas);
	int indexEntradaEliminable = 1;
	for(int i = 2; i < cantEntradas; i++){
		t_clave_valor *masAntiguo = list_get(instancia.tabla_entradas, indexEntradaEliminable);
		t_clave_valor *aComparar = list_get(instancia.tabla_entradas, i);
		if( tam_min_entrada(masAntiguo) < tam_min_entrada(aComparar) )
			indexEntradaEliminable = i;
	}
	t_clave_valor *aEliminar = list_get(instancia.tabla_entradas, indexEntradaEliminable);
	list_remove(instancia.tabla_entradas, indexEntradaEliminable);
	eliminar_entrada(aEliminar);
}
