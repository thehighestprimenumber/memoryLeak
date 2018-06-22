#include "./conexiones.h"
#include "./coordinador.h"
#include "../socket/socket.h"

int enviar_y_loguear_mensaje(int socket, Message msg) {
	int res = send_msg(socket, msg);
	if (res < 0) {
		loguear_error_envio(log_coordinador, &msg, buscar_id_conexion(socket));
		return ERROR_DE_ENVIO;
	}
	loguear_envio_OK(log_coordinador, &msg, buscar_id_conexion(socket));
	return OK;
}

void* recibir_conexion(void* con) {
	Conexion* conexion = (Conexion*) con;

	while (1) {
		Message * msg = (Message *) calloc(sizeof(Message), 1);
		int res = await_msg(conexion->socket, msg);
		if (res < 0) {
			manejar_desconexion(conexion->socket);
			free_msg(&msg);
			break;

		}
		enum tipoMensaje tipo = msg->header->tipo_mensaje;

		loguear_recepcion(log_coordinador, msg, buscar_id_conexion(conexion->socket));

		switch (tipo){
			case CONEXION:
				manejar_conexion(msg, conexion->socket);
				break;
			case DESCONEXION:
				manejar_desconexion(conexion->socket);
				break;
			case ACK:
				break;
			case VALIDAR_BLOQUEO:
			case TEXTO:
			case RESULTADO:
				break;
			case OPERACION:
				manejar_operacion(msg, conexion->socket);
				break;
			case CONFSTORAGE:
			case EJECUTAR:
				loguear_operacion_no_soportada(log_coordinador, msg, conexion->socket);
				break;
			case STATUS_CLAVE:
				manejar_status(msg, conexion->socket);
				break;
		}
		free_msg(&msg);
	}
free(conexion);

return 0;
}

int iniciar_servicio() {

	char* ipLocal = get_local_ip();
	log_info(log_coordinador, "IP Local: %s", ipLocal);
	socket_fd = create_listener(ipLocal, coordinador.puerto_escucha);
	free(ipLocal);

	if (socket_fd < 0) {
		log_info(log_coordinador, "error al obtener socket");
		return ERROR_DE_CONEXION;
	}
	start_listening_threads(socket_fd, *recibir_conexion);

	return 0;
}

