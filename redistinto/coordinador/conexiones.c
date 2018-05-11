#include "./conexiones.h"
#include "./coordinador.h"

int enviar_mensaje(int socket, tipoMensaje tipo, char* mensaje) {
	Message* msg = (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen(mensaje));
	strncpy(msg->contenido, mensaje, strlen(mensaje));
	//msg->contenido = mensaje;
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->tipo_mensaje = tipo;
	msg->header->remitente = COORDINADOR;
	msg->header->size = strlen(msg->contenido) + 1;

	sleep(5);
	log_info(logger_coordinador,
			"se va a enviar mensaje desde el coordinador mensaje a %d: %s",
			socket, msg->contenido);
	int res = send_msg(socket, (*msg));
	if (res < 0) {
		log_info(logger_coordinador, "error al enviar mensaje a %d", socket);
		return ERROR_DE_ENVIO;
	}
	log_info(logger_coordinador,
			"se envio el mensaje desde el coordinador mensaje a %d: %s", socket,
			msg->contenido);
	free_msg(&msg);

	return OK;
}

void* recibir_conexion(void* con) {
	Conexion* conexion = (Conexion*) con;
	char* nombre_instancia = calloc(MAX_LEN_INSTANCE_NAME, 1);
	while (1){
		Message * msg = (Message *) calloc(sizeof(Message), 1);
		int res = await_msg(conexion->socket, msg);
		if (res < 0) {
			log_info(logger_coordinador, "RIP socket %d", conexion->socket);
			close(conexion->socket);
			free_msg(&msg);
			return string_itoa(ERROR_DE_RECEPCION);
		}
		enum tipoRemitente remitente = msg->header->remitente;

		log_info(logger_coordinador, "recibi mensaje de %d", remitente);

		if (msg->header->tipo_mensaje == TEST) {
			free(msg->contenido);
			free(msg->header);
			return string_itoa(
					enviar_mensaje(conexion->socket, ACK, "Hola soy el coordinador"));
		}
		t_operacion* operacion = (t_operacion*) calloc(sizeof(t_operacion), 1);
		if (msg->header->tipo_mensaje == OPERACION) {
			desempaquetar_operacion((char*) msg->contenido, operacion);
		}
		switch (msg->header->remitente) {
			case ESI:
				res = (int) procesarSolicitudDeEsi(*operacion, conexion->socket);
				break;
			case INSTANCIA:
				switch (msg->header->tipo_mensaje) {
					case CONEXION:
						registar_instancia((char*) msg->contenido, conexion->socket);
						break;
					case DESCONEXION:
						desconectar_instancia((char*) msg->contenido);
						break;
					default:
						break; //TODO
				}
				break;
			default:
				break; //TODO
		}
		free_operacion(&operacion);
		free_msg(&msg);
		free(nombre_instancia);
	}

	free(conexion);

	return 0;
}

int iniciar_servicio() {


	socket_fd = create_listener(IP, PUERTO_COORDINADOR);
	if (socket_fd < 0){
		log_error(logger_coordinador, "error al obtener socket");
		return ERROR_DE_CONEXION;
	}
	start_listening_threads(socket_fd, *recibir_conexion);

	return 0;
}

