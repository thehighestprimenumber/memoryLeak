#include "esi.h"

int main(int argc,char *argv[]) {

	// Levantamos configuracion

	esi_configuracion* pConfig = (esi_configuracion*) malloc(sizeof(esi_configuracion));;
	t_config* config = config_create("./configESI.txt");

	if (config == NULL) {
		config = config_create("../configESI.txt");
	}

	path_script = malloc(strlen(argv[1]) + 1);
	memcpy(path_script,argv[1],strlen(argv[1]));
	((char*) path_script)[strlen(argv[1])] = '\0';

	pConfig->coordinador_ip = leer_propiedad_string(config, "IP_coordinador");
	pConfig->coordinador_puerto = leer_propiedad_string(config, "puerto_coordinador");
	pConfig->planificador_ip = leer_propiedad_string(config, "IP_planificador");
	pConfig->planificador_puerto = leer_propiedad_string(config, "puerto_planificador");

	// Levantamos el archivo de log y guardamos IP y Puerto
	log_esi = log_create("log_esi.log", "ESI", true, LOG_LEVEL_TRACE);
	log_trace(log_esi,"Inicia el proceso ESI");
	log_trace(log_esi,"El puerto del coordinador es: %s",pConfig->coordinador_puerto);
	log_trace(log_esi,"La ip del coordinador es: %s",pConfig->coordinador_ip);
	log_trace(log_esi,"El puerto del planificador es: %s",pConfig->planificador_puerto);
	log_trace(log_esi,"La ip del planificador es: %s",pConfig->planificador_ip);
	log_trace(log_esi,"El nombre del script es: %s",argv[1]);

	pidcoordinador = pthread_create(&threadCoordinador, NULL, (void*)&conectar_a_coordinador, (void*) pConfig);
		if (pidcoordinador < 0) {
			log_error(log_esi,"Error al intentar conectar al coordinador");
			exit(EXIT_FAILURE);
		}

	pidplanificador = pthread_create(&threadPlanificador, NULL, (void*)&conectar_a_planificador, (void*) pConfig);
	if (pidplanificador < 0) {
			log_error(log_esi,"Error al intentar conectar al planificador");
			exit(EXIT_FAILURE);
		}

	pthread_join(threadPlanificador,NULL);
	pthread_join(threadCoordinador,NULL);

	//Recibo la ruta del script a ejecutar y se la envio al planificador
	//char* path_script = argv[1];
	//enviar_ruta_script_al_planificador(path_script);

	return EXIT_SUCCESS;
}

// Funciones de inicialización

char* leer_propiedad_string (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_string_value(configuracion,propiedad);
	}
	return "";
}

int conectar_a_planificador(esi_configuracion* pConfig) {

	sleep(2);

	socket_planificador = connect_to_server(pConfig->planificador_ip,pConfig->planificador_puerto);
	//Verifico conexion con el planificador
	if (socket_planificador < 0) {
		log_error(log_esi, "Fallo conexion esi con el Planificador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Planificador");
	}

	Message* msg = empaquetar_texto("Envio mensaje al Planificador desde ESI", strlen("Envio mensaje al Planificador desde ESI"), ESI);

	//a confirmar. Habria que cambiar tipo TEST por CONEXION en todos lados
	msg->header->tipo_mensaje = CONEXION;

	if (send_msg(socket_planificador, (*msg))<0) log_debug(log_esi, "Error al enviar el mensaje");
	log_debug(log_esi, "Se envio el mensaje");

	while (1) {
		Message msg;
		log_debug(log_esi, "esperando mensaje");
		int resultado = await_msg(socket_planificador, &msg);
		log_debug(log_esi, "llego un mensaje. parseando...");
		if (resultado<0){
			log_debug(log_esi, "error de recepcion");
			return ERROR_DE_RECEPCION;
		}

		char * request = desempaquetar_texto(&msg);
		log_debug(log_esi, "mensaje recibido: %s", request);

	}

	free_msg(&msg);

}

int conectar_a_coordinador(esi_configuracion* pConfig) {

	cliente_coordinador = connect_to_server(pConfig->coordinador_ip,pConfig->coordinador_puerto);
	//Verifico conexion con el coordinador
	if (cliente_coordinador == -1) {
		log_error(log_esi, "Fallo conexion con el Coordinador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Coordinador");
	}

	Message* msg = empaquetar_texto("Hola coordinador", strlen("Hola coordinador"), ESI);
	msg->header->tipo_mensaje = TEST;

	if (send_msg(cliente_coordinador, (*msg))<0) log_debug(log_esi, "Error al enviar el mensaje");
	log_debug(log_esi, "Se envio el mensaje");
	while (1) {
		Message msg;
		log_debug(log_esi, "esperando mensaje");
		int result = await_msg(cliente_coordinador, &msg);
		log_debug(log_esi, "llego un mensaje. parseando...");
		if (result<0){
			log_debug(log_esi, "error de recepcion");
			continue;
			return ERROR_DE_RECEPCION;
		}
	char * request = malloc(msg.header->size);
	strncpy(request, (char *) msg.contenido, strlen(msg.contenido) + 1);
	log_debug(log_esi, "mensaje recibido: %s", request);

	return OK;
	}
}

