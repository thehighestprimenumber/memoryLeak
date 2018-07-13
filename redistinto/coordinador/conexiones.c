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
			free(conexion);
			pthread_exit(0);
		} else {
			enum tipoMensaje tipo = msg->header->tipo_mensaje;

			loguear_recepcion(log_coordinador, msg, buscar_id_conexion(conexion->socket));

			switch (tipo){
				case CONEXION:
					manejar_conexion(msg, conexion->socket);
					free_msg(&msg);
					pthread_exit(0);
					break;
				case DESCONEXION:
					manejar_desconexion(conexion->socket);
					free_msg(&msg);
					pthread_exit(0);
					break;
				case OPERACION:
					manejar_operacion(msg, conexion->socket);
					free_msg(&msg);
					break;
				case ACK:
				case VALIDAR_BLOQUEO:
				case TEXTO:
				case RESULTADO:
					free_msg(&msg);
					pthread_exit(0);
					break;
				case CONFSTORAGE:
				case EJECUTAR:
					loguear_operacion_no_soportada(log_coordinador, msg, conexion->socket);
					free_msg(&msg);
					//pthread_exit(0);
					break;
				case STATUS_CLAVE:
					manejar_status(msg, conexion->socket);
					free_msg(&msg);
					//pthread_exit(0);
					break;
			}

		}
	}
	free(conexion);
	pthread_exit(0);
	exit(0);
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

