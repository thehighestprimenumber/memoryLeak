#include <planificador.h>

int main(void) {
	inicializar_logger();

	log_info(log_planificador, "Mensaje desde el planificador");

	estructura_planificador();

	//Pruebas datos planificador (falla todavía en algoritmo)
	printf("\nPuerto planificador: %d", planificador.puerto_planif);
	printf("\nAlgoritmo planificador: %s", planificador.algoritmo_planif);
	printf("\nEstimación: %d", planificador.estimacion_inicial);
	printf("\nIP Coordinador: %s", planificador.IP_coordinador);
	printf("\nPuerto Coordinador: %d",planificador.puerto_coordinador);

	list_destroy(planificador.clavesBloqueadas);
	exit_proceso(0);
}

void inicializar_logger() {
	log_planificador = log_create("/home/utnso/tp/Planificador.log", "Planificador: ", true, LOG_LEVEL_INFO);
}

void estructura_planificador() {
	t_config* configuracion;
	char** claves;

	planificador.clavesBloqueadas = list_create();

	configuracion = config_create("configPlanificador.txt");
	char* algoritmo = config_get_string_value(configuracion,algoritmo_planificador);
	char* IPcoord = config_get_string_value(configuracion,IPCoord_planificador);

	if (config_has_property(configuracion, puerto_planificador)) {
		planificador.puerto_planif = config_get_int_value(configuracion,puerto_planificador);
	}

	if (config_has_property(configuracion, algoritmo_planificador)) {
		planificador.algoritmo_planif = malloc(strlen(algoritmo) + 1);
		memcpy(planificador.algoritmo_planif,algoritmo,strlen(algoritmo));
	}

	if (config_has_property(configuracion, estimacion_planificador)) {
		planificador.estimacion_inicial = config_get_int_value(configuracion,estimacion_planificador);
	}

	if (config_has_property(configuracion, IPCoord_planificador)) {
		planificador.IP_coordinador = malloc(strlen(IPcoord));
		memcpy(planificador.IP_coordinador,IPcoord,strlen(IPcoord));
	}

	if (config_has_property(configuracion, puertoCoord_planificador)) {
		planificador.puerto_coordinador = config_get_int_value(configuracion,puertoCoord_planificador);
	}

	if (config_has_property(configuracion, claves_bloqueadas)) {
		claves = string_split(config_get_string_value(configuracion,claves_bloqueadas),",");
	}

	for (int i=0;i<sizeof(claves) - 1;i++) {
		list_add(planificador.clavesBloqueadas,claves[i]);
	}

	free(algoritmo);
	free(IPcoord);
	config_destroy(configuracion);
}

void exit_proceso(int retorno) {
  log_destroy(log_planificador);
  exit(retorno);
}
