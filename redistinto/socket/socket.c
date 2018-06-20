#include "socket.h"

int connect_to_server(char * ip, char * serverPort) {
	//Nos aseguramos que no nos hayan mandado punteros vacios
	if(ip == NULL || serverPort == NULL) return -1;

	struct addrinfo hints;
	struct addrinfo *server_info;

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
	if(retorno < 0 || server_socket == -1){
		return -1;
	}else{
		return server_socket;
	}
}
/*
 * devuelve la cantidad de bits leidos
 */
int send_msg(int socket, Message msg) {
	//Verifico la existencia del socket y que el mensaje posea contenido
	if(socket == -1 ||  msg.contenido == NULL || msg.header == NULL) return -1;

	//Calculo el tamaño total
	int size = sizeof(ContentHeader) + msg.header->size;

	//Creo y relleno un buffer
	void * buffer = malloc(size);
	memcpy(buffer, msg.header, sizeof(ContentHeader));
	memcpy(buffer + sizeof(ContentHeader), msg.contenido, msg.header->size);

	//Envio y registro el resultado que es el tamaño de lo que devolvio o -1 en caso de error
	int resultado = send(socket, buffer, size, 0);

	free(buffer);

	return resultado;
}

int await_msg(int socket, Message *msg) {
	//Verifico la existencia del socket y que el puntero al mensaje no este vacio
	if(socket == -1 || msg == NULL) return -1;

	//Preparo un header para recibir (tamaño fijo)
	ContentHeader *header = malloc(sizeof(ContentHeader));

	//Recibo el header
	int result_recv = recv(socket, header, sizeof(ContentHeader), MSG_WAITALL);

	//En caso de error al recibir devuelvo -1
	if(result_recv == -1){
		free(header);
		return -1;
	}

	//Preparo el espacio necesario para recibir el contenido (tamaño variable)
	void *contenido = malloc(header->size+1);

	//Recibo el contenido
	result_recv = recv(socket, contenido, header->size, MSG_WAITALL);

	//En caso de error al recibir devuelvo -1
	if(result_recv < 1){
		free(header);
		free(contenido);
		return -1;
	}

	//En caso que t odo salgo correcto relleno el msg y devuelvo 1 para informar la operacion satisfactoria
	msg->header = header;
	msg->contenido = contenido;
	return result_recv;
}

int create_listener(char * ip, char * serverPort){
	//Nos aseguramos que no nos hayan mandado punteros vacios
	if(ip == NULL || serverPort == NULL) return -1;

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


void start_listening_threads(int socket, void* (*manejadorDeNuevaConexion)(void *) ){
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

		conexion->socket = nuevoSocket;

		//Pregunto si salio t odo bien
		if(nuevoSocket == -1){
			close_conection(conexion);
			continue;//Continuo esperando una nueva conexion
		}

		//Genero un nuevo hilo
		pthread_t nuevoHilo;

		if(pthread_create(&nuevoHilo, NULL, *manejadorDeNuevaConexion, conexion)){
			//Si ocurre esto es porque tuvo un problema creando el hilo
			close_conection(conexion);
		}

	}
}


