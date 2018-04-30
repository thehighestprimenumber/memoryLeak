#include "esi.h"

int main(int argc,char *argv[]) {

	// Levantamos configuracion
	//char* ruta_configuracion = "/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/src/configESI.txt";
	char* ruta_configuracion = "./configESI.txt";
	char* ruta_configuracion_debug = "../configESI.txt";

	esi_configuracion configuracion = leer_configuracion_esi(ruta_configuracion,ruta_configuracion_debug);
	esi_configuracion* pConfig = (esi_configuracion*)&configuracion;

	// Levantamos el archivo de log y guardamos IP y Puerto
	log_esi = log_create("log_esi.log", "ESI", true, LOG_LEVEL_TRACE);
	log_trace(log_esi,"Inicia el proceso ESI");
	log_trace(log_esi,"El puerto del coordinador es: %s",pConfig->coordinador_puerto);
	log_trace(log_esi,"La ip del coordinador es: %s",pConfig->coordinador_ip);
	log_trace(log_esi,"El puerto del planificador es: %s",pConfig->planificador_puerto);
	log_trace(log_esi,"La ip del planificador es: %s",pConfig->coordinador_ip);

	log_info(log_esi,"Inicia el proceso ESI\n");
	log_info(log_esi,"El puerto del coordinador es: %s\n",pConfig->coordinador_puerto);
	log_info(log_esi,"La ip del coordinador es: %s\n",pConfig->coordinador_ip);
	log_info(log_esi,"El puerto del plnificador es: %s\n",pConfig->planificador_puerto);
	log_info(log_esi,"La ip del planificador es: %s\n",pConfig->planificador_ip);

	// Nos conectamos y pedimos handshake al planificador, este nos asigna un identificador
	conectar_a_planificador(pConfig);

	// Nos conectamos y pedimos handshake al coordinador
	//conectar_a_coordinador(pConfig);

	return EXIT_SUCCESS;
}

// Funciones de inicialización

esi_configuracion leer_configuracion_esi(char *ruta_config, char* ruta_config_debug) {

	t_config* config = config_create(ruta_config);
	//esi_configuracion datos = (esi_configuracion)malloc(sizeof(esi_configuracion));
	esi_configuracion datos;

	//datos.coordinador_puerto = config_get_string_value(config, "puerto_coordinador");
	//datos.coordinador_ip = config_get_string_value(config,"IP_coordinador");
	//datos.planificador_puerto = config_get_string_value(config, "puerto_planificador");
	//datos.planificador_ip = config_get_string_value(config,"IP_planificador");

	if (config == NULL)
		config = config_create(ruta_config_debug);

	if (config_has_property(config, puerto_coordinador)) {
		char* puertoCoord= config_get_string_value(config,puerto_coordinador);
		datos.coordinador_puerto = malloc(strlen(puertoCoord) + 1);
		memcpy(datos.coordinador_puerto,puertoCoord,strlen(puertoCoord) + 1);
	}

	if (config_has_property(config, IP_coordinador)) {
		char* ipCoord = config_get_string_value(config,IP_coordinador);
		datos.coordinador_ip = malloc(strlen(ipCoord) + 1);
		memcpy(datos.coordinador_ip,ipCoord,strlen(ipCoord) + 1);
		}

	if (config_has_property(config, puerto_planificador)) {
		char* puertoPlanif = config_get_string_value(config,puerto_planificador);
		datos.planificador_puerto = malloc(strlen(puertoPlanif) + 1);
		memcpy(datos.planificador_puerto,puertoPlanif,strlen(puertoPlanif) + 1);
	}

	if (config_has_property(config, IP_planificador)) {
		char* ipPlanif = config_get_string_value(config,IP_planificador);
		datos.planificador_ip = malloc(strlen(ipPlanif) + 1);
		memcpy(datos.planificador_ip,ipPlanif,strlen(ipPlanif) + 1);
	}

	config_destroy(config);

	return datos;

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

	//printf("Se pudo conectar con el coordinador");
	Message* msg= (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen("Envio mensaje al Planificador desde ESI"));
	msg->contenido = "Envio mensaje al Planificador desde ESI";
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->id = ESI;
	msg->header->size = strlen(msg->contenido);

	if (send_msg(socket_planificador, (*msg))<0) return ERROR_DE_ENVIO;
	log_debug(log_esi, "Se envio el mensaje");
	while (1) {
		Message msg;
		log_debug(log_esi, "esperando mensaje");
		int resultado = await_msg(socket_planificador, &msg);
		log_debug(log_esi, "llego un mensaje. parseando...");
		if (resultado<0){
			log_debug(log_esi, "error de recepcion");
			continue;
			//return ERROR_DE_RECEPCION;
		}
		//TODO parsear mensaje y hacer algo.
		char * request = malloc(msg.header->size);
		strcpy(request, (char *) msg.contenido);
		log_debug(log_esi, "mensaje recibido: %s", request); //FIXME aparecen caracteres de mas al final del mensaje ???
		//log_debug(log_inst, "%s", request);
		break;
	}
}

void conectar_a_coordinador(esi_configuracion* pConfig) {

	t_paquete paquete;

	int resultado = connect_to_server(pConfig->coordinador_ip,pConfig->coordinador_puerto);
	//Verifico conexion con el coordinador
	if (resultado != 0) {
		log_error(log_esi, "Fallo conexion con el Coordinador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Coordinador");
	}

	pedir_handshake(&cliente_coordinador, ESI);

	paquete = recibir_paquete(&cliente_coordinador);
	memcpy(&identificador,(int*)paquete.pBuffer, sizeof(int));

	destruir_paquete(paquete);
}
