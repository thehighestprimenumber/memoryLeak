#include "tabla_instancias.h"
#include "coordinador.h"

int buscar_instancia_por_nombre (char* nombre_instancia, fila_tabla_instancias *output){
	t_link_element *element = coordinador.tabla_instancias->head;
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
	free(element);
	return 0;
}
int criterio_nombre(fila_tabla_instancias* fila, void* nombre_instancia){
	return !strcmp ((fila->nombre_instancia), (char*) nombre_instancia);
}

int criterio_socket(fila_tabla_instancias* fila, int socket){
	return (fila->socket_instancia == socket);
}

int criterio_clave(fila_tabla_instancias* fila, void* nombre_clave){
	t_link_element *element = fila->claves->head;
	char una_clave[41] = {'\0'};
	int i=0;

		while (element != NULL) {
			strcpy (una_clave, element->data);
			i++;

			if (!strcmp (una_clave, (char*) nombre_clave)) {
				return i;
			}
			element = element->next;
		}
			log_debug(logger_coordinador, "ubicacion de la instancia instancia en lista: %d", i);
		free(element);
		return 0;

}

int buscar_instancia_por_valor_criterio (void* valor, fila_tabla_instancias *output, int criterio (fila_tabla_instancias*, void*)){
	t_link_element *element = coordinador.tabla_instancias->head;
	int i=0;
	fila_tabla_instancias *fila;
		while (element != NULL) {
			fila = (fila_tabla_instancias*) (element->data);
			i++;
			if (criterio(fila, valor)) {
				output = fila;
				return i;
			}
			element = element->next;
		}
		log_debug(logger_coordinador, "ubicacion de la instancia instancia en lista: %d", i);
	free(element);
	return 0;
}


int cambiar_estado_instancia(fila_tabla_instancias* fila, int esta_activa){
	log_debug(logger_coordinador, "se va a cambiar el estado de la instancia %s. esta activa: %i", fila->nombre_instancia, esta_activa);
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
	if (buscar_instancia_por_valor_criterio(nombre_instancia, fila2, criterio_nombre))
		cambiar_estado_instancia(fila2, 1);
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

int desconectar_instancia(int socket){
	log_debug(logger_coordinador, "se va a desconectar una instancia, socket %d", socket);
	fila_tabla_instancias* fila = (fila_tabla_instancias*) calloc(sizeof(fila_tabla_instancias*), 1);
	if (buscar_instancia_por_valor_criterio(socket, fila, criterio_socket))
		return cambiar_estado_instancia(fila, 0);
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
