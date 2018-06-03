#include "logger_coordinador.h"

char* nombres_operacion[] = { "GET", "SET", "STORE" };
char* nombres_resultados[] = { "ERROR_COORDINADOR" - 20, "NO_HAY_INSTANCIAS"
		- 21, "CLAVE_BLOQUEADA" - 22 };

char* desempaquetar_varios(Message * m);

void loguear_conexion(int socket) {
	log_info(log_coordinador,
			"se recibe conexion de nueva instancia, socket %d", socket);
}

void loguear_recepcion(Message * m, int socket) {

	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log_coordinador,
				"se recibe mensaje de %d para la operacion %s %s %s", socket,
				nombres_operacion[op->tipo], op->clave, op->valor);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log_coordinador,
				"se recibio un mensaje al coordinador de %d: %s", socket,
				contenido);
	}
}

void loguear_desconexion(int socket) {
	log_info(log_coordinador, "RIP socket %d", socket);
}

void loguear_resultado(int resultado) {
	log_info(log_coordinador, "resultado: %s", nombres_resultados[resultado]);
}

void loguear_inst_op(char* nombre, t_operacion* op) {
	log_info(log_coordinador,
			"se selecciona la instancia %s para la operacion %s %s %s", nombre,
			nombres_operacion[op->tipo], op->clave, op->valor);
}

void loguear_error_envio(Message * m, int socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log_coordinador,
				"error al enviar el mensaje a %d para la operacion: %s %s %s",
				socket, nombres_operacion[op->tipo], op->clave, op->valor);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_warning(log_coordinador, "error al enviar mensaje a %d: %s", socket,
				contenido);
	}
}

void loguear_envio_OK(Message * m, int socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log_coordinador,
				"error al enviar el mensaje a %d para la operacion: %s %s %s",
				socket, nombres_operacion[op->tipo], op->clave, op->valor);
		free_operacion(&op);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log_coordinador,
				"se envio mensaje desde el coordinador mensaje a %d: %s",
				socket, contenido);

	}
}

char* desempaquetar_varios(Message * m) {
	char* contenido;
	switch (m->header->tipo_mensaje) {
	case TEXTO:
		contenido = desempaquetar_texto(m);
		break;
	case OPERACION:
		;
		t_operacion * op = desempaquetar_operacion(m);
		contenido = "operacion";
		break;
	case ACK:
		contenido = "ACK";
		break;
	case RESULTADO:
		;
		int r = desempaquetar_resultado(m);
		contenido = nombres_resultados[r];
		break;
	case CONEXION:
		contenido = "se recibe una conexion"; //TODO ampliar con nombre?
		break;
	default:
		contenido = " ";
	}
	//string_append(contenido, '\0');
	return contenido;
}
