
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

//Defines
#define IP "172.0.0.1"//todas estaran dentro de la maquina
#define PUERTO_COORDINADOR "8001"
#define PUERTO_PLANIFICADOR "8002"


//Enums

//Aca se iran agregando los id que identificaran a los mensajes con distinto objetivo
typedef enum{ACK, IDEJMPLO1} tipoId;


//Estructuras

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
	struct sockaddr_storage addr;
} Conexion;


//Funciones
int connect_to_server(char * ip, char * serverPort);
int send_msg(int socket, Message msg);
int await_msg(int socket, Message *msg);
int create_listener(char * ip, char * serverPort);
int start_listening(int socket, t_list *conexiones);
void close_listener(Conexion *conexion);
void delete_conection(t_list *conexiones,int index);
Conexion* get_conection(t_list *conexiones,int index);

#endif
