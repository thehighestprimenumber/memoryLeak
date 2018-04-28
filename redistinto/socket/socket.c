#include "socket.h"

int connect_to_server(char * ip, char * serverPort) {
	struct addrinfo *server_info;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));//Preparamos el espacio para hints

	hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP


	getaddrinfo(ip, serverPort, &hints, &server_info);  // Carga en server_info los datos de la conexion

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

int create_listener(char * ip, char * serverPort){
	struct addrinfo *server_info;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));//Preparamos el espacio para hints

	hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP


	getaddrinfo(ip, serverPort, &hints, &server_info);  // Carga en server_info los datos de la conexion

	//Si devuelve -1 tuvimos un problema
	int server_socket = socket(server_info->ai_family, server_info->ai_socktype,server_info->ai_protocol);

	//Me aseguro que se haya generado sin problemas
	if(server_socket == -1){
		freeaddrinfo(server_info);
		return -1;
	}

	//Le aviso al SO que voy a usar este puerto
	if( bind(server_socket, server_info->ai_addr, server_info->ai_addrlen) == -1){
		freeaddrinfo(server_info);
		return -1;
	}

	freeaddrinfo(server_info);

	//Lo pongo en modo de escucha
	if(listen(server_socket, 5) == -1) return -1;

	return server_socket;
}


void start_listening(int socket, void* (*manejadorDeConexion)(Conexion*)){
	//Por si me mandan un socket con problemas
	if(socket == -1) return;

	while(1){
		//Reservo espacio para una nueva conxion y para la direccion de la conexion
		Conexion *conexion = malloc(sizeof(Conexion));
		conexion->addr = malloc(sizeof(struct sockaddr));

		//Calcula el espacio de la direccion de la nueva conexion
		socklen_t addrSize = sizeof(conexion->addr);

		//Espero una nueva conxion
		int nuevoSocket = accept(socket, conexion->addr, &addrSize);

		//Pregunto si salio todo bien
		if(nuevoSocket == -1){
			free(conexion->addr);
			free(conexion);
			continue;//Continuo esperando una nueva conexion
		}

		conexion->socket = nuevoSocket;
		//Genero un nuevo proceso

		pthread_t nuevoHilo;

		if(pthread_create(&nuevoHilo, NULL, (void*)&manejadorDeConexion, conexion)){
			//Tuvimos un problema
		}


	}
}

void close_conection(Conexion *conexion){
	close(conexion->socket);
	free(conexion->addr);
	free(conexion);
	return;
}

//////////////////////////////////////////////////////////////////////////////////
//FUNCIONES SOCKET INSTANCIA
//////////////////////////////////////////////////////////////////////////////////
/*int start_listening(int socket, t_list *conexiones){
	while(1){
		Conexion *conexion = malloc(sizeof(Conexion));
		socklen_t addrSize = sizeof(conexion->addr);
		int nuevoSocket;
		if(accept(nuevoSocket, &(conexion->addr), &addrSize) == -1) return -1;
		conexion->socket = nuevoSocket;
		//Deberia pedir permiso al semaforo para acceder a la lista
		list_add(conexiones, conexion);
		//
	}
	return 0;

}*/


/////////////////////////////////////////////////////////////////////////////////////
//Funciones carpeta Sockets de ESI
/////////////////////////////////////////////////////////////////////////////////////
void close_listener(Conexion *conexion){
	close(conexion->socket);
	//freeaddrinfo(conexion->addr);
	free(conexion);
	return;
}

void delete_conection(t_list *conexiones,int index){
	//Deberia pedir permiso al semaforo para acceder a la lista
	list_remove_and_destroy_element(conexiones, index, (void*)close_listener);
	//
}

Conexion* get_conection(t_list *conexiones,int index){
	//Deberia pedir permiso al semaforo para acceder a la lista
	return (Conexion*) list_get(conexiones, index);
	//
}

int enviar_paquete(void *buffer,int tamanoBuffer, int *pSocket,int remitente, int protocolo){
	/* Envia el paquete serializado (encabezado + buffer recibido) */
	int tamanoPaquete;
	t_paquete encabezado;
	int resultado, desplazamiento;
	char* pChar;

	encabezado.id_remitente = remitente;
	encabezado.protocolo = protocolo; // Aca va a ir algo que represente lo que queremos hacer
	encabezado.cantBytes = tamanoBuffer;

	tamanoPaquete = tamanoBuffer + sizeof(encabezado);
	void *paquete = malloc (tamanoPaquete);

	pChar = (char*)paquete;
	desplazamiento = 0;

	memcpy(pChar, &encabezado, sizeof(encabezado));
	desplazamiento += sizeof(encabezado);
	if ((buffer != NULL) && (tamanoBuffer > 0)){ // Si el buffer no es nulo
		memcpy((pChar + desplazamiento),(char*)buffer,tamanoBuffer);
	}

	resultado = send(*pSocket, paquete, tamanoPaquete, MSG_NOSIGNAL);
	if (resultado != tamanoPaquete){
		free (paquete);
		perror("error: send\n");
		return -1;
	}

	free (paquete);
	return resultado;

}

t_paquete recibir_paquete(int* pSocket){

	int id; 				// El remitente del mensaje
	int loQueQuieroHacer; 	// Protocolo a definir
	int tam;
	void * pBuffer;
	t_paquete unPaquete;
	int resultado;

	resultado = recv(*pSocket, &id, sizeof(int), MSG_WAITALL);
	if (resultado <= 0){
		unPaquete.protocolo = resultado;
		printf ("Problema con el Cliente: ");
		close(*pSocket);
		//cerrarSocket(pSocket);
		return unPaquete;
	}

	recv(*pSocket, &loQueQuieroHacer, sizeof(int), MSG_WAITALL);
	recv(*pSocket, &tam, sizeof(int), MSG_WAITALL);
	if (tam >0){
		pBuffer = malloc(tam);
		recv(*pSocket, (void*) pBuffer,tam, MSG_WAITALL);
	}
	else{
		pBuffer = NULL;
	}

	// Armo el paquete
	unPaquete.id_remitente = id;
	unPaquete.protocolo = loQueQuieroHacer;
	unPaquete.cantBytes = tam;
	unPaquete.pBuffer = pBuffer;

	return unPaquete;

}

int pedir_handshake(int* pSocket, int remitente){

	int rv;
	rv = enviar_paquete(NULL,0,pSocket, remitente, HANDSHAKE);
	return rv;

}

void destruir_paquete(t_paquete un_paquete){

	if (un_paquete.pBuffer != NULL){
		free (un_paquete.pBuffer);
	}

}
