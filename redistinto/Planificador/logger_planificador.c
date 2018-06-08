#include "logger_planificador.h"

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

char* desempaquetar_varios(Message * m);

void loguear_conexion(int socket) {
	log_info(log_planificador,
			"el planificador recibe conexion de nueva esi, socket %d", socket);
}

void loguear_recepcion(Message * m, int socket) {

	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log_planificador,
				"el planificador recibió un mensaje de %d para la operacion %s %s %s", socket,
				nombres_operacion[op->tipo], op->clave, op->valor);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log_planificador,
				"el planificador recibio un mensaje de %d: %s", socket,
				contenido);
	}
}

void loguear_desconexion(int socket) {
	log_info(log_planificador, "RIP socket %d", socket);
}

void loguear_resultado(int resultado) {
	log_info(log_planificador, "resultado: %s", nombres_resultados[resultado]);
}

void loguear_error_envio(Message * m, int socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log_planificador,
				"error al enviar el mensaje a %d para la operacion: %s %s %s",
				socket, nombres_operacion[op->tipo], op->clave, op->valor);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_warning(log_planificador, "error al enviar mensaje a %d: %s", socket,
				contenido);
	}
}

void loguear_envio_OK(Message * m, int socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log_planificador,
				"Se envío ok el mensaje a %d para la operacion: %s %s %s",
				socket, nombres_operacion[op->tipo], op->clave, op->valor);
		free_operacion(&op);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log_planificador,
				"el planificador envio mensaje a %d: %s",
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
