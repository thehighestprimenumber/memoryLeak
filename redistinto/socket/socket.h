
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
#include <stdbool.h>

//Defines
#define OK 0
#define ERROR_DE_CONEXION -10
#define ERROR_DE_ENVIO -11
#define ERROR_DE_RECEPCION -12
#define CLAVE_DUPLICADA -13
#define CLAVE_INEXISTENTE -14
#define CLAVE_MUY_GRANDE -15
#define ERROR_ARCHIVO_NO_ENCONTRADO -16
#define ERROR_VALOR_NULO -17
#define FIN_ARCHIVO -18
#define VALOR_INEXISTENTE -19
#define ERROR_COORDINADOR -20
#define NO_HAY_INSTANCIAS -21
#define CLAVE_BLOQUEADA -22

#define OK 0

//Enums

//Aca se iran agregando los id que identificaran a los mensajes con distinto objetivo
typedef enum tipoRemitente {DESCONOCIDO, ESI, PLANIFICADOR, INSTANCIA, COORDINADOR} tipoRemitente;

typedef enum tipoMensaje {ACK, CONEXION, DESCONEXION, OPERACION, VALIDAR_BLOQUEO, TEXTO, RESULTADO, EJECUTAR, CONFSTORAGE, STATUS_CLAVE} tipoMensaje;

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

int socket_a_destruir;

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
bool close_conection_condition(void *conexion);

//Funciones socket instancia y esi
//int start_listening(int socket, t_list *conexiones);
//int send_msg(int socket, Message* msg);
void close_listener(Conexion *conexion);
void delete_conection(t_list *conexiones,int index);
Conexion* get_conection(t_list *conexiones,int index);

#endif
