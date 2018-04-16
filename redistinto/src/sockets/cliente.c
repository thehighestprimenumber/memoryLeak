#include "socket.h"

int connect_to_server(char * port) {
	struct addrinfo *server_info;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));//Preparamos el espacio para hints

	hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP


	getaddrinfo(IP, port, &hints, &server_info);  // Carga en server_info los datos de la conexion

	//Si devuelve -1 tuvimos un problema
	int server_socket = socket(server_info->ai_family, server_info->ai_socktype,server_info->ai_protocol);

	// Si devuelve algo distinto de 0 tuvimos un problema
	int retorno = connect(server_socket, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);//Liberamos addrinfo

	//verificamos errores y devolvemos acorde
	if(retorno != 0 || server_socket == -1){
		return -1;
	}else{
		return server_socket;
	}
}

int send_msg(int socket, Message msg) {
	//Verifico la existencia del socket y que el mensaje posea contenido
	if(socket == -1 || msg.contenido == NULL || msg.header == NULL) return -1;

	//Calculo el tamaño total
	int sizeTotal = sizeof(ContentHeader) + msg.header->size;

	//Creo y relleno un buffer
	void * buffer = malloc(sizeTotal);
	memcpy(buffer, msg.header, sizeof(ContentHeader));
	memcpy(buffer + sizeof(ContentHeader), msg.contenido, msg.header->size);

	//Envio y registro el resultado que es el tamaño de lo que devolvio o -1 en caso de error
	int resultado = send(socket, buffer, sizeTotal, 0);

	free(buffer);

	if(resultado < 1) return -1;
	else return 1;
}

int await_msg(int socket, Message *msg) {
	//Verifico la existencia del socket y que el puntero al mensaje no este vacio
	if(socket == -1 || msg == NULL) return -1;

	//Preparo un header para recibir (tamaño fijo)
	ContentHeader *header = malloc(sizeof(ContentHeader));

	//Recibo el header
	int result_recv = recv(socket, header, sizeof(ContentHeader), MSG_WAITALL);

	//En caso de error al recibir devuelvo -1
	if(result_recv == -1) return -1;

	//Preparo el espacio necesario para recibir el contenido (tamaño variable)
	void *contenido = malloc(header->size);

	//Recibo el contenido
	result_recv = recv(socket, contenido, header->size, MSG_WAITALL);

	//En caso de error al recibir devuelvo -1
	if(result_recv == -1) return -1;

	//En caso que todo salgo correcto relleno el msg y devuelvo 1 para informar la operacion satisfactoria
	msg->header = header;
	msg->contenido = contenido;
	return 1;
}
