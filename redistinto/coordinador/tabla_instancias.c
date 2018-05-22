#include "tabla_instancias.h"
#include "coordinador.h"

int buscar_instancia_por_nombre (char* nombre_instancia, fila_tabla_instancias *output){
	t_link_element *element = coordinador.tabla_instancias->head;
	//t_link_element *aux = element;
	int i=0;
	fila_tabla_instancias *fila;
		while (element != NULL) {
			fila = (fila_tabla_instancias*) (element->data);
			i++;
			if (!strcmp ((fila->nombre_instancia), nombre_instancia)) {
				output = fila;
				return i;
			}
			element = element->next;
		}
		log_debug(logger_coordinador, "ubicacion de la instancia instancia en lista: %d", i);
	//free(element);
//	free(aux);
	return 0;
}

int cambiar_estado_instancia(fila_tabla_instancias* fila, int esta_activa){
	log_debug(logger_coordinador, "se va a cambiar el estado de la instancia %s. esta activa: %i", fila->nombre_instancia, esta_activa);
	//fila_tabla_instancias * fila = (fila_tabla_instancias *) calloc(sizeof(fila_tabla_instancias), 1);
	if (!buscar_instancia_por_nombre(fila->nombre_instancia, fila)) return NO_HAY_INSTANCIAS;
	fila->esta_activa=esta_activa;
	return OK;
}

fila_tabla_instancias* seleccionar_instancia(char* clave) {
	switch (coordinador.algoritmo) {
		case EQUITATIVE_LOAD:
			return list_get(coordinador.tabla_instancias, ultima_instancia_usada++);
		default: return 0;
	}
}

int registar_instancia(char* nombre_instancia, int socket_instancia) {
	fila_tabla_instancias* fila2 = (fila_tabla_instancias*) calloc(sizeof(fila_tabla_instancias), 1);
	if (buscar_instancia_por_nombre(nombre_instancia, fila2)) cambiar_estado_instancia(fila2, 1);
	else {
		fila_tabla_instancias* fila = (fila_tabla_instancias*) calloc(sizeof(fila_tabla_instancias), 1);
					fila->socket_instancia = 1;
					fila->esta_activa = 1;
					strcpy(fila->nombre_instancia, (char*) nombre_instancia);
					fila->claves = list_create();
					pthread_mutex_init(&fila->lock, NULL);
					pthread_mutex_lock(&fila->lock);
					list_add(coordinador.tabla_instancias, fila);
					log_debug(logger_coordinador, "se registro una instancia, nombre: %s", fila->nombre_instancia);
	}
	free_fila_tabla_instancias(&fila2);
	return OK;
}

int desconectar_instancia(char* nombre_instancia){
	log_debug(logger_coordinador, "se va a desconectar una instancia, nombre %s", nombre_instancia);
		fila_tabla_instancias* fila = (fila_tabla_instancias*) calloc(sizeof(fila_tabla_instancias*), 1);
		if (buscar_instancia_por_nombre(nombre_instancia, fila)) return cambiar_estado_instancia(fila, 0);
		return ERROR_COORDINADOR;
}


void free_fila_tabla_instancias(fila_tabla_instancias** fila){
	if(fila != NULL && (*fila) != NULL){
		//if ((*fila)->lock != NULL) pthread_mutex_destroy(&((*fila)->lock));
		//if((*fila)->claves != NULL) list_destroy_and_destroy_elements((fila)->claves, free_lista_claves);
		if( (*fila)->claves != NULL) list_destroy((*fila)->claves);
		free(*fila);
	}
}