void start_listening_select(int socketListener, int socketCoordinador, int (*manejadorDeEvento)(int, Message*)){
	//Por si me mandan un socket con problemas
	if(socketListener == -1 || socketCoordinador == -1) return;

	int socketConsola = 0;

	t_list *conexiones = list_create();
	int activity, fdMax = socketListener > socketCoordinador ? socketListener:socketCoordinador;
	fd_set readfds;

	while(1){
		//Vacio el set e incluyo al socket de escucha y el coordinador
		//FD_ZERO(&master);
		FD_ZERO(&readfds);
		FD_SET(socketConsola, &readfds);
		FD_SET(socketListener, &readfds);
		FD_SET(socketCoordinador, &readfds);

		//readfds = master;

		//Agrego a todas las ESIs
		for(int i = 0; i < list_size(conexiones); i++){
			FD_SET( ((Conexion*) list_get(conexiones, i))->socket , &readfds);
		}

		//Esperamos que ocurra algo con alguna de las conexiones (inclusive con el socket de escucha)
		//activity = select( list_size(conexiones) + 2 , &readfds , NULL , NULL , NULL);
		activity = select( fdMax + 1, &readfds, NULL, NULL, NULL);

		if (activity < 0) {
			//Ocurrio un error #lpm
			continue;
		}

		//Checkeamos si ocurrio algo con el socket de escucha
		if(FD_ISSET(socketListener, &readfds)){
			Conexion *conexion = malloc(sizeof(Conexion));
			conexion->addr = malloc(sizeof(struct sockaddr));

			//Calcula el espacio de la direccion de la nueva conexion
			socklen_t addrSize = sizeof(conexion->addr);

			//Acepto esta nueva conexion
			int nuevoSocket = accept(socketListener, conexion->addr, &addrSize);
			conexion->socket = nuevoSocket;

			//Pregunto si salio toodo bien
			if(nuevoSocket == -1) close_conection(conexion);

			//Añado la conexion a la lista
			list_add(conexiones, conexion);

			//Creamos un nuevo mensaje para avisar de la nueva conexion
			Message *msg = malloc(sizeof(Message));
			msg->header = malloc(sizeof(ContentHeader));
			msg->header->remitente = DESCONOCIDO;
			msg->header->tipo_mensaje = CONEXION;
			msg->header->size = 0;
			msg->contenido = NULL;

			//Llamo a la funcion encargada de manejar las nuevas conexiones
			manejadorDeEvento(conexion->socket, msg);
			free_msg(&msg);

			//Seteo el nuevo fd maximo
			fdMax = nuevoSocket > fdMax ? nuevoSocket:fdMax;

		}

		//Preguntamos si ocurrio un evento en la consola
		if(FD_ISSET(socketConsola, &readfds)){
			//Creamos un nuevo mensaje para avisar de la nueva conexion
			Message *msg = malloc(sizeof(Message));
			msg->header = malloc(sizeof(ContentHeader));
			msg->header->remitente = CONSOLA;
			msg->header->tipo_mensaje = TEXTO;
			msg->header->size = 0;
			msg->contenido = NULL;

			//FD_SET(socketConsola, &readfds);

			//Llamo a la funcion encargada de manejar las nuevas conexiones
			manejadorDeEvento(socketConsola, msg);
			free_msg(&msg);
		}

		//Preguntamos si ocurrio un evento en el coordinador
		if(FD_ISSET(socketCoordinador, &readfds)){
			Message *msg = malloc(sizeof(Message));
			if(await_msg( socketCoordinador, msg) == -1){
				//Si se cayo la conexion por decision de diseño mato al coordinador
				exit(EXIT_FAILURE);
			}else{
				manejadorDeEvento(socketCoordinador, msg);
			}
			free_msg(&msg);
		}

		//Recorremos preguntando por cada conexion si ocurrio algun evento
		for(int i = 0; i < list_size(conexiones); i++){

			if(FD_ISSET( ((Conexion*) list_get(conexiones, i))->socket , &readfds )){//Ocurrio algo con este socket
				//Deberia verificar primero si el evento ocurrido fue una desconexion
				//Recibo el mensaje
				Message *msg = malloc(sizeof(Message));
				msg->header = NULL;
				msg->contenido = NULL;
				if(await_msg( ((Conexion*) list_get(conexiones, i))->socket, msg) == -1){
					msg->header = malloc(sizeof(ContentHeader));
					msg->header->remitente = ((Conexion*) list_get(conexiones, i))->conectado;
					msg->header->tipo_mensaje = DESCONEXION;
					msg->header->size = 0;
					msg->contenido = NULL;
					manejadorDeEvento(((Conexion*) list_get(conexiones, i))->socket, msg);
					//list_destroy_and_destroy_elements(conexiones, close_conection);
					socket_a_destruir = ((Conexion*) list_get(conexiones, i))->socket;
					list_remove_and_destroy_by_condition(conexiones, close_conection_condition, close_conection);
				}else{
					((Conexion*) list_get(conexiones, i))->conectado = msg->header->remitente;
					if( manejadorDeEvento(((Conexion*) list_get(conexiones, i))->socket, msg) == -1){//Llamo a la funcion que se encarga de manejar este nuevo mensaje
						//Si es -1 significa que por alguna razon quiere que cierre la conexion
						//list_destroy_and_destroy_elements(conexiones, close_conection);
						list_remove_and_destroy_by_condition(conexiones, close_conection_condition, close_conection);
					}
				}
				free_msg(&msg);
			}
		}

	}
}

void close_conection(void *conexion){
	if(conexion != NULL){
		if(((Conexion*)conexion)->socket != -1) close(((Conexion*)conexion)->socket);
		if(((Conexion*)conexion)->addr != NULL) free(((Conexion*)conexion)->addr);
		free(conexion);
	}
	return;
}

bool close_conection_condition(void *conexion){
	if(conexion != NULL){
		if ((((Conexion*)conexion)->socket) == socket_a_destruir)
		{
			return true;
		}
	}

	return false;
}

void free_msg(Message **msg){
	if(msg != NULL && (*msg) != NULL){
		if((*msg)->header != NULL){
			if((*msg)->contenido != NULL && (*msg)->header->size > 0) free_memory(&((*msg)->contenido));
			free_memory(&((*msg)->header));
		}
		free_memory(msg);
	}
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
