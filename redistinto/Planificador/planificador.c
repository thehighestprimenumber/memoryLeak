#include "planificador.h"

int main(void) {
	inicializar_logger();

	log_info(log_planificador, "\nCargando configuración");
	estructura_planificador();

	log_info(log_planificador,"\nPuerto planificador: %s", planificador.puerto_planif);
	log_info(log_planificador,"\nAlgoritmo planificador: %s", planificador.algoritmo_planif);
	log_info(log_planificador,"\nEstimación: %d", planificador.estimacion_inicial);
	log_info(log_planificador,"\nIP Coordinador: %s", planificador.IP_coordinador);
	log_info(log_planificador,"\nPuerto Coordinador: %s",planificador.puerto_coordinador);

	//conectar a coordinador
	t_planificador* pConfig = (t_planificador*)&planificador;
	conectar_a_coordinador(pConfig);

	log_info(log_planificador,"\nInicio de la consola\n");

	//Abrir Consola
	pidConsola = pthread_create(&threadConsola, NULL, (void*)&abrir_consola, (void*) "Inicio del hilo de la consola");

	if (pidConsola < 0) {
		log_error(log_planificador,"Error al intentar abrir la consola");
		exit_proceso(-1);
	}

	//pthread_join(threadConsola,NULL);

	//Escuchar conexiones ESI
	iniciar();

	log_info(log_planificador,"\nProceso finalizado");
	list_destroy(planificador.clavesBloqueadas);

	exit_proceso(0);
}

void inicializar_logger() {
	log_planificador = log_create("/home/utnso/tp/Planificador.log", "Planificador: ", true, LOG_LEVEL_INFO);
}

int iniciar(){
	log_info(log_planificador, "Iniciando proceso planificador");

	int socket_fd = create_listener(IP,planificador.puerto_planif);
	if (socket_fd <0) return ERROR_DE_CONEXION;

	start_listening_select(socket_fd, *recibir_conexion);
	//start_listening_threads(socket_fd, *recibir_mensaje);

	return 0;
}

int recibir_conexion(Conexion* conexion, Message* msg){
	int res = await_msg(conexion->socket, msg);
	if (res<0) {
		log_info(log_planificador, "error al recibir un ensaje de %d", socket);
		return ERROR_DE_RECEPCION;
		//return string_itoa(ERROR_DE_RECEPCION);
	}

	enum tipoRemitente recipiente = msg->header->remitente;
	char * request = malloc((msg->header->size));
	strcpy(request, (char *) msg->contenido);

	log_info(log_planificador, "recibi mensaje de %d: %s", recipiente, request);

	if (msg->header->tipo_mensaje==TEST)
	{
		return enviar_mensaje(conexion->socket, "Hola soy el planificador");
		//return string_itoa(enviar_mensaje(conexion->socket, "Hola soy el planificador"));
	}

	return ERROR_COORDINADOR;
}

/*void* recibir_mensaje(void* con){
	Conexion* conexion = (Conexion*) con;
	Message msg;
	int res = await_msg(conexion->socket, &msg);
	if (res<0) {
				log_info(log_planificador, "error al recibir un ensaje de %d", socket);
				return string_itoa(ERROR_DE_RECEPCION);
			}
	enum tipoRemitente recipiente =  msg.header->remitente;
	char * request = malloc((msg.header->size));
			strcpy(request, (char *) msg.contenido);

	log_info(log_planificador, "recibi mensaje de %d: %s", recipiente, request);

	if (msg.header->tipo_mensaje==TEST)
	{
		free(msg.contenido);
		free(msg.header);
		return string_itoa(enviar_mensaje(conexion->socket, "Hola soy el planificador"));
	}

	free(msg.contenido);
	free(msg.header);
	return ERROR;
}*/

int enviar_mensaje(int socket, char* mensaje){
	Message* msg= (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen(mensaje));
	strncpy(msg->contenido,mensaje,strlen(mensaje));
	//msg->contenido = mensaje;
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->remitente = PLANIFICADOR;
	msg->header->size = strlen(msg->contenido);

	log_info(log_planificador, "se va a enviar mensaje desde el planificador mensaje a %d: %s", socket, msg->contenido);
	int res = send_msg(socket, (*msg));
		if (res<0) {
			log_info(log_planificador, "error al enviar mensaje a %d", socket);
			return ERROR_DE_ENVIO;
		}
	log_info(log_planificador, "se envio el mensaje desde el planificador mensaje a %d: %s", socket, msg->contenido);
	free_msg(&msg);
	//free(msg->contenido);
	//free(msg->header);
	//free(msg);

	return OK;
}

