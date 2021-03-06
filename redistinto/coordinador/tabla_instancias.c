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
		free(element);
	return 0;
}
int criterio_nombre(fila_tabla_instancias* fila, void* nombre_instancia){
	return !strcmp ((fila->nombre_instancia), (char*) nombre_instancia);
}

int criterio_esta_activa(fila_tabla_instancias* fila, void* nombre_instancia){
	return fila->esta_activa;
}

int criterio_socket(fila_tabla_instancias* fila, void * socket){
	int numero_socket = *(int*) socket;
	return (fila->socket_instancia == numero_socket);
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
		free(element);
		return 0;

}

void buscar_instancia_por_valor_criterio (void* valor, int criterio (fila_tabla_instancias*, void*), fila_tabla_instancias** output){
	*output = NULL;
	t_link_element *element = coordinador.tabla_instancias->head;
	fila_tabla_instancias *fila;
		while (element != NULL && element->data != NULL) {
			fila = (fila_tabla_instancias*) (element->data);
			if (criterio(fila, valor)>0) {
				*output=fila;
				return;
			}
			element = element->next;
	}
}

int cambiar_estado_instancia(fila_tabla_instancias* fila, int esta_activa){
	fila->esta_activa=esta_activa;
	return OK;
}

fila_tabla_instancias* seleccionar_instancia_EL(){
	fila_tabla_instancias* candidata;
	int i;
	for (i = 0; i<list_size(coordinador.tabla_instancias); i++){
		candidata = list_get(coordinador.tabla_instancias, ((ultima_instancia_usada++)%list_size(coordinador.tabla_instancias)));
		if (candidata->esta_activa) return candidata;
	}
	return NULL;
}

fila_tabla_instancias * seleccionar_instancia_LSU (){
	t_link_element *element = coordinador.tabla_instancias->head;
	fila_tabla_instancias *mejor_opcion = seleccionar_instancia_EL();
	//buscar_instancia_por_valor_criterio(&activa, criterio_esta_activa, &mejor_opcion);
	if (mejor_opcion==NULL)
		return NULL;
	fila_tabla_instancias *fila_test = (fila_tabla_instancias*) element->data;
		while (element != NULL && element->data != NULL) {
			fila_test = (fila_tabla_instancias*) (element->data);
			if (fila_test->esta_activa && fila_test->entradas_libres > mejor_opcion->entradas_libres) {
				mejor_opcion = fila_test;
			}
			element = element->next;
		}
	return mejor_opcion;
}

fila_tabla_instancias* seleccionar_instancia(char* clave) {
	fila_tabla_instancias* fila;
	buscar_instancia_por_valor_criterio(clave, &criterio_clave, &fila);
	if (fila != NULL)
		return fila;
	else
	switch (coordinador.algoritmo) {
		case EQUITATIVE_LOAD:
			return seleccionar_instancia_EL();
		case LEAST_SPACE_USED:
			return seleccionar_instancia_LSU();
		default: return 0;
	}
}


void esperar_operacion(fila_tabla_instancias* instancia){
	while (instancia->esta_activa) {
		sem_wait(&instancia->lock);
		Message * mensaje;
		empaquetar_op_en_mensaje(coordinador.operacion_global_threads, COORDINADOR, &mensaje);
		if (enviar_y_loguear_mensaje(instancia->socket_instancia, *mensaje)<0)
			coordinador.resultado_global = ERROR_DE_ENVIO;
		free_msg(&mensaje);
			//log_debug(logger_coordinador, "se solicita %s %s %s", tipoMensajeNombre[operacion->header->tipo], operacion->clave, operacion->valor);

		Message respuesta;
			if (await_msg(instancia->socket_instancia, &respuesta)<0){
				coordinador.resultado_global = ERROR_DE_RECEPCION;
				log_error(log_coordinador, "error al recibir respuesta de la instancia");
			}

			else coordinador.resultado_global = desempaquetar_resultado(&respuesta);
			//free_msg(&respuesta);

		sem_post(&coordinador.lock_operaciones);
	}
	pthread_exit(OK);
}

fila_tabla_instancias* registrar_instancia(char* nombre_instancia, int socket_instancia) {
	fila_tabla_instancias* fila;
	buscar_instancia_por_valor_criterio(nombre_instancia, criterio_nombre, &fila);
	if (fila !=NULL)
		cambiar_estado_instancia(fila, 1);
	else {
		free_fila_tabla_instancias(&fila);
		fila = calloc(sizeof(fila_tabla_instancias), 1); //TODO (fila_tabla_instancias*)
			fila->socket_instancia = socket_instancia;
			fila->esta_activa = 1;
			fila->entradas_libres = tabla_instancias.cantidad_entradas;
			strcpy(fila->nombre_instancia, (char*) nombre_instancia);
			fila->claves = list_create();
			sem_init(&(fila->lock), 0, 0);
			list_add(coordinador.tabla_instancias, fila);
	}
	return fila;
}

int desconectar_instancia(int socket){
	fila_tabla_instancias* fila;
	buscar_instancia_por_valor_criterio(&socket, criterio_socket, &fila);
	if (fila!=NULL)
		return cambiar_estado_instancia(fila, 0);
	return OK;
}


void free_fila_tabla_instancias(fila_tabla_instancias** fila){
	if(fila != NULL && (*fila) != NULL){
	//if ((*fila)->lock != NULL) pthread_mutex_destroy(&((*fila)->lock));
	free(*fila);
	}
}
