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
	log_info(log_planificador,"\nAlfa Planificación: %d", planificador.alfaPlanificacion);

	//Crear listas de procesos
	cola_ready = list_create();
	cola_blocked = list_create();
	cola_finished = list_create();

	//conectar a coordinador
	t_planificador* pConfig = (t_planificador*)&planificador;
	int socketCoordinador = conectar_a_coordinador(pConfig);

	log_info(log_planificador,"\nInicio de la consola\n");

	//Abrir Consola
	pidConsola = pthread_create(&threadConsola, NULL, (void*)&abrir_consola, (void*) "Inicio del hilo de la consola");

	if (pidConsola < 0) {
		log_error(log_planificador,"Error al intentar abrir la consola");
		exit_proceso(-1);
	}

	//pthread_join(threadConsola,NULL);

	//Escuchar conexiones ESI
	algorimoEnUso = FIFO;
	iniciar(socketCoordinador);

	log_info(log_planificador,"\nProceso finalizado");
	list_destroy(planificador.clavesBloqueadas);

	exit_proceso(0);
}

void inicializar_logger() {
	log_planificador = log_create("/home/utnso/tp/Planificador.log", "Planificador: ", true, LOG_LEVEL_INFO);
}

int iniciar(int socketCoordinador){
	log_info(log_planificador, "Iniciando proceso planificador");

	int socket_fd = create_listener(IP,planificador.puerto_planif);
	if (socket_fd <0) return ERROR_DE_CONEXION;

	start_listening_select(socket_fd, socketCoordinador, *manejador_de_eventos);
	//start_listening_threads(socket_fd, *recibir_mensaje);

	return 0;
}

int manejador_de_eventos(int socket, Message* msg){
	log_info(log_planificador, "Ocurrio un evento");
	tipo_mensaje tipo;

	int res = await_msg(socket, msg);
	if (res<0) {
		log_info(log_planificador, "error al recibir un mensaje de %d", socket);
		return ERROR_DE_RECEPCION;
	}

	//Por ahora agrego caso con test para que siga funcionando, después sacar
	enum tipoRemitente recipiente = msg->header->remitente;
	char * request = malloc((msg->header->size));
	strncpy(request, (char *) msg->contenido, strlen(msg->contenido) + 1);

	log_info(log_planificador, "recibi mensaje de %d: %s", recipiente, request);

	if (msg->header->tipo_mensaje==TEST)
	{
		tipo.enumTipo = TEST;

		return enviar_mensaje(socket, "Hola soy el planificador",tipo);
			//return string_itoa(enviar_mensaje(conexion->socket, "Hola soy el planificador"));
	}

	else if(msg->header->remitente == ESI){
		log_info(log_planificador, "Me hablo una ESI");
		switch(msg->header->tipo_mensaje){
			case TEXTO:
				log_info(log_planificador, "Me envio un texto");
				//Por ahora es texto, en un futuro alguna estructura mas compleja
				return manejar_mensaje_esi_fifo(socket, msg);

			case CONEXION:
				log_info(log_planificador, "Se conecto una ESI");
				//Switcheamos por el algoritmo
				switch(algorimoEnUso){
					case FIFO:
						manejar_nueva_esi_fifo(socket);
						return 0;
					default:
						//fuck
						return -2;
				}
				break;

			case DESCONEXION:
				log_info(log_planificador, "Se desconecto una ESI");
				switch(algorimoEnUso){
					case FIFO:
						manejar_desconexion_esi_fifo(socket);
						return 0;
					default:
						//fuck
						return -2;
				}
				break;

			default:
				//fuck
				return 0;
		}//fin del switch
	}else if(msg->header->remitente == COORDINADOR){
		//No se que mensajes manda el coordinador...
	}else if(msg->header->remitente == DESCONOCIDO){
		log_info(log_planificador, "Ocurrio algo con alguien pero no se quien");
	}
	return 0;
	//Aca deberia haber algo para manejar otro tipo de eventos que no sean de la ESI

	return ERROR;
}

