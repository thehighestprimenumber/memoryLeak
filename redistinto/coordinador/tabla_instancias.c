#include "tabla_instancias.h"
#include "coordinador.h"


void seleccionar_instancia(char* clave, fila_tabla_instancias *  instancia){
	if (coordinador.algoritmo==EQUITATIVE_LOAD){
		instancia = list_get(coordinador.tabla_instancias, ultima_instancia_usada++);
	}
}

int registar_instancia(int socket_instancia){
	fila_tabla_instancias * fila = malloc(sizeof(fila_tabla_instancias));
		fila->socket_inst=socket_instancia;
		fila->esta_activa=1;
		fila->claves=list_create();
	list_add(coordinador.tabla_instancias, fila);
	return OK;
}

void cambiar_estado_instancia(int id_instancia, int esta_activa){
	t_link_element *element = coordinador.tabla_instancias->head;
	t_link_element *aux = NULL;
	fila_tabla_instancias * fila = NULL;
		while (element != NULL) {
			aux = element->next;
			fila = (fila_tabla_instancias*) aux->data;
			if (fila->socket_inst == id_instancia) {
				fila->esta_activa=esta_activa;
				return;
			}
			element = aux;
		}
}