t_operacion* convertir_operacion(t_esi_operacion operacionOriginal){

	if (operacionOriginal.valido == false) {
		return CLAVE_MUY_GRANDE;
	}

	t_operacion* operacionNueva = (t_operacion*) malloc(sizeof(t_operacion));

	switch(operacionOriginal.keyword){
	case GET:
		operacionNueva->tipo = op_GET;
		operacionNueva->clave = calloc(1, strlen(operacionOriginal.argumentos.GET.clave)+1);
		memcpy(operacionNueva->clave, operacionOriginal.argumentos.GET.clave, strlen(operacionOriginal.argumentos.GET.clave));
		operacionNueva->largo_clave = strlen(operacionNueva->clave)+1;
		//Asigno valores vacios para que funcione la operacion
		operacionNueva->valor = calloc(1, 1);
		strcpy(operacionNueva->valor, "\0");
		operacionNueva->largo_valor=strlen(operacionNueva->valor)+1;
		break;
	case SET:
		operacionNueva->tipo = op_SET;
		operacionNueva->clave = calloc(1, strlen(operacionOriginal.argumentos.SET.clave)+1);
		memcpy(operacionNueva->clave, operacionOriginal.argumentos.SET.clave, strlen(operacionOriginal.argumentos.SET.clave));
		operacionNueva->largo_clave = strlen(operacionNueva->clave)+1;
		operacionNueva->valor = calloc(1, strlen(operacionOriginal.argumentos.SET.valor)+1);
		strcpy(operacionNueva->valor, operacionOriginal.argumentos.SET.valor);
		operacionNueva->largo_valor=strlen(operacionNueva->valor)+1;
		break;
	case STORE:
		operacionNueva->tipo = op_STORE;
		operacionNueva->clave = calloc(1, strlen(operacionOriginal.argumentos.STORE.clave)+1);
		memcpy(operacionNueva->clave, operacionOriginal.argumentos.STORE.clave, strlen(operacionOriginal.argumentos.STORE.clave));
		operacionNueva->largo_clave = strlen(operacionNueva->clave)+1;
		break;
	default:
		log_error(log_esi, "Error al convertir la operacion solicitada.");
	}

	return operacionNueva;

}

void* enviar_operacion_a_coordinador(t_operacion* operacion){

	Message * msg = empaquetar_op_en_mensaje(operacion, ESI);

	int res = send_msg(cliente_coordinador, *msg);
	if (res < 0) return ERROR_DE_ENVIO;

	//TODO: Segun la respuesta que recibo deberia bloquear la ESI o desconectarla
	//El while ya lo hace cuando recorre las sentencias del script
	//while (1) {
		Message *rta;
		int resultado = await_msg(cliente_coordinador, rta);
		free_msg(&rta);
		if (resultado < 0){
			return ERROR_DE_RECEPCION;
		}
	//}
}

void enviar_ruta_script_al_planificador(char* path){

	Message* msg = empaquetar_texto(path, strlen(path), ESI);
	msg->header->tipo_mensaje = CONEXION;

	if (send_msg(socket_planificador, (*msg))<0) log_debug(log_esi, "Error al enviar la ruta del script.");
	log_debug(log_esi, "Se envio la ruta del script.");

}

int cantidad_lineas_script(char* script){

	int n_lineas = 0;

	for(int i = 0; script[i] != '\0'; i++){
	if (script[i] == '\n') n_lineas++;
	}

	return n_lineas + 1;

}

void correr_script(){

	//int n_lineas = cantidad_lineas_script(script);
	char** split = string_n_split(script.script_contenido, script.script_largo - 1,(char*) "\n");

	for(int i = 0; split[i] != NULL; i++){
		enviar_operacion_a_coordinador(convertir_operacion(parse(split[i])));
	}

}

void manejador_mensajes(Message mensaje) {
	log_info(log_esi, "LLegó un mensaje");

	if(mensaje.header->remitente == PLANIFICADOR) {
		//Recibe contenido del script
		if (mensaje.header->tipo_mensaje == CONEXION) {
			log_info(log_esi, "Recibido el contenido del script");

			armar_estructura_script(mensaje.contenido);
		}

		//Recibo ok para ejecutar sus instrucciones
		if (mensaje.header->tipo_mensaje == EJECUTAR) {
			correr_script();
		}
	}

	if(mensaje.header->remitente == COORDINADOR) {
		if (mensaje.header->tipo_mensaje == RESULTADO) {
			send_msg(socket_planificador, mensaje);
		}
	}
}

void armar_estructura_script(char* contenidoScript) {
	script.script_contenido = malloc(strlen(contenidoScript));
	memcpy(script.script_contenido,"",1);
	strcat(script.script_contenido,contenidoScript);
	//memcpy(script.script_contenido,contenidoScript,strlen(contenidoScript));
	((char*) script.script_contenido)[strlen(script.script_contenido)] = '\0';
	script.script_largo = cantidad_lineas_script(script.script_contenido);
}
