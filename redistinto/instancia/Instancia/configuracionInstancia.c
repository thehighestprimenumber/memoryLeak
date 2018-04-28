#include "configuracionInstancia.h"

int leer_propiedad_int (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_int_value(configuracion,propiedad);
	}
	return 0;
}

char* leer_propiedad_string (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_string_value(configuracion,propiedad);
	}
	return "";
}
