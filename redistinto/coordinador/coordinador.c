# include "coordinador.h"
# include "configuracion.h"


int procesarSolicitudDeEsi(Message * msg, int socket_solicitante);
int validar_bloqueo_con_planificador(t_operacion* operacion);
int informar_resultado_al_planificador(int resultado);
extern int iniciar_servicio();
void inicializar_configuracion();
void manejar_kill(int signal);
int despertar_hilo_instancia(t_operacion * operacion, fila_tabla_instancias* instancia);
void registrar_coordinador_y_quedar_esperando(int socket);

int main() {
	inicializar_configuracion();
	if (signal(SIGINT, manejar_kill) == SIG_ERR)
		log_error(log_coordinador, "error en registrar manejador de kill");
	iniciar_servicio();//cuando llegue un mensaje se va a correr recibir_mensaje (en conexiones.c)
	sem_init(&coordinador.lock_operaciones, 0, 1);
	sem_init(&coordinador.lock_planificador, 0, 1);

	//sem_post(&coordinador.lock_operaciones);
}

void inicializar_configuracion(){
	log_coordinador = log_create("./log_de_operaciones.log", "log_operaciones", true, LOG_LEVEL_DEBUG);
	log_debug(log_coordinador, "cargando configuracion\n");
	cargar_configuracion();

	log_debug(log_coordinador, "\nretardo: %d\ncantidad_entradas: %d\ntamanio_entrada: %d\npuerto_escucha: %s\nalgoritmo: %s\n",
			coordinador.retardo, coordinador.cantidad_entradas, coordinador.tamanio_entrada, coordinador.puerto_escucha, coordinador.algoritmo);

	coordinador.tabla_instancias = list_create();
	ultima_instancia_usada=0;
}

int manejar_conexion(Message * m, int socket){
	if (m->header->remitente == INSTANCIA) {
		char* nombre_instancia = desempaquetar_conexion(m);
		loguear_conexion(socket);
		Message * m_ack = empaquetar_ack(COORDINADOR);
		enviar_mensaje(socket, *m_ack);
		//FIXME free_msg(&m_ack);
		registar_instancia_y_quedar_esperando(nombre_instancia, socket);
	} else if (m->header->remitente == PLANIFICADOR) {
		registrar_coordinador_y_quedar_esperando(socket);
	}
	return 0;
}


void manejar_desconexion(int socket){
	close(socket);
	loguear_desconexion(socket);
	desconectar_instancia(socket);
}

int manejar_operacion(Message * msg, int socket){
int res;
enum tipoRemitente remitente = msg->header->remitente;
	switch (remitente) {
	case ESI:
		res = procesarSolicitudDeEsi(msg, socket);
		break;
	default:
		break; //TODO
	}
	//FIXME free(&msg);

	return res;
}

int procesarSolicitudDeEsi(Message * msg, int socket_solicitante) {
	t_operacion * op = desempaquetar_operacion(msg);

	//enviar_mensaje(socket_solicitante, *empaquetar_ack(COORDINADOR));
	char* clave = op->clave;
	coordinador.operacion_global_threads = op;
	sem_post(&coordinador.lock_planificador);
	sem_wait(&coordinador.lock_operaciones);
	int resultado = coordinador.resultado_global;
	if (resultado == OK) {
//	log_info(logger_coordinador, "procesando solicitud: %s %s %s de ESI socket# %d"
//			, tipoMensajeNombre[op->tipo], clave, op->valor, socket);
		fila_tabla_instancias * instancia;
		if (op->tipo == op_GET){
			instancia = seleccionar_instancia(clave);
			if (instancia != NULL)
				list_add(instancia->claves, clave);
		} else {
			instancia = buscar_instancia_por_valor_criterio(op->clave, &criterio_clave);
		}
		if (instancia == NULL) {
			resultado = NO_HAY_INSTANCIAS;
		}
		loguear_inst_op(instancia->nombre_instancia, op);
		if (op->tipo != op_GET){
			despertar_hilo_instancia(op, instancia);
		}
	}
	Message* rta_a_esi = empaquetar_resultado(COORDINADOR, resultado);
	enviar_mensaje(socket_solicitante, *rta_a_esi);

	/*informar_resultado_al_planificador(coordinador.resultado_global);

		resultado = coordinador.resultado_global;
		//free(instancia);

	*/

	loguear_resultado(resultado);
	free_msg(&msg);
	return resultado;
}

int validar_bloqueo_con_planificador(t_operacion* operacion){
	t_operacion interna = {.tipo = operacion->tipo, .largo_clave = operacion->largo_clave, .largo_valor= operacion->largo_valor};
	interna.clave = operacion->clave;
	interna.valor = operacion->valor;

	if (interna.tipo==op_SET) { //para no mandar el valor de la clave que podria ser bastante largo al pedo
		free (interna.valor);
		interna.valor = calloc(1,1);
		interna.largo_valor = 0;
	}

	Message * mensaje = empaquetar_op_en_mensaje(&interna, COORDINADOR);

	if (enviar_mensaje(coordinador.socket_planificador, *mensaje)<0)
		return ERROR_DE_ENVIO;
		//log_debug(logger_coordinador, "se solicita bloqueo de clave %s", (char*) operacion->clave);

	Message * respuesta;
	if (await_msg(coordinador.socket_planificador, respuesta)<0)
		return ERROR_DE_RECEPCION;
	int contenido_respuesta = desempaquetar_resultado(respuesta);
	loguear_resultado(contenido_respuesta);
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

	sem_wait(&coordinador.lock_operaciones);

	return coordinador.resultado_global;
}

int informar_resultado_al_planificador(int resultado){
	Message * rta_a_planif = empaquetar_resultado(COORDINADOR, coordinador.resultado_global);
		if (enviar_mensaje(socket_planificador, *rta_a_planif)) {
			log_warning(log_coordinador, "error al enviar resultado al coordinador");
			return ERROR_DE_ENVIO;
		}
		free(rta_a_planif);
		return 0;
}


void manejar_kill(int signal){
	if (signal == SIGINT) {
		if (socket_fd > 0) close(socket_fd);
		exit(-1);

	}
}
