# include "coordinador.h"
# include "configuracion.h"
# include "conexiones.h"


int procesarSolicitudDeEsi(Message * msg, int socket_solicitante);
int validar_bloqueo_con_planificador(t_operacion* operacion);
// int informar_resultado_al_planificador(int resultado);
void manejar_kill(int signal);
int despertar_hilo_instancia(t_operacion * operacion, fila_tabla_instancias* instancia);
void registrar_coordinador_y_quedar_esperando(int socket);

int main() {
	inicializar_configuracion();
	iniciar_servicio();//cuando llegue un mensaje se va a correr recibir_mensaje (en conexiones.c)
	sem_init(&coordinador.lock_operaciones, 0, 1);
	sem_init(&coordinador.lock_planificador, 0, 1);

	//sem_post(&coordinador.lock_operaciones);
}

int manejar_conexion(Message * m, int socket){
	registrar_conexion(m, socket);

	if (m->header->remitente == INSTANCIA) {
		char* nombre_instancia = desempaquetar_conexion(m);
		fila_tabla_instancias * fila = registrar_instancia(nombre_instancia, socket);
		Message * configuracion_instancia = empaquetar_config_storage(COORDINADOR, tabla_instancias.cantidad_entradas, tabla_instancias.tamanio_entrada);
		if (enviar_y_loguear_mensaje(socket, *configuracion_instancia)<0)
			return ERROR_DE_ENVIO;
		esperar_operacion(fila);
		free(nombre_instancia);
		free_msg(&configuracion_instancia);
	} else if (m->header->remitente == PLANIFICADOR) {
		registrar_coordinador_y_quedar_esperando(socket);
	}
	return 0;
}


void manejar_desconexion(int socket){
	loguear_desconexion(log_coordinador, buscar_id_conexion(socket));
	desconectar_instancia(socket);
	close(socket);
}

int manejar_operacion(Message * msg, int socket){
	int res;
	enum tipoRemitente remitente = msg->header->remitente;
		switch (remitente) {
		case ESI:
			res = procesarSolicitudDeEsi(msg, socket);
			break;
		default:
			loguear_operacion_no_soportada(log_coordinador, msg, socket);
		}
		return res;
}

int procesarSolicitudDeEsi(Message * msg, int socket_solicitante) {
	t_operacion * op = desempaquetar_operacion(msg);

	//enviar_mensaje(socket_solicitante, *empaquetar_ack(COORDINADOR));
	char* clave = op->clave;
	coordinador.operacion_global_threads = op;
	sem_post(&coordinador.lock_planificador); //ESTO DEBE DESPERTAR A registrar_coordinador_y_quedar_esperando
	sem_wait(&coordinador.lock_operaciones);
	int resultado = coordinador.resultado_global;
	fila_tabla_instancias * instancia;
	if (resultado == OK) {
//	log_info(logger_coordinador, "procesando solicitud: %s %s %s de ESI socket# %d"
//			, tipoMensajeNombre[op->tipo], clave, op->valor, socket);
		if (op->tipo == op_GET){
			instancia = seleccionar_instancia(clave);
			if (instancia != NULL)
				list_add(instancia->claves, clave);
		} else {
			instancia = buscar_instancia_por_valor_criterio(clave, &criterio_clave);
		}
		if (instancia == NULL) {
			resultado = NO_HAY_INSTANCIAS;
			loguear_no_hay_instancia(log_coordinador, op);
		} else {
			loguear_inst_op(log_coordinador, instancia->nombre_instancia, op);
			despertar_hilo_instancia(op, instancia);
			resultado = coordinador.resultado_global;
			if (resultado > 0) {
				instancia->entradas_libres = resultado;
				resultado = OK;
			}
		}
	}
	//free(clave);

	Message* rta_a_esi = empaquetar_resultado(COORDINADOR, resultado);
	enviar_y_loguear_mensaje(socket_solicitante, *rta_a_esi);
	loguear_resultado(log_coordinador, resultado);
	free_msg(&rta_a_esi);
	return resultado;
}

int validar_bloqueo_con_planificador(t_operacion* operacion){

	Message * mensaje = empaquetar_op_en_mensaje(operacion, COORDINADOR);

	if (enviar_y_loguear_mensaje(coordinador.socket_planificador, *mensaje)<0)
		return ERROR_DE_ENVIO;
		//log_debug(logger_coordinador, "se solicita bloqueo de clave %s", (char*) operacion->clave);
	free_msg(&mensaje);
	Message * respuesta = malloc(sizeof(Message));
	if (await_msg(coordinador.socket_planificador, respuesta)<0)
		return ERROR_DE_RECEPCION;
	int contenido_respuesta = desempaquetar_resultado(respuesta);
	free_msg(&respuesta);
	//int contenido_respuesta = OK;
	loguear_resultado(log_coordinador, contenido_respuesta);
	coordinador.resultado_global = contenido_respuesta;
	return contenido_respuesta; ////CLAVE_DUPLICADA o OK */
}

void registrar_coordinador_y_quedar_esperando(int socket){
	coordinador.socket_planificador = socket;
	while (	coordinador.socket_planificador != 0) {
		sem_wait(&coordinador.lock_planificador);
		coordinador.resultado_global = validar_bloqueo_con_planificador(coordinador.operacion_global_threads);
		sem_post(&coordinador.lock_operaciones);
	}

}

int despertar_hilo_instancia(t_operacion * operacion, fila_tabla_instancias* instancia){

	coordinador.operacion_global_threads = operacion;
	sem_post(&instancia->lock);
	//ACA SE ACTIVA EL esperar_operacion de tabla_instancias.c
	sem_wait(&coordinador.lock_operaciones);

	return coordinador.resultado_global;
}

/*int informar_resultado_al_planificador(int resultado){
	Message * rta_a_planif = empaquetar_resultado(COORDINADOR, coordinador.resultado_global);
		if (enviar_y_loguear_mensaje(socket_planificador, *rta_a_planif)) {
			log_warning(log_coordinador, "error al enviar resultado al coordinador");
			return ERROR_DE_ENVIO;
		}
		free(rta_a_planif);
		return 0;
}*/


void manejar_kill(int signal){
	if (signal == SIGINT) {
		if (socket_fd > 0) close(socket_fd);
		exit(-1);

	}
}

