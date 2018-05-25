#include "./conexiones.h"
#include "./coordinador.h"
#include "./socket.h"

int enviar_mensaje(int socket, Message msg) {
	log_info(logger_coordinador,
			"se va a enviar mensaje desde el coordinador mensaje a %d: %s", socket, msg.contenido);
	int res = send_msg(socket, msg);
	if (res < 0) {
		log_info(logger_coordinador, "error al enviar mensaje a %d", socket);
		return ERROR_DE_ENVIO;
	}
	log_info(logger_coordinador,
			"se envio el mensaje desde el coordinador mensaje a %d: %s", socket,
			msg.contenido);
	return OK;
}

void* recibir_conexion(void* con) {
	Conexion* conexion = (Conexion*) con;

	while (1) {
		Message * msg = (Message *) calloc(sizeof(Message), 1);
		int res = await_msg(conexion->socket, msg);
		if (res < 0) {
			log_info(logger_coordinador, "RIP socket %d", conexion->socket);
			close(conexion->socket);
			free_msg(&msg);
			return string_itoa(ERROR_DE_RECEPCION);
		}
		enum tipoMensaje tipo = msg->header->tipo_mensaje;

		log_info(logger_coordinador, "recibi mensaje de tipo %d", tipo);

		switch (tipo){
			case TEST: ;
				Message* m = empaquetar_texto("hola soy el coodinador\0", strlen("hola soy el coodinador\0"), COORDINADOR);
				m->header->tipo_mensaje = ACK;
				int result = enviar_mensaje(conexion->socket, *m);
				if (result) log_info(logger_coordinador, "error al enviar ack");
				break;
			case CONEXION:
				manejar_conexion(msg, conexion->socket);
				break;
			case DESCONEXION:
				manejar_desconexion(conexion->socket);
				break;
			case ACK:
			case VALIDAR_BLOQUEO:
			case TEXTO:
			case RESULTADO:
				break;
			case op_GET:
			case op_SET:
			case op_STORE:
				manejar_operacion(msg, conexion->socket);
		}
		free_msg(&msg);
	}
	free(conexion);

	return 0;
}

int iniciar_servicio() {

	socket_fd = create_listener(IP, PUERTO_COORDINADOR);
	if (socket_fd < 0) {
		log_error(logger_coordinador, "error al obtener socket");
		return ERROR_DE_CONEXION;
	}
	start_listening_threads(socket_fd, *recibir_conexion);

	return 0;
}

