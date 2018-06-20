#include "logger_comun.h"

char* nombres_operacion[] = { "GET", "SET", "STORE" };
char* nombres_resultados[] = {	"OK" 	,
								"ERROR DE ENVIO" 		-11,
								"ERROR DE RECEPCION" 	-12,
								"CLAVE DUPLICADA" 		-13,
								"CLAVE INEXISTENTE" 	-14,
								"CLAVE MUY GRANDE" 		-15,
								"ERROR COORDINADOR" 	-20,
								"NO HAY INSTANCIAS"		-21,
								"CLAVE BLOQUEADA" 		-22,
								"ERROR DE CONEXION" 	-10,
							};
char* nombres_modulos[] = {"DESCONOCIDO", "ESI", "PLANIFICADOR", "INSTANCIA", "COORDINADOR"};

char* desempaquetar_varios(Message * m);

void loguear_conexion(t_log* log, Message * m, char* id_socket) {
	log_info(log,
			"recibe conexion de %s", desempaquetar_conexion(m));
}

void loguear_recepcion(t_log* log, Message * m, char* id_socket) {

	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log,
				"recibio un mensaje de %s para la operacion %s %s", id_socket,
				nombres_operacion[op->tipo], op->clave);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log,
				"recibio un mensaje de %s: %s", id_socket,
				contenido);
	}
}

void loguear_desconexion(t_log* log, char* id_socket) {
	log_info(log, "RIP socket %s", id_socket);
}

void loguear_desconexion_int(t_log* log, int id_socket) {
	log_info(log, "RIP socket %d", id_socket);
}

void loguear_resultado(t_log* log, int resultado) {
	log_info(log, "resultado: %s", nombres_resultados[resultado]);
}

void loguear_inst_op(t_log* log, char* nombre, t_operacion* op) {
	log_info(log,
			"se selecciona la instancia '%s' para la operacion %s %s", nombre,
			nombres_operacion[op->tipo], op->clave);
}

void loguear_error_envio(t_log* log, Message * m, char* id_socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log,
				"error al enviar el mensaje a %s para la operacion: %s %s",
				id_socket, nombres_operacion[op->tipo], op->clave);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_warning(log, "error al enviar mensaje a %s: %s", id_socket,
				contenido);
	}
}

void loguear_envio_OK(t_log* log, Message * m, char* id_socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log,
				"se envia el mensaje a %s para la operacion: %s %s",
				id_socket, nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log,
				"envio mensaje a %s: %s",
				id_socket, contenido);

	}
}

char* desempaquetar_varios(Message * m) {
	char* contenido;
	int r;
	switch (m->header->tipo_mensaje) {
	case TEXTO:
		contenido = desempaquetar_texto(m);
		break;
	case OPERACION:
		contenido = "operacion";
		break;
	case ACK:
		contenido = "ACK";
		break;
	case RESULTADO:
		r = desempaquetar_resultado(m);
		contenido = nombres_resultados[r];
		break;
	case CONEXION:
		contenido = "conexion";
		break;
	case DESCONEXION:
		contenido = "desconexion";
		break;
	case CONFSTORAGE:
		contenido = "storageConfig";
		break;
	case EJECUTAR:
		contenido = "ejecutar";
		break;
	default:
		contenido = "tipo no definido";
	}
	//string_append(contenido, '\0');
	return contenido;
}
