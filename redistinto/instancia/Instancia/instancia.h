#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/log.h>

#include "../../socket/operacion.h"
#include "configuracionInstancia.h"
#include "handlerCoordinador.h"
#include <commons/txt.h>

#define OK 0

t_log * log_inst;

typedef struct {
	int identificador;
	char* ip_coordinador;
	char* puerto_coord;
	int socket_coordinador;
	char* nombre_inst;
	int int_dump;
	char* path;
	t_list* tabla_entradas;
} t_instancia;
t_instancia instancia;

typedef struct {
	int largo_clave;
	int largo_valor;
	char* clave;
	char* valor;
} t_clave_valor;


#endif /* INSTANCIA_INSTANCIA_H_ */
