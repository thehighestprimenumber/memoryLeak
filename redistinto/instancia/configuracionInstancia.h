#ifndef INSTANCIA_CONFIGURACIONINSTANCIA_H_
#define INSTANCIA_CONFIGURACIONINSTANCIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/config.h>
#include <commons/log.h>

int leer_propiedad_int(t_config* , char*);
char* leer_propiedad_string(t_config* , char*);

#endif
