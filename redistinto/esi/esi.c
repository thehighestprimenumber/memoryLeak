#include "esi.h"
#define nombre_script "ESI_Spoilers"
//#define nombre_script "ESI_ClaveLarga"

esi_configuracion* inicializar_configuracion(char* argv[]);
int enviar_y_loguear_mensaje(int socket, Message msg, char* destinatario);

int main(int argc,char *argv[]) {
	// Levantamos configuracion -  si argv esta vacio levanta ESI_1
	esi_configuracion* pConfig = inicializar_configuracion(argv);
	conectar_a_coordinador(pConfig);
	conectar_a_planificador(pConfig);

	esi_correr = true;
	//TODO sacar esi_correr para poder soportar bloqueos temporales.
	while (esi_correr) {
			Message *msg = malloc(sizeof(Message));
			msg->header = NULL;
			msg->contenido = NULL;
			int resultado = await_msg(socket_planificador, msg);
			//log_info(log_esi, "se recibe un mensaje del planificador");
			if (resultado<0){
				log_error(log_esi, "error de recepcion");
				break;
			}

			manejar_mensajes(*msg);
			free_msg(&msg);

	}
	//Recibo la ruta del script a ejecutar y se la envio al planificador
	//char* path_script = argv[1];
	//enviar_ruta_script_al_planificador(path_script);

	//Deberia liberar toda la lista con un list_destroy_and_destroy_elements(2)
	free(pConfig->coordinador_ip);
	free(pConfig->coordinador_puerto);
	free(pConfig->planificador_ip);
	free(pConfig->planificador_puerto);
	free(pConfig);
	return EXIT_SUCCESS;
}

// Funciones de inicialización

char* leer_propiedad_string (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_string_value(configuracion,propiedad);
	}
	return "";
}

