#include "logger_coordinador.h"

char* nombres_algoritmos_coordinador[] = {"EQUITATIVE LOAD", "LEAST SPACE USED"};

char* buscar_id_conexion (int socket){
	t_link_element *element = coordinador.conexiones->head;
	t_socket_nombre *dato_conexion;
		while (element != NULL) {
			dato_conexion = (t_socket_nombre*) (element->data);
			if ((dato_conexion->socket == socket)) {
				return dato_conexion->nombre;
			}
			element = element->next;
		}
	return "desconocido";
}

void registrar_conexion(Message * m, int socket) {
	t_socket_nombre* dato_conexion = calloc(1, sizeof(t_socket_nombre));
		char* nombre;
		desempaquetar_conexion(m, &nombre);
		dato_conexion->socket = socket;
		dato_conexion->nombre = calloc(1, strlen(nombre)+1);
		strcpy(dato_conexion->nombre, nombre);
		list_add(coordinador.conexiones, dato_conexion);
		loguear_conexion(log_coordinador, m, dato_conexion->nombre);
		free(nombre);
}


void loguear_inst_op(t_log* log, char* nombre, t_operacion* op) {
	log_info(log,
			"se selecciona la instancia '%s' para la operacion %s %s", nombre,
			nombres_operacion[op->tipo], op->clave);
}

void loguear_no_hay_instancia(t_log* log, t_operacion* op) {
	log_info(log,
			"no hay instancia para la operacion %s %s",
			nombres_operacion[op->tipo], op->clave);
}
