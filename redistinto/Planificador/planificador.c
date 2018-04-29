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

	pidConsola = pthread_create(&threadConsola, NULL, (void*)&abrir_consola, (void*) "Inicio del hilo de la consola");

	if (pidConsola < 0) {
		log_error(log_planificador,"Error al intentar abrir la consola");
		exit_proceso(-1);
	}

	pthread_join(threadConsola,NULL);

	log_info(log_planificador,"\nProceso finalizado");
	list_destroy(planificador.clavesBloqueadas);

	exit_proceso(0);
}

void inicializar_logger() {
	log_planificador = log_create("/home/utnso/tp/Planificador.log", "Planificador: ", true, LOG_LEVEL_INFO);
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
	msg->contenido = (char*) malloc(strlen("Envio mensaje al Coordinador"));
	msg->contenido = "Envio mensaje al Coordinador";
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->id = PLANIFICADOR;
	msg->header->size = sizeof(msg);

	send_msg(pidCoordinador, (*msg));
		//printf("Se envio el mensaje");
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
