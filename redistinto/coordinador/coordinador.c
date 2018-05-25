# include "coordinador.h"
# include "configuracion.h"


//int procesarSolicitudDeEsi(t_operacion op_a_realizar, int solicitante);
int enviar_solicitud_a_instancia(t_operacion * operacion, fila_tabla_instancias* instancia);
int procesarSolicitudDeEsi(t_operacion * op_a_realizar, int socket_solicitante);
int pedirle_al_planif_que_bloquee_clave(t_operacion * operacion);
int informar_resultado_al_planificador(int resultado);
extern int iniciar_servicio();
void inicializar_configuracion();
void manejar_kill(int signal);

int main() {
	inicializar_configuracion();
	if (signal(SIGINT, manejar_kill) == SIG_ERR)
		log_error(logger_coordinador, "error en registrar manejador de kill");
	iniciar_servicio();//cuando llegue un mensaje se va a correr recibir_mensaje (en conexiones.c)
}

void inicializar_configuracion(){
	logger_coordinador = log_create("./log_de_operaciones.log", "log_operaciones", true, LOG_LEVEL_DEBUG);
	log_debug(logger_coordinador, "cargando configuracion\n");
	cargar_configuracion();

	log_debug(logger_coordinador, "\nretardo: %d\ncantidad_entradas: %d\ntamanio_entrada: %d\npuerto_escucha: %s\nalgoritmo: %s\n",
			coordinador.retardo, coordinador.cantidad_entradas, coordinador.tamanio_entrada, coordinador.puerto_escucha, algoritmo_coordinador_nombres[coordinador.algoritmo]);

	coordinador.tabla_instancias = list_create();
	ultima_instancia_usada=0;
}

int manejar_conexion(Message * m, int socket){
	t_operacion * operacion = desempaquetar_operacion(m);
	if (m->header->remitente == INSTANCIA) {
		log_info(logger_coordinador, "se va a recibe conexion de nueva instancia, socket %d", socket);
		return registar_instancia(operacion->valor, socket);
	}
	return 0;
}

int manejar_desconexion(int socket){
	return desconectar_instancia(socket);
}

int manejar_operacion(Message * msg, int socket){
t_operacion * operacion = desempaquetar_operacion(msg);
int res;
enum tipoRemitente remitente = msg->header->remitente;
	switch (remitente) {
	case ESI://TODO NOW
		res = procesarSolicitudDeEsi(operacion, socket);
		break;
	case INSTANCIA:
	default:
		break; //TODO
	}

	free_operacion(&operacion);
	return res;
}

int procesarSolicitudDeEsi(t_operacion * op, int socket_solicitante) {
	char* clave = op->clave;
	int resultado;
//	log_info(logger_coordinador, "procesando solicitud: %s %s %s de ESI socket# %d"
//			, tipoMensajeNombre[op->tipo], clave, op->valor, socket);

	if (op->opHeader->tipo == op_GET) {
//		log_debug(logger_coordinador, tipoMensajeNombre[op_GET]);
		return pedirle_al_planif_que_bloquee_clave(op);

	} else {
		fila_tabla_instancias * instancia;
		if (op->opHeader->tipo == op_SET)
			instancia = seleccionar_instancia(clave);
		else
			buscar_instancia_por_valor_criterio(op->valor, &instancia, &criterio_clave);

		if (instancia == NULL) {
					resultado = NO_HAY_INSTANCIAS;
				}

		log_debug(logger_coordinador, "usando %s", instancia->nombre_instancia);

		resultado = enviar_solicitud_a_instancia(op, instancia);
		free(instancia);

	}
	log_info(logger_coordinador, "resultado: %d", resultado);
	informar_resultado_al_planificador(resultado);
	return resultado;
}

int pedirle_al_planif_que_bloquee_clave(t_operacion* operacion){
	Message * mensaje = empaquetar_op_en_mensaje(operacion, COORDINADOR);
	if (enviar_mensaje(socket_planificador, *mensaje)<0) return ERROR_DE_ENVIO;
		log_debug(logger_coordinador, "se solicita bloqueo de clave %s", (char*) operacion->clave);

	Message respuesta;
	if (await_msg(socket_planificador, &respuesta)<0) return ERROR_DE_RECEPCION;
	char * contenido_respuesta = calloc(1, respuesta.header->size);
		strcpy(contenido_respuesta, (char *) respuesta.contenido);
		log_debug(logger_coordinador, "resultado bloqueo de clave: %s", contenido_respuesta);
	return atoi(contenido_respuesta); ////CLAVE_YA_BLOQUEADA o OK
}

int enviar_solicitud_a_instancia(t_operacion * operacion, fila_tabla_instancias* instancia){
	Message * mensaje = empaquetar_op_en_mensaje(operacion, COORDINADOR);
	if (enviar_mensaje(instancia->socket_instancia, *mensaje)<0) return ERROR_DE_ENVIO;
	free(mensaje);
		//log_debug(logger_coordinador, "se solicita %s %s %s", tipoMensajeNombre[operacion->header->tipo], operacion->clave, operacion->valor);
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
	Message * mensaje = empaquetar_texto(mje_resultado, strlen(mje_resultado), COORDINADOR);
	mensaje->header->tipo_mensaje = RESULTADO;
		if (enviar_mensaje(socket_planificador, *mensaje)<0) return ERROR_DE_ENVIO;

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
}


