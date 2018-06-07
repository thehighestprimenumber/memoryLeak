
#ifndef REDISTINTO_SOCKET_H_
#define REDISTINTO_SOCKET_H_

//Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "common.h"

//Defines
#define IP "127.0.0.1"//todas estaran dentro de la maquina
#define PUERTO_COORDINADOR "8086"
#define PUERTO_PLANIFICADOR "8002"

#define ERROR_DE_CONEXION -10
#define ERROR_DE_ENVIO -11
#define ERROR_DE_RECEPCION -12
#define CLAVE_DUPLICADA -13
#define CLAVE_INEXISTENTE -14
#define CLAVE_MUY_GRANDE -15
#define ERROR_ARCHIVO_NO_ENCONTRADO

#define OK 0

//Enums

//Aca se iran agregando los id que identificaran a los mensajes con distinto objetivo
typedef enum tipoRemitente {DESCONOCIDO, ESI, PLANIFICADOR, INSTANCIA, COORDINADOR} tipoRemitente;

typedef enum tipoMensaje {ACK, CONEXION, DESCONEXION, OPERACION, VALIDAR_BLOQUEO, TEST, TEXTO, RESULTADO, EJECUTAR} tipoMensaje;

//Estructura del header
typedef struct {
	tipoRemitente remitente;
	tipoMensaje tipo_mensaje;
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
	tipoRemitente conectado;
	__SOCKADDR_ARG addr;
} Conexion;


//Funciones
int connect_to_server(char * ip, char * serverPort);
int send_msg(int socket, Message msg);
int await_msg(int socket, Message *msg);
int create_listener(char * ip, char * serverPort);
void start_listening_threads(int socket, void*(*manejadorDeNuevaConexion)(void*));//(void*) -> recibira un (Conexion*)

//el manejador de eventos de start_listening_select debe devolver -1 si desea cerrar ese socket
void start_listening_select(int socketListener, int socketCoordinador, int (*manejadorDeEvento)(int, Message*));
void free_msg(Message **msg);

void close_conection(void *conexion);

//Funciones socket instancia y esi
//int start_listening(int socket, t_list *conexiones);
//int send_msg(int socket, Message* msg);
void close_listener(Conexion *conexion);
void delete_conection(t_list *conexiones,int index);
Conexion* get_conection(t_list *conexiones,int index);

#endif
