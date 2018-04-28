#include "./conexiones.h"
#include "./coordinador.h"
#include <socket.h>

#define ERROR_DE_CONEXION "-1"


 extern int send_msg(int socket, Message msg);
 extern int await_msg(int socket, Message *msg);
 extern int create_listener(char * ip, char * serverPort);
 extern  void start_listening_threads(int socket, void*(*manejadorDeNuevaConexion)(void*));//(void*) -> recibira un (Conexion*)
//el manejador de mensajes de start_listening_select debe devolver -1 si desea cerrar ese socket
 extern void close_conection(void *conexion);

int recibir_mensaje(Conexion* con){
	printf("tienes un email");
}
int iniciar(){
//	Message *msg;

//	int socket_fd = create_listener(IP, PUERTO_COORDINADOR);
//		if (socket_fd <0) return ERROR_DE_CONEXION;

//	start_listening_threads(socket_fd, recibir_mensaje);
//
//	aways_msg(socket_fd, msg);

	return 1;
}


