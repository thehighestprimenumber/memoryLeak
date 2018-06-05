#include "./conexiones.h"
#include "./coordinador.h"
#include "./socket.h"

int enviar_mensaje(int socket, Message msg) {

	int res = send_msg(socket, msg);
	if (res < 0) {
		loguear_error_envio(&msg, socket);
		return ERROR_DE_ENVIO;
	}
	loguear_envio_OK(&msg, socket);
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

		}
		enum tipoMensaje tipo = msg->header->tipo_mensaje;

		loguear_recepcion(msg, conexion->socket);

		switch (tipo){
			case TEST: ;
				Message* m = empaquetar_ack(COORDINADOR);
				enviar_mensaje(conexion->socket, *m);
				break;
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
				//resultadoGlobal = desempaquetar_resultado(msg);
				//loguear_resultado(resultadoGlobal);
				//sem_post(&sem_resultado);
				break;
			case OPERACION:
				manejar_operacion(msg, conexion->socket);
				break;
			case EJECUTAR:
				//TODO ERROR
				break;
		}
		//free_msg(&msg);//FIXME
	}
free(conexion);

return 0;
}

int iniciar_servicio() {

	socket_fd = create_listener(IP, PUERTO_COORDINADOR);
	if (socket_fd < 0) {
		log_info(log_coordinador, "error al obtener socket");
		return ERROR_DE_CONEXION;
	}
	start_listening_threads(socket_fd, *recibir_conexion);

	return 0;
}

