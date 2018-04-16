
#ifndef REDISTINTO_SOCKET_H_
#define REDISTINTO_SOCKET_H_

//Includes
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//Defines
#define IP "172.0.0.1"//todas estaran dentro de la maquina
#define PUERTO_COORDINADOR "8001"
#define PUERTO_PLANIFICADOR "8002"


//Enums

//Aca se iran agregando los id que identificaran a los mensajes con distinto objetivo
typedef enum{IDEJMPLO1, IDEJMPLO2} tipoId;


//Estructuras

//Estructura del header
typedef struct {
	tipoId id;
	int size;
} __attribute__((packed)) ContentHeader;

//Estructura del Mensaje
typedef struct {
	ContentHeader *header;
	void *contenido;
} Message;

//Funciones
int connect_to_server(char * port);
int send_msg(int socket, Message msg);
int await_msg(int socket, Message *msg);

#endif
