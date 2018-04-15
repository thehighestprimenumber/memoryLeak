#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct {
       //operacion* operacion, TODO crear struct
	   t_list* instancias,
	   int puerto_escucha = 7777,
	   //algoritmo de distribucion

	   //TODO reemplazar por valores del archivo de configuracion
	   int cantidad_entradas = 20,
       int tamanio_entrada = 100,
	   int retardo = 300
} coordinador;

typedef enum {OK, ERROR} t_result_coord;

arch_config = ""; //TODO setear ruta del archivo de configuracion
t_log * log_operaciones;

//t_result_coord procesarOperacion(operacion* op);


#endif
