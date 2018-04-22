#define m_puerto_escucha "puerto"
#define m_algoritmo "algoritmo"
#define m_cantidad_entradas "cantidad_entradas"
#define m_tamanio_entrada "tamanio_entrada"
#define m_retardo "retardo"

# include "coordinador.h"

char* ruta_arch_config = "./config_coordinador.txt";

int leer_propiedad_int (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_int_value(configuracion,propiedad);
	}
	return 0;
}

void leer_propiedad_puerto_escucha (t_config *configuracion){
	if (config_has_property(configuracion, m_puerto_escucha)){
		coordinador.puerto_escucha = malloc(sizeof(config_get_string_value(configuracion,m_puerto_escucha)));
		strcpy(coordinador.puerto_escucha, config_get_string_value(configuracion,m_puerto_escucha));
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

int cargar_configuracion(){
	t_config* configuracion;
	configuracion = config_create(ruta_arch_config);
	coordinador.retardo = leer_propiedad_int(configuracion, m_retardo);
	coordinador.cantidad_entradas = leer_propiedad_int(configuracion, m_cantidad_entradas);
	coordinador.tamanio_entrada = leer_propiedad_int(configuracion, m_tamanio_entrada);
	leer_propiedad_puerto_escucha(configuracion);
	leer_propiedad_algoritmo(configuracion);
	config_destroy(configuracion);

	return 0;
}
