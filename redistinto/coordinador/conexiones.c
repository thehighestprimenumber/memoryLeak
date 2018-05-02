#include "./conexiones.h"
#include "./coordinador.h"


int enviar_mensaje(int socket, char* mensaje){
	Message* msg= (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen(mensaje));
	msg->contenido = mensaje;
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->remitente = COORDINADOR;
	msg->header->size = strlen(msg->contenido);

	sleep(5);
	log_info(logger_coordinador, "se va a enviar mensaje desde el coordinador mensaje a %d: %s", socket, msg->contenido);
	int res = send_msg(socket, (*msg));
		if (res<0) {
			log_info(logger_coordinador, "error al enviar mensaje a %d", socket);
			return ERROR_DE_ENVIO;
		}
	log_info(logger_coordinador, "se envio el mensaje desde el coordinador mensaje a %d: %s", socket, msg->contenido);
	free(msg->contenido);
	free(msg->header);
	free(msg);

	return OK;
}

void* recibir_mensaje(void* con){
	Conexion* conexion = (Conexion*) con;
	Message msg;
	int res = await_msg(conexion->socket, &msg);
	if (res<0) {
				log_info(logger_coordinador, "error al recibir un ensaje de %d", socket);
				return string_itoa(ERROR_DE_RECEPCION);
			}
	enum tipoRemitente recipiente =  msg.header->remitente;
	char * request = malloc(msg.header->size);
			strcpy(request, (char *) msg.contenido);

	log_info(logger_coordinador, "recibi mensaje de %d: %s", recipiente, request);

	if (msg.header->tipo_mensaje==TEST)
		return string_itoa(enviar_mensaje(conexion->socket, "Hola soy el coordinador"));

	if (msg.header->remitente == ESI) {
		t_operacion* operacion = malloc(sizeof(t_operacion));
		desempaquetar_operacion ((char*) msg.contenido, operacion);
		res = (int) procesarSolicitudDeEsi(*operacion, conexion->socket);
		free(operacion);
		free(operacion->valor);
		free(operacion->clave);
	}

	free(msg.contenido);
	free(msg.header);
	return ERROR;
}


int iniciar_servicio(){
	log_info(logger_coordinador, "iniciando");

	int socket_fd = create_listener(IP, PUERTO_COORDINADOR);
		if (socket_fd <0) return ERROR_DE_CONEXION;

	start_listening_threads(socket_fd, *recibir_mensaje);

	return 0;
}