void conectar_a_planificador(esi_configuracion* pConfig) {
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

void conectar_a_coordinador(esi_configuracion* pConfig) {

	cliente_coordinador = connect_to_server(pConfig->coordinador_ip,pConfig->coordinador_puerto);
	//Verifico conexion con el coordinador
	if (cliente_coordinador == -1) {
		log_error(log_esi, "Fallo conexion con el Coordinador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Coordinador");
	}

}

t_operacion * crearOperacion(tipoOperacion tipo, char* clave, int largoClave, char* valor, int largoValor) {
	t_operacion * operacion = (t_operacion * ) calloc(1, sizeof(t_operacion));
	operacion->tipo = tipo;
	operacion->largo_clave = largoClave;
	operacion->largo_valor = largoValor;
	operacion->clave = (char*) calloc(1, largoClave+1);
	operacion->valor = (char*) calloc(1, largoValor+1);
	memcpy(operacion->clave, clave, largoClave);
	memcpy(operacion->valor, valor, largoValor);
	return operacion;
}

t_operacion * convertir_operacion(t_esi_operacion operacionOriginal){
	switch(operacionOriginal.keyword){
	case GET:
		return crearOperacion(op_GET, operacionOriginal.argumentos.GET.clave, strlen(operacionOriginal.argumentos.GET.clave) + 1, "__", 2);
		break;
	case SET:
		return crearOperacion(op_SET, operacionOriginal.argumentos.SET.clave, strlen(operacionOriginal.argumentos.SET.clave) + 1,
				operacionOriginal.argumentos.SET.valor, strlen(operacionOriginal.argumentos.SET.valor) + 1);
		break;
	case STORE:
		return crearOperacion(op_STORE, operacionOriginal.argumentos.STORE.clave, strlen(operacionOriginal.argumentos.STORE.clave) + 1, "__", 2);
		break;
	}

	return OK;

}

void enviar_operacion_a_coordinador(t_operacion* operacion){
	Message* msg;
	empaquetar_op_en_mensaje(operacion, ESI, &msg);

	int res = enviar_y_loguear_mensaje(cliente_coordinador, *msg, "coordinador\0");
	free_msg(&msg);
	if (res < 0) exit(-1);
}

void enviar_ruta_script_al_planificador(char* path){

	Message* msg;
	empaquetar_conexion(path, strlen(path), ESI, &msg);

	int res = enviar_y_loguear_mensaje(socket_planificador, (*msg), "planificador\0");
		free_msg(&msg);
	if (res<0)
		exit(ERROR_DE_ENVIO);

}

int enviar_y_loguear_mensaje(int socket, Message msg, char* destinatario) {
	int res = send_msg(socket, msg);
	if (res < 0) {
		loguear_error_envio(log_esi, &msg, destinatario);
		return ERROR_DE_ENVIO;
	}
	loguear_envio_OK(log_esi, &msg, destinatario);
	return OK;
}

int ejecutar_proxima_operacion(){
	if (operaciones->head == NULL) {
		//envio_desconexion(socket_planificador);
		esi_correr = false; //TODO reemplazar esto por un exit
		return FIN_ARCHIVO;
	}

	t_link_element *element = operaciones->head;
	t_operacion * op = element->data;
		enviar_operacion_a_coordinador(op);

	Message *rta = calloc (1, sizeof(Message));
	if(await_msg(cliente_coordinador, rta) < 0){
		log_error(log_esi, "error al recibir resultado");
		exit(ERROR_DE_RECEPCION);
	}

	int resultado;
	resultado = desempaquetar_resultado(rta);
	if(resultado != OK){
		loguear_resultado(log_esi, resultado);
		return resultado;
	}
	loguear_resultado(log_esi, resultado);
	free_msg(&rta);
	list_remove(operaciones, 0);
	return OK;
}

void manejar_mensajes(Message mensaje) {
	//loguear_recepcion(log_esi, &mensaje, nombres_modulos[mensaje.header->remitente]);
	int resultado;
	switch (mensaje.header->tipo_mensaje) {
		case CONEXION:
			log_info(log_esi, "Recibido el contenido del script");
			resultado = armar_estructura_script(mensaje.contenido);
			Message confirmacion;
			await_msg(socket_planificador, &confirmacion);
			break;
		case DESCONEXION:
			esi_correr = false;
			break;
		case EJECUTAR:
			log_info(log_esi, "Recibida instruccion de ejecutar");
			resultado = ejecutar_proxima_operacion();
			break;
		case RESULTADO:
			mensaje.header->remitente = ESI;
			resultado = enviar_y_loguear_mensaje(socket_planificador, mensaje, "planificador/0");
			break;
		default:
			log_error(log_esi, "se recibio un mensaje de tipo inesperado");
		}
		Message * m;
		empaquetar_resultado(ESI, resultado, &m);
		resultado = enviar_y_loguear_mensaje(socket_planificador, *m, "planificador\0");
		free_msg(&m);
}

int armar_estructura_script(char* contenidoScript) {
	script.script_contenido = malloc(strlen(contenidoScript));
	memcpy(script.script_contenido,"",1);
	strcat(script.script_contenido,contenidoScript);
	//memcpy(script.script_contenido,contenidoScript,strlen(contenidoScript));
	((char*) script.script_contenido)[strlen(script.script_contenido)] = '\0';
	script.script_largo = cantidad_lineas_script(script.script_contenido);
	char** split = string_n_split(script.script_contenido, script.script_largo,(char*) "\n");
	for(int i = 0; split[i] != NULL; i++){
		t_esi_operacion operacionOriginal = parse(split[i]);
		if (!operacionOriginal.valido) {
			//return CLAVE_MUY_GRANDE;
			return OK;
		}
		t_operacion * op = convertir_operacion(operacionOriginal);
		list_add(operaciones, op);
	} return OK;
}

esi_configuracion* inicializar_configuracion(char* argv[]) {
	// Levantamos configuracion
	esi_configuracion* pConfig = (esi_configuracion*) malloc(
			sizeof(esi_configuracion));

	t_config* config = config_create("./configESI.txt");
	if (config == NULL) {
		config = config_create("../configESI.txt");
	}
	if (argv == NULL || argv[1] == NULL) {
		argv[1] = nombre_script;
	}
	path_script = malloc(strlen(argv[1]) + 1);
	memcpy(path_script, argv[1], strlen(argv[1]));
	((char*) path_script)[strlen(argv[1])] = '\0';

	pConfig->coordinador_ip = calloc(strlen(leer_propiedad_string(config, "IP_coordinador"))+1, 1);
	memcpy(pConfig->coordinador_ip,
			leer_propiedad_string(config, "IP_coordinador"),
			strlen(leer_propiedad_string(config, "IP_coordinador")));

	pConfig->coordinador_puerto = calloc(strlen(leer_propiedad_string(config,"puerto_coordinador"))+1, 1);
	memcpy(pConfig->coordinador_puerto,
			leer_propiedad_string(config,"puerto_coordinador"),
			strlen(leer_propiedad_string(config,"puerto_coordinador")));

	pConfig->planificador_ip = calloc(strlen(leer_propiedad_string(config, "IP_planificador"))+1, 1);
	memcpy(pConfig->planificador_ip,
			leer_propiedad_string(config, "IP_planificador"),
			strlen(leer_propiedad_string(config, "IP_planificador")));

	pConfig->planificador_puerto = calloc(strlen(leer_propiedad_string(config,"puerto_planificador"))+1, 1);
	memcpy(pConfig->planificador_puerto,
			leer_propiedad_string(config,"puerto_planificador"),
			strlen(leer_propiedad_string(config,"puerto_planificador")));

	// Levantamos el archivo de log y guardamos IP y Puerto
	log_esi = log_create("log_esi.log", "ESI", true, LOG_LEVEL_TRACE);
	log_trace(log_esi, "Inicia el proceso ESI");
	log_trace(log_esi, "El puerto del coordinador es: %s",
			pConfig->coordinador_puerto);
	log_trace(log_esi, "La ip del coordinador es: %s", pConfig->coordinador_ip);
	log_trace(log_esi, "El puerto del planificador es: %s",
			pConfig->planificador_puerto);
	log_trace(log_esi, "La ip del planificador es: %s",
			pConfig->planificador_ip);
	log_trace(log_esi, "El nombre del script es: %s", argv[1]);
	operaciones = list_create();
	config_destroy(config);
	return pConfig;
}
