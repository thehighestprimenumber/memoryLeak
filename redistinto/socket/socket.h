
#ifndef REDISTINTO_SOCKET_H_
#define REDISTINTO_SOCKET_H_

//Includes
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>
#include <pthread.h>

//Defines
#define IP "127.0.0.1"//todas estaran dentro de la maquina
#define PUERTO_COORDINADOR "8001"
#define PUERTO_PLANIFICADOR "8002"


//Enums

//Aca se iran agregando los id que identificaran a los mensajes con distinto objetivo
typedef enum{ACK, IDEJMPLO1, ESI, PLANIFICADOR, INSTANCIA} tipoId;

typedef enum {
	HANDSHAKE = 1
}t_protocolo;


//Estructuras

//Estructura de paquete
typedef struct {
	int id_remitente; // Remitente del paquete
	int protocolo; // Un int que representa lo que quiere hacer el remitente
	long cantBytes;
	void* pBuffer;
} t_paquete;

//Estructura del header
typedef struct {
	tipoId id;
	int size;
} __attribute__((packed)) ContentHeader;

//Estructura del mensaje
typedef struct {
	ContentHeader *header;
	void *contenido;
} Message;

//Estructura de conexion
typedef struct{
	int socket;
	__SOCKADDR_ARG addr;
} Conexion;


//Funciones
int connect_to_server(char * ip, char * serverPort);
int send_msg(int socket, Message msg);
int await_msg(int socket, Message *msg);
int create_listener(char * ip, char * serverPort);
void start_listening_threads(int socket, void*(*manejadorDeNuevaConexion)(void*));//(void*) -> recibira un (Conexion*)
//el manejador de mensajes de start_listening_select debe devolver -1 si desea cerrar ese socket
void start_listening_select(int socketListener, int (*manejadorDeEvento)(Conexion*, Message*), void* (*manejadorDeNuevaConexion)(Conexion*));
void close_conection(void *conexion);

//Funciones socket instancia y esi
//int start_listening(int socket, t_list *conexiones);
//int send_msg(int socket, Message* msg);
void close_listener(Conexion *conexion);
void delete_conection(t_list *conexiones,int index);
Conexion* get_conection(t_list *conexiones,int index);

int pedir_handshake(int* pSocket, int remitente);
void destruir_paquete(t_paquete un_paquete);
t_paquete recibir_paquete(int* pSocket);

#endif
