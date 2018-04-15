#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct {
       //operacion* operacion, TODO crear struct
	   t_list* instancias;
	   int puerto_escucha;;
	   //algoritmo de distribucion
	   int cantidad_entradas;
       int tamanio_entrada;
	   int retardo;
} coordinador;

typedef enum {OK, ERROR} t_result_coord;

char* arch_config = ""; //TODO setear ruta del archivo de configuracion
t_log * log_operaciones;

//t_result_coord procesarOperacion(operacion* op);


#endif