void estructura_planificador() {
	t_config* configuracion;

	planificador.clavesBloqueadas = list_create();

	configuracion = config_create(configTxt);

	if (configuracion == NULL)
		configuracion = config_create(configTxtDebug);

	puerto_planif_read(configuracion);
	algoritmo_read(configuracion);
	estimacion_read(configuracion);
	ip_coordinador_read(configuracion);
	puerto_coordinador_read(configuracion);
	clavesBloqueadas_read(configuracion);

	config_destroy(configuracion);
}

void puerto_planif_read(t_config* configuracion) {
	if (config_has_property(configuracion, puerto_planificador)) {
		char* puertoPlanif = config_get_string_value(configuracion,puerto_planificador);
		planificador.puerto_planif = malloc(strlen(puertoPlanif) + 1);
		memcpy(planificador.puerto_planif,puertoPlanif,strlen(puertoPlanif) + 1);
	}
}

void algoritmo_read(t_config* configuracion) {
	if (config_has_property(configuracion, algoritmo_planificador)) {
		char* algoritmo = config_get_string_value(configuracion,algoritmo_planificador);
		planificador.algoritmo_planif = malloc(strlen(algoritmo) + 1);
		memcpy(planificador.algoritmo_planif,algoritmo,strlen(algoritmo) + 1);
	}
}

void estimacion_read(t_config* configuracion) {
	if (config_has_property(configuracion, estimacion_planificador)) {
		planificador.estimacion_inicial = config_get_int_value(configuracion,estimacion_planificador);
	}
}

void ip_coordinador_read(t_config* configuracion) {
	if (config_has_property(configuracion, IPCoord_planificador)) {
		char* IPcoord = config_get_string_value(configuracion,IPCoord_planificador);

		planificador.IP_coordinador = malloc(strlen(IPcoord) + 1);
		memcpy(planificador.IP_coordinador,IPcoord,strlen(IPcoord) + 1);

	}
}

void puerto_coordinador_read(t_config* configuracion) {
	if (config_has_property(configuracion, puertoCoord_planificador)) {
		char* puertoCoord = config_get_string_value(configuracion,puertoCoord_planificador);
		planificador.puerto_coordinador = malloc(strlen(puertoCoord) + 1);
		memcpy(planificador.puerto_coordinador,puertoCoord,strlen(puertoCoord) + 1);
	}
}

void clavesBloqueadas_read(t_config* configuracion) {
	char** claves = NULL;

	if (config_has_property(configuracion, claves_bloqueadas)) {
		claves = string_split(config_get_string_value(configuracion,claves_bloqueadas),",");
	}

	for (int i=0;i<sizeof(claves) - 1;i++) {
		list_add(planificador.clavesBloqueadas,claves[i]);
	}

	liberar_split(claves);
}

void conectar_a_coordinador(t_planificador* pConfig) {
	int pidCoordinador = connect_to_server(pConfig->IP_coordinador,pConfig->puerto_coordinador);
	//Verifico conexion con el coordinador
	if (pidCoordinador < 0) {
		log_error(log_planificador, "Fallo conexión Planificador con el Coordinador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_planificador, "Planificador se conecto con el Coordinador");
	}

	//printf("Se pudo conectar con el coordinador");
	Message* msg= (Message*) malloc(sizeof(Message));

	msg->contenido = (char*) malloc(strlen("Envio mensaje al Coordinador desde Planificador") + 1);
	strcpy(msg->contenido,"Envio mensaje al Coordinador desde Planificador");
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader));
	msg->header->remitente = PLANIFICADOR;
	msg->header->tipo_mensaje = TEST;
	msg->header->size = strlen(msg->contenido)+1;

	if (send_msg(pidCoordinador, (*msg))<0) log_debug(log_planificador, "Error al enviar el mensaje");

	while (1) {
		Message msg;
		log_info(log_planificador, "esperando mensaje");
		int resultado = await_msg(pidCoordinador, &msg);
		log_info(log_planificador, "llego un mensaje. parseando...");
		if (resultado<0){
			log_debug(log_planificador, "error de recepcion");
			continue;
				//return ERROR_DE_RECEPCION;
		}
		//TODO parsear mensaje y hacer algo.
		char * request = malloc(msg.header->size);
		strncpy(request, (char *) msg.contenido, strlen(msg.contenido) + 1);

		log_info(log_planificador, "mensaje recibido: %s", request); //FIXME aparecen caracteres de mas al final del mensaje ???
		//log_debug(log_inst, "%s", request);

		free(msg.contenido);
		free(msg.header);

		break;
	}

		//printf("Se envio el mensaje");
	//log_info(log_planificador, "Planificador envio un mensaje: %s", (*msg).contenido);
}

void exit_proceso(int retorno) {
  log_destroy(log_planificador);
  exit(retorno);
}

void liberar_split(char** array){
	int i = 0;
	while (array[i] != NULL) {
		free(array[i]);
		i++;
	}
	free(array);
}
