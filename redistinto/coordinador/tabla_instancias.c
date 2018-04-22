#include "tabla_instancias.h"
#include "coordinador.h"


mock_instancia * seleccionar_instancia(char* clave){
	fila_tabla_instancias * fila;
	if (coordinador.algoritmo==EQUITATIVE_LOAD){
		fila = list_get(coordinador.tabla_instancias, ultima_instancia_usada);
	}
	mock_instancia * instancia = fila->instancia;
	return instancia;
}

enum resultado_coord crear_instancia(){
	mock_instancia * instancia = malloc(sizeof(instancia));
									instancia->id=1;
	fila_tabla_instancias * fila = malloc(sizeof(fila_tabla_instancias));
									fila->instancia = instancia;
	list_add(coordinador.tabla_instancias, fila);
	return OK;
}