/*int recibir_mensaje(Conexion* conexion, Message* msg){
	tipo_mensaje tipo;
	int esi_seleccionado = 0;

	int res = await_msg(conexion->socket, msg);
	if (res<0) {
		log_info(log_planificador, "error al recibir un ensaje de %d", socket);
		return ERROR_DE_RECEPCION;
		//return string_itoa(ERROR_DE_RECEPCION);
	}

	enum tipoRemitente recipiente = msg->header->remitente;
	char * request = malloc((msg->header->size));
	strncpy(request, (char *) msg->contenido, strlen(msg->contenido) + 1);
	//strcpy(request, (char *) msg->contenido);

	log_info(log_planificador, "recibi mensaje de %d: %s", recipiente, request);

	if (msg->header->tipo_mensaje==TEST)
	{
		tipo.enumTipo = TEST;

		return enviar_mensaje(conexion->socket, "Hola soy el planificador",tipo);
		//return string_itoa(enviar_mensaje(conexion->socket, "Hola soy el planificador"));
	}
	else if ((msg->header->remitente==ESI) && (msg->header->tipo_mensaje==CONEXION)) {
		//Agrego la nueva conexión a lista de preparados
		agregar_ready(conexion->socket);

		//Verifico si algún esi está corriendo, caso contrario
		//envio un esi a ejecutarse según el algorimto seleccionado
		if (esiRunning == 0) {
			esi_seleccionado = planificar_esis();
			if (esi_seleccionado > 0) {
			//Primero confirmo la conexión
				tipo.enumTipo = CONEXION;
				int res_conexion = enviar_mensaje(conexion->socket, "Se conectó al planificador correctamente",tipo);

				if (res_conexion < 0) {
					log_debug(log_planificador,"Falló la conexión con el esi: %d",conexion->socket);
					exit_proceso(-1);
				}
				else
				{
					//Envío mensaje para pedirle al esi que ejecute
					tipo.enumTipo = OPERACION;
					return enviar_mensaje(esi_seleccionado, "El planificador ordena ejecutar a este esi",tipo);
				}
			}
		}

		tipo.enumTipo = CONEXION;

		//Por último envío mensaje de confirmación al esi que se conecto
		return enviar_mensaje(conexion->socket, "Se conecto al planificador correctamente",tipo);
	}
	else if ((msg->header->remitente==ESI) && (msg->header->tipo_mensaje==DESCONEXION)) {
		finalizar_esi(conexion->socket);

		esi_seleccionado = planificar_esis();

		if (esi_seleccionado > 0) {
		//Primero confirmo la desconexión
			tipo.enumTipo = ACK;
			int res_conexion = enviar_mensaje(conexion->socket, "Planificador recibió notificación de desconexión",tipo);

			if (res_conexion < 0) {
				log_debug(log_planificador,"Falló la desconexión con el esi: %d",conexion->socket);
				exit_proceso(-1);
			}
			else
			{
				//Envío mensaje para pedirle al esi que ejecute
				tipo.enumTipo = OPERACION;
				return enviar_mensaje(esi_seleccionado, "El planificador ordena ejecutar a este esi",tipo);
			}
		}
	}

	return ERROR;
}

*/

