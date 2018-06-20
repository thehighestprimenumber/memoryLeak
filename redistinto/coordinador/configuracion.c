# include "configuracion.h"
# include "coordinador.h"

char* ruta_arch_config = "./config_coordinador.txt";
char* ruta_arch_config_debug = "../config_coordinador.txt";

int leer_propiedad_int (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_int_value(configuracion,propiedad);
	}
	return 0;
}

void leer_propiedad_puerto_escucha (t_config *configuracion){
	if (config_has_property(configuracion, m_puerto_escucha)){
		coordinador.puerto_escucha = malloc(strlen(config_get_string_value(configuracion,m_puerto_escucha)) + 1);
		strcpy(coordinador.puerto_escucha, config_get_string_value(configuracion,m_puerto_escucha));
	}
}

void leer_propiedad_ip (t_config *configuracion){
	if (config_has_property(configuracion, m_ip_coordinador)){
		coordinador.ip_coordinador = malloc(strlen(config_get_string_value(configuracion,m_ip_coordinador)) + 1);
		strcpy(coordinador.ip_coordinador, config_get_string_value(configuracion,m_ip_coordinador));
	}
}


void leer_propiedad_algoritmo (t_config *configuracion){
	if (config_has_property(configuracion, m_algoritmo)){
		char* valor = config_get_string_value(configuracion,m_algoritmo);
		if (!strcmp(valor, "EL")) coordinador.algoritmo = EQUITATIVE_LOAD;
		else if (!strcmp(valor, "LSU")) coordinador.algoritmo = LEAST_SPACE_USED;
		else if (!strcmp(valor, "KE")) coordinador.algoritmo = KEY_EXPLICIT;
	}
}

void inicializar_configuracion(){
	log_coordinador = log_create("./log_de_operaciones.log", "log_operaciones", true, LOG_LEVEL_DEBUG);
	log_debug(log_coordinador, "cargando configuracion\n");
	leer_configuracion();

	log_debug(log_coordinador, "\nretardo: %d\ncantidad_entradas: %d\ntamanio_entrada: %d\npuerto_escucha: %s\nalgoritmo: %s\n",
			coordinador.retardo, coordinador.cantidad_entradas, coordinador.tamanio_entrada, coordinador.puerto_escucha, coordinador.algoritmo);

	coordinador.tabla_instancias = list_create();
	coordinador.conexiones = list_create();
	ultima_instancia_usada=0;
}

int leer_configuracion(char* ip, char* puerto){
	t_config* configuracion;
	configuracion = config_create(ruta_arch_config);

	if (configuracion == NULL)
		configuracion = config_create(ruta_arch_config_debug);

	coordinador.retardo = leer_propiedad_int(configuracion, m_retardo);
	coordinador.cantidad_entradas = leer_propiedad_int(configuracion, m_cantidad_entradas);
	coordinador.tamanio_entrada = leer_propiedad_int(configuracion, m_tamanio_entrada);

	leer_propiedad_ip(configuracion);
	leer_propiedad_puerto_escucha(configuracion);
	leer_propiedad_algoritmo(configuracion);
	config_destroy(configuracion);

	return 0;
}
