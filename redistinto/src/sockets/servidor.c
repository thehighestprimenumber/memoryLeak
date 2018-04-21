#include "socket.h"

int create_listener(char * ip, char * serverPort){
	struct addrinfo *server_info;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));//Preparamos el espacio para hints

	hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP


	getaddrinfo(ip, serverPort, &hints, &server_info);  // Carga en server_info los datos de la conexion

	//Si devuelve -1 tuvimos un problema
	int server_socket = socket(server_info->ai_family, server_info->ai_socktype,server_info->ai_protocol);

	if(server_socket == -1){
		freeaddrinfo(server_info);
		return -1;
	}

	if( bind(server_socket, server_info->ai_addr, server_info->ai_addrlen) == -1){
			freeaddrinfo(server_info);
			return -1;
	}

	freeaddrinfo(server_info);

	if(listen(socket, 5) == -1) return -1;

	return socket;
}


int start_listening(int socket, t_list *conexiones){
	while(1){
		Conexion *conexion = malloc(sizeof(Conexion));
		socklen_t addrSize = sizeof(conexion->addr);
		int nuevoSocket;
		if(accept(nuevoSocket, &(conexion->addr), &addrSize) == -1) return -1;
		list_add(conexiones, conexion);//Esto deberia estar echo con un semaforo
	}
	return 0;

}
