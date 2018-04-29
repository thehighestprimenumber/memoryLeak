#include "./conexiones.h"
#include "./coordinador.h"

 extern int send_msg(int socket, Message msg);
 extern int await_msg(int socket, Message *msg);
 extern int create_listener(char * ip, char * serverPort);
 extern  void start_listening_threads(int socket, void*(*manejadorDeNuevaConexion)(void*));//(void*) -> recibira un (Conexion*)
//el manejador de mensajes de start_listening_select debe devolver -1 si desea cerrar ese socket
 extern void close_conection(void *conexion);

int enviar_mensaje(int socket, char* mensaje){
	Message* msg= (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen(mensaje));
	msg->contenido = mensaje;
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->id = COORDINADOR;
	msg->header->size = strlen(msg->contenido);

	sleep(5);
	log_info(logger_coordinador, "se va a enviar mensaje desde el coordinador mensaje a %d: %s", socket, msg->contenido);
	int res = send_msg(socket, (*msg));
		if (res<0) {
			log_info(logger_coordinador, "error al enviar mensaje a %d", socket);
			return ERROR_DE_ENVIO;
		}
	log_info(logger_coordinador, "se envio el mensaje desde el coordinador mensaje a %d: %s", socket, msg->contenido);
	return OK;
}

void* recibir_mensaje(void* con){
	Conexion* conexion = (Conexion*) con;
	Message msg;
	int res = await_msg(conexion->socket, &msg);
	if (res<0) {
				log_info(logger_coordinador, "error al recibir un ensaje de %d", socket);
				return ERROR_DE_RECEPCION;
			}
	enum tipoId recipiente =  msg.header->id;
	char * request = malloc(msg.header->size);
			strcpy(request, (char *) msg.contenido);

	log_info(logger_coordinador, "recibi mensaje de %d: %s", recipiente, request);
	//TODO parsear mensaje y hablar con el planificador y la instancia
	return enviar_mensaje(conexion->socket, "Hola soy el coordinador");
}


int iniciar(){
	log_info(logger_coordinador, "iniciando");

	int socket_fd = create_listener(IP, PUERTO_COORDINADOR);
		if (socket_fd <0) return ERROR_DE_CONEXION;

	start_listening_threads(socket_fd, *recibir_mensaje);

	return 0;
}


