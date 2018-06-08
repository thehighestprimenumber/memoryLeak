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
	cola_esi_blocked = list_create();
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

char* armarPathScript(char* cadenaPath,char* nombreScript) {
	char* cadenaScript = malloc(strlen(cadenaPath) + strlen(nombreScript) + 1);
	memcpy(cadenaScript,cadenaPath,strlen(cadenaPath) + 1);
	strcat(cadenaScript,nombreScript);
	//strcat(cadenaScript,".txt");
	return cadenaScript;
}

void leer_script_completo(char* nombreArchivo) {
	//Abrir archivo para los esis
	//script_a_procesar = txt_open_for_append(scriptTxt);
	//if (script_a_procesar == NULL)
	//	script_a_procesar = txt_open_for_append(scriptTxtDebug);

	FILE *f = fopen(armarPathScript(scriptTxtDebug,nombreArchivo), "rb");
		if (f == NULL) {
			f = fopen(armarPathScript(scriptTxt,nombreArchivo), "rb");
			if (f == NULL) {
				//perror("fopen");
				//exit(EXIT_FAILURE);
				contenidoScript = malloc(strlen(" "));
				memcpy(contenidoScript," ",strlen(" "));
			}
		}

		fseek(f,0,SEEK_END);
		long fsize = ftell(f);
		fseek(f,0,SEEK_SET);

		contenidoScript = malloc(fsize + 1);
		if (contenidoScript == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		fread(contenidoScript, fsize, 1, f);

		fclose(f);

		contenidoScript[fsize] = '\0';
}

int iniciar(int socketCoordinador){
	log_info(log_planificador, "Iniciando proceso planificador");

	int socket_fd = create_listener(IP,planificador.puerto_planif);
	if (socket_fd <0) return ERROR_DE_CONEXION;

	start_listening_select(socket_fd, socketCoordinador, *manejador_de_eventos);

	return 0;
}

int manejador_de_eventos(int socket, Message* msg){
	log_info(log_planificador, "Ocurrio un evento del tipo: %d", msg->header->tipo_mensaje);

	//En el caso de las operaciones el socket ya escucha el mensaje por lo que esto no va
	if (msg->header->tipo_mensaje != OPERACION && msg->header->tipo_mensaje != RESULTADO)
	{
		int res = await_msg(socket, msg);
		if (res<0) {
			log_info(log_planificador, "RIP socket %d", socket);
			close(socket);
			return ERROR_DE_RECEPCION;
		}
	}

	//Por ahora agrego caso con test para que siga funcionando, después sacar
	enum tipoRemitente recipiente = msg->header->remitente;

	if (msg->header->tipo_mensaje==TEST)
	{
		char * request = desempaquetar_texto(msg);
		log_info(log_planificador, "recibi mensaje de %d: %s", recipiente, request);

		Message* mensaje = empaquetar_texto("Hola soy el planificador\0", strlen("Hola soy el planificador\0"), PLANIFICADOR);
		mensaje->header->tipo_mensaje = ACK;
		int result = enviar_mensaje(socket, *mensaje);
		if (result) {
			log_info(log_planificador, "error al enviar ack");
			return ERROR_DE_ENVIO;
		}

		//return enviar_mensaje(socket, "Hola soy el planificador",tipo);
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
				aceptar_conexion(socket, msg->contenido);
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
				loguear_desconexion(socket);
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
		log_info(log_planificador, "Me hablo el Coordinador");
		switch(msg->header->tipo_mensaje){
			case OPERACION:
				log_info(log_planificador, "Me pidió validar un get o un set");

				sleep(1);

				//Aca diferenció que operación me pide verificar el coordinador
				//y actuo según el caso
				int resultado_operacion = manejar_operacion(socket,msg);

				Message* mensaje = empaquetar_resultado(PLANIFICADOR, resultado_operacion);
				int result = enviar_mensaje(socket, *mensaje);
				if (result) {
					log_info(log_planificador, "error al enviar resultado al coordinador");
					return ERROR_DE_ENVIO;
				}

				//Por ahora es texto, en un futuro alguna estructura mas compleja
				return resultado_operacion;
				break;
			default:
				//fuck
			return 0;
		}//fin del switch
	}else if(msg->header->remitente == DESCONOCIDO){
		log_info(log_planificador, "Ocurrio algo con alguien pero no se quien");
	}
	return 0;
	//Aca deberia haber algo para manejar otro tipo de eventos que no sean de la ESI

	return ERROR;
}

int enviar_mensaje(int socket, Message msg) {
	//log_info(log_planificador, "se va a enviar mensaje desde el planificador mensaje a %d: %s", socket, msg.contenido);
	int res = send_msg(socket, msg);
		if (res<0) {
			//log_info(log_planificador, "error al enviar mensaje a %d", socket);
			loguear_error_envio(&msg, socket);
			return ERROR_DE_ENVIO;
		}
	//log_info(log_planificador, "se envio el mensaje desde el planificador mensaje a %d: %s", socket, msg.contenido);
	loguear_envio_OK(&msg, socket);
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

	//liberar_split(claves);
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

	Message * mensaje = empaquetar_texto("Planificador conecta a coordinador\0", strlen("Planificador conecta a coordinador\0"), PLANIFICADOR);
	mensaje->header->tipo_mensaje = CONEXION;
	int resultado = enviar_mensaje(pidCoordinador, *mensaje);

	if (resultado < 0) {
		log_error(log_planificador, "Fallo envio mensaje conexión al Coordinador");
		return -1;
	}

	return pidCoordinador;
}

void aceptar_conexion(int socket, char* nombreScript) {
	//Por último envío mensaje de confirmación al esi que se conecto
	leer_script_completo(nombreScript);
	Message* mensaje = empaquetar_texto(contenidoScript, strlen(contenidoScript), PLANIFICADOR);
	mensaje->header->tipo_mensaje = CONEXION;
	enviar_mensaje(socket, *mensaje);
	free(mensaje);
}

void agregar_ready(int idEsi) {
	struct_ready* elemento = malloc(sizeof(struct_ready));
	elemento->pid = idEsi;
	list_add(cola_ready,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, a la lista de listos",idEsi);
}

void agregar_blocked(int idEsi, char* clave) {
	struct_blocked* elemento = malloc(sizeof(struct_blocked));
	elemento->pid = idEsi;
	elemento->clave = (char*)malloc(strlen(clave)+1);
	strcpy(elemento->clave,clave);
	list_add(cola_blocked,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, clave %s a la lista de bloqueados",idEsi,clave);
}

void agregar_esi_blocked(int idEsi, char* clave) {
	struct_blocked* elemento = malloc(sizeof(struct_blocked));
	elemento->pid = idEsi;
	elemento->clave = (char*)malloc(strlen(clave)+1);
	strcpy(elemento->clave,clave);
	list_add(cola_esi_blocked,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, clave %s a la lista de esis bloqueados por clave tomada anteriormente",idEsi,clave);
}

void agregar_finished(int idEsi) {
	struct_finished* elemento = malloc(sizeof(struct_finished));
	elemento->pid = idEsi;
	list_add(cola_blocked,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, a la lista de finalizados",idEsi);
}

struct_ready* seleccionar_esi_ready_fifo() {
	struct_ready* esi_seleccionado;

	//Si hay esis esperando, selecciona al primero de la lista
	if (list_size(cola_ready) > 0) {
		esi_seleccionado = list_get(cola_ready, 0);
		return esi_seleccionado;
	}

	//Si no encuentro nada retorno null
	return NULL;
}

int planificar_esis() {
	int esiSeleccionado = 0;

	if (strcmp(planificador.algoritmo_planif,"FIFO") == 0) {
		struct_ready* primer_esi = seleccionar_esi_ready_fifo();
		if (primer_esi != NULL){
			//Obtengo el seleccionado para enviar un mensaje, y me lo guardo en variable
			//global como proceso que está ejecutando
			esiSeleccionado = primer_esi->pid;
			esiRunning = esiSeleccionado;
		}
	}
	else if (strcmp(planificador.algoritmo_planif,"SJF-SD") == 0) {

	}

	else if (strcmp(planificador.algoritmo_planif,"SJF-CD") == 0) {

	}

	else if (strcmp(planificador.algoritmo_planif,"HRRN") == 0) {

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
int manejar_nueva_esi_fifo(int socket){

	//Agrego la nueva conexión a lista de preparados
	agregar_ready(socket);

	//Verifico si algún esi está corriendo, caso contrario
	//envio un esi a ejecutarse según el algorimto seleccionado
	if (esiRunning == 0) {
		ejecutar_nueva_esi();
	}

	return OK;
}

int manejar_mensaje_esi_fifo(int socket, Message *msg){return 0;}

void manejar_desconexion_esi_fifo(int socket){
	//Le confirmo al esi que se puede desconectar
	Message* mensaje = empaquetar_texto("Planificador recibió notificación de desconexión\0", strlen("Planificador recibió notificación de desconexión\0"), PLANIFICADOR);
	mensaje->header->tipo_mensaje = ACK;

	int res_desconexion = enviar_mensaje(socket, *mensaje);

	free(mensaje);

	if (res_desconexion < 0) {
		log_debug(log_planificador,"Falló la desconexión con el esi: %d",socket);
		exit_proceso(-1);
	}

	finalizar_esi(socket);

	ejecutar_nueva_esi();
}

int manejar_operacion(int socket,Message* msg) {
	operacionEnMemoria = desempaquetar_operacion(msg);

	if (operacionEnMemoria->largo_clave - 2 > 40)
	{
		free_operacion(&operacionEnMemoria);
		return CLAVE_MUY_GRANDE;
	}

	if (operacionEnMemoria->tipo == op_GET) {
		return validar_operacion_get();
	}
	else if (operacionEnMemoria->tipo == op_SET) {
		return validar_operacion_set();
	}
	else if (operacionEnMemoria->tipo == op_STORE) {
		return validar_operacion_store();
	}
	else
	{
		//esto no debería pasar nunca
		free_operacion(&operacionEnMemoria);
		exit_proceso(-1);
	}

	return OK;
}

int validar_operacion_get() {
	if (list_any_satisfy(planificador.clavesBloqueadas, ((void*) clave_ya_bloqueada_config)))
	{
		agregar_esi_blocked(esiRunning, operacionEnMemoria->clave);
		free_operacion(&operacionEnMemoria);
		return CLAVE_DUPLICADA;
	}
	else
	{
		if (list_any_satisfy(cola_blocked, (void*) clave_ya_bloqueada))
		{
			agregar_esi_blocked(esiRunning, operacionEnMemoria->clave);
			free_operacion(&operacionEnMemoria);
			return CLAVE_DUPLICADA;
		}
	}

	//Si la clave no está tomada, la agrego a lista de bloqueados
	agregar_blocked(esiRunning, operacionEnMemoria->clave);
	free_operacion(&operacionEnMemoria);
	return OK;
}

int validar_operacion_set() {
	if (!list_any_satisfy(cola_blocked, ((void*) clave_set_disponible)))
	{
		//kill(esiRunning,SIGTERM);
		free_operacion(&operacionEnMemoria);
		return CLAVE_INEXISTENTE;
	}

	//Si puede tomar la clave sin problemas, simplemente retorna ok
	free_operacion(&operacionEnMemoria);
	return OK;
}

int validar_operacion_store() {
	//Si está entre las claves bloqueadas por config lo libero
	list_remove_by_condition(planificador.clavesBloqueadas, ((void*) clave_ya_bloqueada_config));

	//quito la clave con el respectivo esi que la tenía bloqueada
	list_remove_by_condition(cola_blocked, ((void*) clave_set_disponible));

	//Desbloqueo al primer esi que tuviera la clave tomada
	desbloquear_esi();

	free_operacion(&operacionEnMemoria);
	return OK;
}

void ejecutar_nueva_esi() {
	int esi_seleccionado = planificar_esis();

	if (esi_seleccionado > 0) {
	//Envío mensaje para pedirle al esi que ejecute. El ESI es quien debería abrir su archivo
	//y comenzar a procesar instrucciones
	Message* mensajeEjec = empaquetar_texto("Planificador pide ejecutar ESI", strlen("Planificador pide ejecutar ESI") + 1, PLANIFICADOR);
	mensajeEjec->header->tipo_mensaje = EJECUTAR;

	int res_ejecutar = enviar_mensaje(esi_seleccionado, *mensajeEjec);
	free(mensajeEjec);
	if (res_ejecutar < 0) {exit_proceso(-1);}
	}
}

bool clave_ya_bloqueada_config(char* clave) {
	return (strcmp(operacionEnMemoria->clave,clave) == 0);
}

bool clave_ya_bloqueada(struct_blocked elemento) {
	return (strcmp(operacionEnMemoria->clave,elemento.clave) == 0);
}

bool clave_set_disponible(struct_blocked* elemento) {
	return (strcmp(operacionEnMemoria->clave,elemento->clave) == 0 && (elemento->pid == esiRunning));
}

void desbloquear_esi() {
	struct_blocked* esi_a_desbloquear;
	esi_a_desbloquear = (struct_blocked*)list_find(cola_esi_blocked, (void*)clave_ya_bloqueada);
	if (esi_a_desbloquear != NULL)
	{
		list_remove_by_condition(cola_esi_blocked, ((void*) clave_set_disponible));
		agregar_ready(esi_a_desbloquear->pid);
	}
}
