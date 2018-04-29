#include "./conexiones.h"
#include "./coordinador.h"


#define ERROR_DE_CONEXION -1


 extern int send_msg(int socket, Message msg);
 extern int await_msg(int socket, Message *msg);
 extern int create_listener(char * ip, char * serverPort);
 extern  void start_listening_threads(int socket, void*(*manejadorDeNuevaConexion)(void*));//(void*) -> recibira un (Conexion*)
//el manejador de mensajes de start_listening_select debe devolver -1 si desea cerrar ese socket
 extern void close_conection(void *conexion);

void* recibir_mensaje(void* con){
	Conexion* conexion = (Conexion*) con;
	Message msg;
	await_msg(conexion->socket, &msg);
	enum tipoId recipiente =  msg.header->id;
	char * request = malloc(msg.header->size);
			strcpy(request, (char *) msg.contenido);

	log_info(logger_coordinador, "recibi mensaje de %d: %s", recipiente, request);
	//TODO parsear mensaje y hablar con el planificador y la instancia
	return NULL;
}
int iniciar(){
	log_info(logger_coordinador, "iniciado");

	int socket_fd = create_listener(IP, PUERTO_COORDINADOR);
		if (socket_fd <0) return ERROR_DE_CONEXION;

	start_listening_threads(socket_fd, *recibir_mensaje);

	return 1;
}


