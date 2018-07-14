#include "compactador.h"

void mover_entrada(int nuevaPosicion, t_clave_valor *entrada);
bool hay_espacio_libre_entre_entradas(t_clave_valor *entrada1, t_clave_valor *entrada2);

void compactar(){
	if(cantidad_entradas_libres() == 0) return;
	ordenar_lista_entradas();
	int index = 0, cantEntradas = list_size(instancia.tabla_entradas);

	while(index+1 < cantEntradas){
		t_clave_valor *entradaA = list_get(instancia.tabla_entradas, index);
		t_clave_valor *entradaB = list_get(instancia.tabla_entradas, index);
		if(entradaA->nroEntrada < 0) {index++;continue;}
		if(hay_espacio_libre_entre_entradas(entradaA, entradaB)){
			int entradaEscribible = entradaA->nroEntrada + tam_min_entrada(entradaA);
			mover_entrada(entradaEscribible, entradaB);
		}
		index++;
	}
}

int tam_min_entrada(t_clave_valor *entrada){
	if(entrada->nroEntrada<0) return 0;
	double d = (double)entrada->largo_valor/(double)instancia.tamEntrada;
	int i = d;
	return i==d ? i:i+1;
}

int entradas_que_ocuparia(t_clave_valor *entrada){
	double d = (double)entrada->largo_valor/(double)instancia.tamEntrada;
	int i = d;
	return i==d ? i:i+1;
}

void sumardor_parcial_espacio_usado(void *contenido){
	t_clave_valor *entrada = contenido;
	espacioUsado += tam_min_entrada(entrada);
}

int cantidad_entradas_libres(){
	espacioUsado = 0;
	//list_iterate(instancia.tabla_entradas, sumardor_parcial_espacio_usado);
	for(int i = 0; i < list_size(instancia.tabla_entradas); i++){
		t_clave_valor *entrada = list_get(instancia.tabla_entradas, i);
		espacioUsado += tam_min_entrada(entrada);
	}
	return instancia.cantEntradas-espacioUsado;
}

bool comparar_entradas_posicion(void *contenido1, void *contenido2){
	t_clave_valor *entrada1 = contenido1, *entrada2 = contenido2;
	return (entrada1->nroEntrada < entrada2->nroEntrada);
}

void ordenar_lista_entradas(){
	list_sort(instancia.tabla_entradas, comparar_entradas_posicion);
}

bool hay_espacio_libre_entre_entradas(t_clave_valor *entrada1, t_clave_valor *entrada2){
	return entrada1->nroEntrada + tam_min_entrada(entrada1) < entrada2->nroEntrada;
}

void mover_entrada(int nuevaPosicion, t_clave_valor *entrada){
	memcpy(storage + nuevaPosicion, storage + entrada->nroEntrada, entrada->largo_valor);
	entrada->nroEntrada = nuevaPosicion;
}

int obtener_ultima_entrada_libre(){
	int cantEntradas = list_size(instancia.tabla_entradas), entradaMaxima = 0, indexEntradaMaxima = 0;

	for(int i = 0; i < cantEntradas; i++){
		t_clave_valor *aComparar = list_get(instancia.tabla_entradas, i);
		if(aComparar->nroEntrada > entradaMaxima){
			entradaMaxima = aComparar->nroEntrada;
			indexEntradaMaxima = i;
		}
	}
	t_clave_valor *maximo = list_get(instancia.tabla_entradas, indexEntradaMaxima);
	return entradaMaxima + tam_min_entrada(maximo);
}

