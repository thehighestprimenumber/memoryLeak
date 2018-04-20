#include "planificador.h"

int main(void) {
	inicializar_logger();

	estructura_planificador();

	//Pruebas datos planificador (falla todavía en algoritmo)
	printf("Puerto planificador: %d\n", planificador.puerto_planif);
	printf("Algoritmo planificador: %s\n", planificador.algoritmo_planif);
	printf("Estimación: %d\n", planificador.estimacion_inicial);
	printf("IP Coordinador: %s\n", planificador.IP_coordinador);
	printf("Puerto Coordinador: %d\n",planificador.puerto_coordinador);

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
		planificador.puerto_planif = config_get_int_value(configuracion,puerto_planificador);
	}
}

void algoritmo_read(t_config* configuracion) {
	char* algoritmo = config_get_string_value(configuracion,algoritmo_planificador);
	if (config_has_property(configuracion, algoritmo_planificador)) {
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
	char* IPcoord = config_get_string_value(configuracion,IPCoord_planificador);

	planificador.IP_coordinador = malloc(strlen(IPcoord) + 1);
	memcpy(planificador.IP_coordinador,IPcoord,strlen(IPcoord) + 1);
}

void puerto_coordinador_read(t_config* configuracion) {
	if (config_has_property(configuracion, puertoCoord_planificador)) {
		planificador.puerto_coordinador = config_get_int_value(configuracion,puertoCoord_planificador);
	}
}

void clavesBloqueadas_read(t_config* configuracion) {
	char** claves;

	if (config_has_property(configuracion, claves_bloqueadas)) {
		claves = string_split(config_get_string_value(configuracion,claves_bloqueadas),",");
	}

	for (int i=0;i<sizeof(claves) - 1;i++) {
		list_add(planificador.clavesBloqueadas,claves[i]);
	}

	free(claves);
}

void exit_proceso(int retorno) {
  log_destroy(log_planificador);
  exit(retorno);
}
