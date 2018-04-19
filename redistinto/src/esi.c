#include "esi.h"

void configure_logger() {
	log_esi = log_create("/home/utnso/tp/ESI.log", "ESI: ", true, LOG_LEVEL_INFO);
}

void cargar_esi(){

	t_config* configuracion;
	configuracion = config_create("src/configESI.txt");

	proceso_esi esi;

	if (config_has_property(configuracion, puerto_planificador)) {
		esi.puerto_planif = config_get_int_value(configuracion,puerto_planificador);
	}

	if (config_has_property(configuracion, IP_planificador)) {
		esi.ip_planif = config_get_string_value(configuracion,IP_planificador);
	}

	if (config_has_property(configuracion, puerto_coordinador)) {
		esi.puerto_coord = config_get_int_value(configuracion,puerto_coordinador);
	}

	if (config_has_property(configuracion, IP_coordinador)) {
		esi.ip_coord = config_get_string_value(configuracion,IP_coordinador);
	}

	config_destroy(configuracion);

}
