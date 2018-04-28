# include "coordinador.h"
# include "configuracion.h"

extern int iniciar();


//mensajes a enviar al planificador
bool esta_bloqueada(char* clave){
	return false;
}
bool bloquear_clave(char* clave){
	return OK;
}

bool informar_resultado_al_planificador(enum resultado_coord resultado){return true;}

//mensajes a enviar a una instancia
int enviar_solicitud_a_instancia(t_operacion operacion, mock_instancia * instancia){
	printf("enviada solicitud a instancia#%d", instancia->id);
	return OK;
}

int main() {
	logger_coordinador = log_create("./log_de_operaciones.log", "log_operaciones", true, LOG_LEVEL_DEBUG);
	log_debug(logger_coordinador, "cargando configuracion\n");
	cargar_configuracion();
	log_debug(logger_coordinador, "\nretardo: %d\ncantidad_entradas: %d\ntamanio_entrada: %d\npuerto_escucha: %s\nalgoritmo: %s\n",
			coordinador.retardo, coordinador.cantidad_entradas, coordinador.tamanio_entrada, coordinador.puerto_escucha, algoritmo_coordinador_nombres[coordinador.algoritmo]);
	t_operacion op = { .clave = "k1054", .valor = "54 alumnos", .tipo = 1 };
	proceso_esi esi = { .identificador = 1 };
	coordinador.tabla_instancias = list_create();
	crear_instancia(1);
	crear_instancia(2);
	crear_instancia(3);
	ultima_instancia_usada=0;
	enum resultado_coord resultado = procesarOperacion(op, esi);
	resultado = procesarOperacion(op, esi);
	informar_resultado_al_planificador(resultado);
	iniciar();
}

enum resultado_coord procesarOperacion(t_operacion op_a_realizar, proceso_esi solicitante) {
	char* clave = op_a_realizar.clave;
	log_info(logger_coordinador, "procesando solicitud: %s %s %s de ESI# %d", tipo_operacion_nombres[op_a_realizar.tipo], clave,
			op_a_realizar.valor, solicitante.identificador);
	if (op_a_realizar.tipo == GET) {
		bloquear_clave(clave);
		log_info(logger_coordinador, "bloquea clave: %s", clave);
		return OK;
	}
	if (esta_bloqueada(clave)) {
		return CLAVE_BLOQUEADA;
	} else {
		mock_instancia* instancia = seleccionar_instancia(clave);
		log_debug(logger_coordinador, "usando instancia#%d", instancia->id);
		if (instancia == NULL) {
			return NO_HAY_INSTANCIAS;
		}
		bloquear_clave(clave);
		int resultado = enviar_solicitud_a_instancia(op_a_realizar, instancia);
		log_info(logger_coordinador, "resultado: %d", resultado);
		if (resultado == 0)
			return OK;
		else
			return ERROR;
	}
}
