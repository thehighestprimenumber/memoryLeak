#include <esi.h>

int main(int argc,char *argv[]) {

	// Levantamos configuracion
	char* ruta_configuracion = "/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/src/configESI.txt";
	esi_configuracion configuracion;
	configuracion = leer_configuracion_esi(ruta_configuracion);
	esi_configuracion* pConfig = &configuracion;

	// Levantamos el archivo de log y guardamos IP y Puerto
	log_esi = log_create("log_esi.log", "ESI", true, LOG_LEVEL_TRACE);
	log_trace(log_esi,"Inicia el proceso ESI");
	log_trace(log_esi,"El puerto del coordinador es: %s",configuracion.coordinador_puerto);
	log_trace(log_esi,"La ip del coordinador es: %s",configuracion.coordinador_ip);
	log_trace(log_esi,"El puerto del planificador es: %s",configuracion.coordinador_puerto);
	log_trace(log_esi,"La ip del planificador es: %s",configuracion.coordinador_ip);

	printf("Inicia el proceso ESI\n");
	printf("El puerto del coordinador es: %s\n",configuracion.coordinador_puerto);
	printf("La ip del coordinador es: %s\n",configuracion.coordinador_ip);
	printf("El puerto del plnificador es: %s\n",configuracion.planificador_puerto);
	printf("La ip del planificador es: %s\n",configuracion.planificador_ip);

	// Nos conectamos y pedimos handshake al planificador, este nos asigna un identificador
	conectar_a_planificador(pConfig);

	return EXIT_SUCCESS;
}

// Funciones de inicialización

esi_configuracion leer_configuracion_esi(char *ruta_config) {

	t_config* config = config_create(ruta_config);
	esi_configuracion datosParaRetornar;
	esi_configuracion* datos = &datosParaRetornar;

	datos->coordinador_puerto = config_get_string_value(config, "puerto_coordinador");
	datos->coordinador_ip = config_get_string_value(config,"IP_coordinador");
	datos->planificador_puerto = config_get_string_value(config, "puerto_planificador");
	datos->planificador_ip = config_get_string_value(config,"IP_planificador");

	config_destroy(ruta_config);

	return datosParaRetornar;

}

void conectar_a_planificador(esi_configuracion* pConfig) {

	t_paquete paquete;

	int resultado = connect_to_server(pConfig->planificador_ip,pConfig->planificador_puerto);
	//Verifico conexion con el planificador
	if (resultado != 0) {
		log_error(log_esi, "Fallo conexion con el Planificador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Planificador");
	}

	pedir_handshake(&cliente_planificador, ESI);

	paquete = recibir_paquete(&cliente_planificador);

	memcpy(&identificador,(int*)paquete.pBuffer, sizeof(int));
	printf("El planificador asigno el id:%d a este ESI.\n", identificador);
	log_trace(log_esi,"EL planificador asigno el id:%d a este ESI.", identificador);
	destruir_paquete(paquete);
}