int enviar_mensaje(int socket, char* mensaje, tipo_mensaje tipo){
	Message* msg= (Message*) malloc(sizeof(Message));
	//msg->contenido = (char*) malloc(strlen(mensaje));
	//strncpy(msg->contenido,mensaje,strlen(mensaje));
	msg->contenido = (char*) malloc(strlen(mensaje) + 1);
	strcpy(msg->contenido,mensaje);

	//msg->contenido = mensaje;
	//msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader));
	msg->header->remitente = PLANIFICADOR;
	msg->header->size = strlen(msg->contenido) + 1;
	msg->header->tipo_mensaje = tipo.enumTipo;

	sleep(1);
	log_info(log_planificador, "se va a enviar mensaje desde el planificador mensaje a %d: %s", socket, msg->contenido);
	int res = send_msg(socket, (*msg));
		if (res<0) {
			log_info(log_planificador, "error al enviar mensaje a %d", socket);
			return ERROR_DE_ENVIO;
		}
	log_info(log_planificador, "se envio el mensaje desde el planificador mensaje a %d: %s", socket, msg->contenido);
	free_msg(&msg);

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
	alfaPlanificacion_read(configuracion);
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

void alfaPlanificacion_read(t_config* configuracion) {
	if (config_has_property(configuracion, alfa_planificador)) {
		planificador.alfaPlanificacion = config_get_int_value(configuracion,alfa_planificador);
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

int conectar_a_coordinador(t_planificador* pConfig) {
	int pidCoordinador = connect_to_server(pConfig->IP_coordinador,pConfig->puerto_coordinador);
	//Verifico conexion con el coordinador
	if (pidCoordinador < 0) {
		log_error(log_planificador, "Fallo conexión Planificador con el Coordinador");
		return -1;
	} else {
		log_info(log_planificador, "Planificador se conecto con el Coordinador");
	}

	//printf("Se pudo conectar con el coordinador");
	Message* msg= empaquetar_texto("Envio mensaje al Coordinador desde Planificador",
			strlen("Envio mensaje al Coordinador desde Planificador"),
			PLANIFICADOR);

	//Para mantener el funcionamiento
	msg->header->tipo_mensaje = TEST;

	//Chequeo que se haya enviado correctamente, registro y lo elimino
	if (send_msg(pidCoordinador, (*msg))<0) log_debug(log_planificador, "Error al enviar el mensaje hacia el planificador");
	else log_debug(log_planificador, "Mensaje enviado al coordinador corectamente");
	free_msg(&msg);

	//Espero mensaje de respuesta
	log_debug(log_planificador, "esperando mensaje");

	msg = malloc(sizeof(Message));
	int resultado = await_msg(pidCoordinador, msg);

	log_info(log_planificador, "llego un mensaje. parseando...");

	if (resultado<0){
		log_debug(log_planificador, "error de recepcion");
		//return ERROR_DE_RECEPCION;
	}else{
		char *respuesta = desempaquetar_texto(msg);
		log_info(log_planificador, "mensaje recibido: %s", respuesta);
		free(respuesta);
		free_msg(&msg);
	}

	return pidCoordinador;
}

void agregar_ready(int idEsi) {
	struct_ready elemento;
	elemento.pid = idEsi;
	list_add(cola_blocked,&elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, a la lista de listos",idEsi);
}

void agregar_blocked(int idEsi, char* clave) {
	struct_blocked elemento;
	elemento.pid = idEsi;
	elemento.clave = (char*)malloc(strlen(clave)+1);
	strcpy(elemento.clave,clave);
	list_add(cola_blocked,&elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, clave %s a la lista de bloqueados",idEsi,clave);
}

void agregar_finished(int idEsi) {
	struct_finished elemento;
	elemento.pid = idEsi;
	list_add(cola_blocked,&elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, a la lista de finalizados",idEsi);
}

struct_ready* seleccionar_esi_ready_fifo() {
	struct_ready* esi_seleccionado;

	//Si hay esis esperando, selecciona al primero de la lista
	if (list_size(cola_ready) > 0) {
		esi_seleccionado = (struct_ready*)list_get(cola_ready, 0);
		return esi_seleccionado;
	}

	//Si no encuentro nada retorno null
	return NULL;
}

int planificar_esis() {
	int esiSeleccionado = 0;

	if (strcmp(planificador.algoritmo_planif,"FIFO")) {
		struct_ready* primer_esi = seleccionar_esi_ready_fifo();
		if (primer_esi != NULL){
			//Obtengo el seleccionado para enviar un mensaje, y me lo guardo en variable
			//global como proceso que está ejecutando
			esiSeleccionado = primer_esi->pid;
			esiRunning = esiSeleccionado;
		}
	}
	else if (strcmp(planificador.algoritmo_planif,"SJF-SD")) {

	}

	else if (strcmp(planificador.algoritmo_planif,"SJF-CD")) {

	}

	else if (strcmp(planificador.algoritmo_planif,"HRRN")) {

	}

	else
	{
		log_debug(log_planificador,"El algoritmo de planificación %s es inválido",planificador.algoritmo_planif);
		exit_proceso(-1);
	}

	return esiSeleccionado;
}

void finalizar_esi(int socket_esi) {
	agregar_finished(socket_esi);
	esiRunning = 0;
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

//Funciones dummy para que corra provisionalmente
void manejar_nueva_esi_fifo(int socket){
	tipo_mensaje tipo;
	int esi_seleccionado = 0;
	//Agrego la nueva conexión a lista de preparados
	agregar_ready(socket);

	//Verifico si algún esi está corriendo, caso contrario
	//envio un esi a ejecutarse según el algorimto seleccionado
	if (esiRunning == 0) {
			esi_seleccionado = planificar_esis();
			if (esi_seleccionado > 0) {
				tipo.enumTipo = CONEXION;

				//Primero confirmo la conexión
				int res_conexion = enviar_mensaje(socket, "Se conectó al planificador correctamente",tipo);

				if (res_conexion < 0) {
					log_debug(log_planificador,"Falló la conexión con el esi: %d",socket);
					exit_proceso(-1);
				}
				else
				{   tipo.enumTipo = CONEXION;
					//Envío mensaje para pedirle al esi que ejecute
					//DESPUES SEGURAMENTE CAMBIAR PARA QUE LE ENVIE UNA INSTRUCCION A PARSEAR
					enviar_mensaje(esi_seleccionado, "El planificador ordena ejecutar a este esi",tipo);
				}
			}
		}

	tipo.enumTipo = CONEXION;

	//Por último envío mensaje de confirmación al esi que se conecto
	enviar_mensaje(socket, "Se conecto al planificador correctamente",tipo);
}

int manejar_mensaje_esi_fifo(int socket, Message *msg){return 0;}

void manejar_desconexion_esi_fifo(int socket){
	tipo_mensaje tipo;
	int esi_seleccionado = 0;

	//Le confirmo al esi que se puede desconectar
	tipo.enumTipo = ACK;
	int res_conexion = enviar_mensaje(socket, "Planificador recibió notificación de desconexión",tipo);

	if (res_conexion < 0) {
		log_debug(log_planificador,"Falló la desconexión con el esi: %d",socket);
		exit_proceso(-1);
	}

	finalizar_esi(socket);

	esi_seleccionado = planificar_esis();

	if (esi_seleccionado > 0) {
		//Envío mensaje para pedirle al esi que ejecute
		//DESPUES SEGURAMENTE CAMBIAR PARA QUE LE ENVIE UNA INSTRUCCION A PARSEAR
/**********************USAR UN TIPO DE OPERACION ESPECIFICO *************************/
		//FIXME tipo.enumTipo = OPERACION;
		enviar_mensaje(esi_seleccionado, "El planificador ordena ejecutar a este esi",tipo);
	}
}
