# include "coordinador.h"
# include "configuracion.h"

//int procesarSolicitudDeEsi(t_operacion op_a_realizar, int solicitante);
int enviar_solicitud_a_instancia(t_operacion operacion, fila_tabla_instancias* instancia);

int pedirle_al_planif_que_bloquee_clave(t_operacion* operacion);
int informar_resultado_al_planificador(int resultado);
extern int iniciar_servicio();
void inicializar_configuracion();
void manejar_kill(int signal);

int main() {
	inicializar_configuracion();
	if (signal(SIGINT, manejar_kill) == SIG_ERR); log_error(logger_coordinador, "error en registrar manejador de kill");
	iniciar_servicio();//cuando llegue un mensaje se va a correr recibir_mensaje (en conexiones.c)
}

void inicializar_configuracion(){
	logger_coordinador = log_create("./log_de_operaciones.log", "log_operaciones", true, LOG_LEVEL_DEBUG);
	//log_debug(logger_coordinador, "cargando configuracion\n");
	cargar_configuracion();

	//log_debug(logger_coordinador, "\nretardo: %d\ncantidad_entradas: %d\ntamanio_entrada: %d\npuerto_escucha: %s\nalgoritmo: %s\n",
	//		coordinador.retardo, coordinador.cantidad_entradas, coordinador.tamanio_entrada, coordinador.puerto_escucha, algoritmo_coordinador_nombres[coordinador.algoritmo]);

	coordinador.tabla_instancias = list_create();
	ultima_instancia_usada=0;
}

int procesarSolicitudDeEsi(t_operacion op_a_realizar, int socket_solicitante) {
	char* clave = op_a_realizar.clave;
	int resultado;
	log_info(logger_coordinador, "procesando solicitud: %s %s %s de ESI socket# %d", tipo_operacion_nombres[op_a_realizar.tipo], clave,
			op_a_realizar.valor, socket);
	if (op_a_realizar.tipo == GET) {
		return pedirle_al_planif_que_bloquee_clave(clave);
	} else {

		fila_tabla_instancias * instancia = seleccionar_instancia(clave);
		log_debug(logger_coordinador, "usando %s", instancia->nombre_instancia);

		if (instancia == NULL) {
			resultado = NO_HAY_INSTANCIAS;
		}
		resultado = enviar_solicitud_a_instancia(op_a_realizar, instancia);
		free(instancia);

	}
	log_info(logger_coordinador, "resultado: %d", resultado);
	informar_resultado_al_planificador(resultado);
	return resultado;
}

int pedirle_al_planif_que_bloquee_clave(t_operacion* operacion){
	if (enviar_mensaje(socket_planificador, OPERACION, operacion)<0) return ERROR_DE_ENVIO;
		log_debug(logger_coordinador, "se solicita bloqueo de clave %s", (char*) operacion->clave);

	Message respuesta; //=  (Message*) calloc(1, sizeof(Message));
	if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
	char * contenido_respuesta = calloc(1, respuesta.header->size);
		strcpy(contenido_respuesta, (char *) respuesta.contenido);
		log_debug(logger_coordinador, "resultado bloqueo de clave: %s", contenido_respuesta);
	return atoi(contenido_respuesta); ////CLAVE_YA_BLOQUEADA o OK
}

int enviar_solicitud_a_instancia(t_operacion operacion, fila_tabla_instancias* instancia){
	char* pedido = tipo_operacion_nombres[operacion.tipo];
		string_append(&pedido, operacion.clave);
		string_append(&pedido, operacion.valor);
		if (enviar_mensaje( *string_itoa(instancia->socket_instancia), OPERACION, pedido)<0) return ERROR_DE_ENVIO;
		free(pedido);
		log_debug(logger_coordinador, "se solicita %s %s %s", tipo_operacion_nombres[operacion.tipo], operacion.clave, operacion.valor);
	Message respuesta;
		if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
		char * contenido_respuesta = calloc(1, respuesta.header->size);
			strcpy(contenido_respuesta, (char *) respuesta.contenido);
			log_debug(logger_coordinador, "resultado de operacion: %s", contenido_respuesta);
		int r = atoi(contenido_respuesta);
		free(contenido_respuesta);
	return r;
}

int informar_resultado_al_planificador(int resultado){
	char* mje_resultado = string_itoa(resultado);
	if (enviar_mensaje(socket_planificador, RESULTADO, mje_resultado)<0) return ERROR_DE_ENVIO;
		log_debug(logger_coordinador, "se envia resultado %s", mje_resultado);

		Message respuesta;
	if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
	char * contenido_respuesta = calloc(1, respuesta.header->size);
		strcpy(contenido_respuesta, (char *) respuesta.contenido);
		log_debug(logger_coordinador, "se recibe AK: %s", contenido_respuesta);
	int r = atoi(contenido_respuesta);
			free(contenido_respuesta);
		return r;
}

void manejar_kill(int signal){
	if (signal == SIGINT) {
		if (socket_fd > 0) close(socket_fd);
		exit(-1);

	}
	log_info(logger_coordinador, "resultado: %d", resultado);
	informar_resultado_al_planificador(resultado);
	return resultado;
}

int pedirle_al_planif_que_bloquee_clave(char* clave){
	char* pedido = "get ";
		string_append(&pedido, clave);
	if (enviar_mensaje(socket_planificador, pedido)<0) return ERROR_DE_ENVIO;
		log_debug(logger_coordinador, "se solicita bloqueo de clave %s", clave);
	Message respuesta; //=  (Message*) malloc(sizeof(Message));
	if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
	char * contenido_respuesta = malloc(respuesta.header->size);
		strcpy(contenido_respuesta, (char *) respuesta.contenido);
		log_debug(logger_coordinador, "resultado bloqueo de clave: %s", contenido_respuesta);
	return atoi(contenido_respuesta); ////CLAVE_YA_BLOQUEADA o OK
}

int enviar_solicitud_a_instancia(t_operacion operacion, fila_tabla_instancias* instancia){
	char* pedido = tipo_operacion_nombres[operacion.tipo];
		string_append(&pedido, operacion.clave);
		string_append(&pedido, operacion.valor);
		if (enviar_mensaje( *string_itoa(instancia->socket_inst), pedido)<0) return ERROR_DE_ENVIO;
		free(pedido);
		log_debug(logger_coordinador, "se solicita %s %s %s", tipo_operacion_nombres[operacion.tipo], operacion.clave, operacion.valor);
	Message respuesta;
		if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
		char * contenido_respuesta = malloc(respuesta.header->size);
			strcpy(contenido_respuesta, (char *) respuesta.contenido);
			log_debug(logger_coordinador, "resultado de operacion: %s", contenido_respuesta);
		int r = atoi(contenido_respuesta);
		free(contenido_respuesta);
	return r;
}

int informar_resultado_al_planificador(int resultado){
	char* mje_resultado = string_itoa(resultado);
	if (enviar_mensaje(socket_planificador, mje_resultado)<0) return ERROR_DE_ENVIO;
		log_debug(logger_coordinador, "se envia resultado %s", mje_resultado);
	Message respuesta;
	if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
	char * contenido_respuesta = malloc(respuesta.header->size);
		strcpy(contenido_respuesta, (char *) respuesta.contenido);
		log_debug(logger_coordinador, "se recibe AK: %s", contenido_respuesta);
	int r = atoi(contenido_respuesta);
			free(contenido_respuesta);
		return r;
}
