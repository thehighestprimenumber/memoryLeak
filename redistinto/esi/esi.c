#include "esi.h"

int main(int argc,char *argv[]) {

	// Levantamos configuracion

	esi_configuracion* pConfig = (esi_configuracion*) malloc(sizeof(esi_configuracion));;
	t_config* config = config_create("./configESI.txt");

	sem_init(&lock_resultados, 0, 1);

	if (config == NULL) {
		config = config_create("../configESI.txt");
	}
	if (argv==NULL || argv[1]==NULL) {
		argv[1] = "ESI_1";
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

	conectar_a_coordinador(pConfig);
	conectar_a_planificador(pConfig);
	while (1) {
			Message msg;
			log_debug(log_esi, "esperando mensaje");
			int resultado = await_msg(socket_planificador, &msg);
			log_debug(log_esi, "llego un mensaje. parseando...");
			if (resultado<0){
				log_debug(log_esi, "error de recepcion");
				return ERROR_DE_RECEPCION;
			}

			manejador_mensajes(msg);

			//char * request = desempaquetar_texto(&msg);
			log_debug(log_esi, "mensaje recibido: %i", msg.header->tipo_mensaje);

		}
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

	enviar_ruta_script_al_planificador(path_script);
	//Message* msg = empaquetar_texto("Envio mensaje al Planificador desde ESI", strlen("Envio mensaje al Planificador desde ESI"), ESI);

	//a confirmar. Habria que cambiar tipo TEST por CONEXION en todos lados
	//msg->header->tipo_mensaje = CONEXION;
	free(path_script);

	//if (send_msg(socket_planificador, (*msg))<0) log_debug(log_esi, "Error al enviar el mensaje");
	//log_debug(log_esi, "Se envio el mensaje");



	//free_msg(&msg);

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
		return OK;

}

t_operacion* convertir_operacion(t_esi_operacion operacionOriginal){

	if (operacionOriginal.valido == false) {
		return CLAVE_MUY_GRANDE; //FIXME acá estás devolviendo un int, no un t_operacion*.
								// la solución sería recibir un t_operacion* como parámetro
								// al que le editas el contenido. y devolver un OK cuando sale bien
								// y sino, los mensajes de error que correspondan
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
		operacionNueva->valor = calloc(1, 1);
		strcpy(operacionNueva->valor, "\0");
		operacionNueva->largo_valor=strlen(operacionNueva->valor)+1;
		break;
	default:
		log_error(log_esi, "Error al convertir la operacion solicitada.");
	}

	return operacionNueva;

}

void* enviar_operacion_a_coordinador(t_operacion* operacion){
	//FIXME acá devovles un int, pero la función declara return tipo void*
	// y aparte la función que llama a esta no mira el resultado.
	Message * msg = empaquetar_op_en_mensaje(operacion, ESI);

	int res = send_msg(cliente_coordinador, *msg);
	if (res < 0) exit(-1);
	return NULL;

	//Message *rta;
	//int resultado = await_msg(cliente_coordinador, rta);
	//free_msg(&rta);
	//if (resultado < 0){
	//	return ERROR_DE_RECEPCION;
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
	char** split = string_n_split(script.script_contenido, script.script_largo,(char*) "\n");

	for(int i = 0; split[i] != NULL; i++){
		t_operacion * op = convertir_operacion(parse(split[i]));
		enviar_operacion_a_coordinador(op);
		free_operacion(&op);
		Message *rta = malloc(sizeof(Message));
		if(await_msg(cliente_coordinador, rta) < 0){
			log_info(log_esi, "error al recibir resultado");
			exit(-1);
		}
		if(desempaquetar_resultado(rta) != OK){
			log_info(log_esi, "error en la operacion: %d", desempaquetar_resultado(rta));
			exit(-1);
		}
		log_info(log_esi, "operacion OK: %d", desempaquetar_resultado(rta));
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
			mensaje.header->remitente = ESI;
			send_msg(socket_planificador, mensaje);
			sem_post(&lock_resultados);
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
