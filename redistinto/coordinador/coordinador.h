#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <commons/log.h>
#include <commons/collections/list.h>
#include "../src/esi.h"

typedef struct {
       //operacion* operacion, TODO crear struct
	   t_list* instancias;
	   int puerto_escucha;;
	   //algoritmo de distribucion
	   int cantidad_entradas;
       int tamanio_entrada;
	   int retardo;
} t_coordinador;

enum tipo_operacion {GET, SET, STORE} ;

typedef struct {
	enum tipo_operacion tipo;
	char* nombreRecurso;
	char* clave;

} operacion;

enum resultado_coord {OK, ERROR} ;

enum algoritmo_coordinador {EQUITATIVE_LOAD, LEAST_SPACE_USED, KEY_EXPLICIT} t_result_coord;

char* arch_config;
t_log * log_operaciones;

enum resultado_coord procesarOperacion(operacion* op_a_realizar, proceso_esi* solicitante);

enum resultado_coord abrirCoordinador(enum algoritmo_coordinador); //recibir datos de conexion al planificador

enum resultado_coord crearInstancia(); //recibir datos de instancia?

enum resultado_coord eliminarInstancia(); //recibir instancia

#endif
