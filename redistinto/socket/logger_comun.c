#include "logger_comun.h"
#include "socket.h"

char* nombres_operacion[] = { "GET", "SET", "STORE" };

const char* nombres_resultados[] = {
								[-OK] = "OK" ,
								[-ERROR_DE_CONEXION] = "ERROR_DE_CONEXION" ,
								[-ERROR_DE_ENVIO] = "ERROR_DE_ENVIO" ,
								[-ERROR_DE_RECEPCION] = "ERROR_DE_RECEPCION" ,
								[-CLAVE_DUPLICADA] = "CLAVE_DUPLICADA" ,
								[-CLAVE_INEXISTENTE] = "CLAVE_INEXISTENTE" ,
								[-CLAVE_MUY_GRANDE] = "CLAVE_MUY_GRANDE" ,
								[-ERROR_ARCHIVO_NO_ENCONTRADO] = "ERROR_ARCHIVO_NO_ENCONTRADO" ,
								[-ERROR_VALOR_NULO] = "ERROR_VALOR_NULO" ,
								[-FIN_ARCHIVO] = "FIN_ARCHIVO" ,
								[-VALOR_INEXISTENTE] = "VALOR_INEXISTENTE" ,
								[-ERROR_COORDINADOR] = "ERROR_COORDINADOR" ,
								[-NO_HAY_INSTANCIAS] = "NO_HAY_INSTANCIAS" ,
								[-CLAVE_BLOQUEADA] = "CLAVE_BLOQUEADA" ,
							};

char* nombres_modulos[] = {"DESCONOCIDO", "ESI", "PLANIFICADOR", "INSTANCIA", "COORDINADOR"};

char* desempaquetar_varios(Message * m);

void loguear_conexion(t_log* log, Message * m, char* id_socket) {
	char* nombre = desempaquetar_conexion(m);
	log_info(log,
			"recibe conexion de %s", nombre);
	free (nombre);
}

void loguear_recepcion(t_log* log, Message * m, char* id_socket) {

	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log,
				"recibio un mensaje de %s para la operacion %s %s", id_socket,
				nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_info(log,
				"recibio un mensaje de %s: %s", id_socket,
				contenido);
		free(contenido);
	}
}

void loguear_desconexion(t_log* log, char* id_socket) {
	log_info(log, "RIP socket %s", id_socket);
}

void loguear_desconexion_int(t_log* log, int id_socket) {
	log_info(log, "RIP socket %d", id_socket);
}

void loguear_resultado(t_log* log, int resultado) {
	log_info(log, "resultado: %s", nombres_resultados[-resultado]);
}

void loguear_error_envio(t_log* log, Message * m, char* id_socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op = desempaquetar_operacion(m);
		log_info(log,
				"error al enviar el mensaje a %s para la operacion: %s %s",
				id_socket, nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char* contenido = desempaquetar_varios(m);
		log_warning(log, "error al enviar mensaje a %s: %s", id_socket,
				contenido);
		free(contenido);
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
		free(contenido);

	}
}
void loguear_operacion_no_soportada(t_log* log, Message * m, int id_socket) {
	char* contenido = desempaquetar_varios(m);
	log_info(log, "mensaje no soportado: %s", contenido); //TODO agregar id_socket
	free(contenido);
}

char* desempaquetar_varios(Message * m) {
	char* contenido;
	int r;
	switch (m->header->tipo_mensaje) {
	case TEXTO:
		contenido = desempaquetar_texto(m);
		break;
	case OPERACION:
		contenido = calloc(1, strlen("operacion")+1);
		strcpy(contenido, "operacion");
		break;
	case ACK:
		contenido = calloc(1, strlen("ACK")+1);
		strcpy(contenido, "ACK");
		break;
	case RESULTADO:
		r = desempaquetar_resultado(m);
		if (r<=0) {
			contenido = calloc(1, strlen(nombres_resultados[-r])+1);
			strcpy(contenido, nombres_resultados[-r]);
		} else {
			contenido = calloc(1,3);
			sprintf(contenido, "%d", r);
		}
		break;
	case CONEXION:
		contenido = calloc(1, strlen("conexion")+1);
		strcpy(contenido, "conexion");
		break;
	case DESCONEXION:
		contenido = calloc(1, strlen("desconexion")+1);
		strcpy(contenido, "desconexion");
		break;
	case CONFSTORAGE:
		contenido = calloc(1, strlen("storageConfig")+1);
		strcpy(contenido, "storageConfig");
		break;
	case EJECUTAR:
		contenido = calloc(1, strlen("ejecutar")+1);
		strcpy(contenido, "ejecutar");
		break;
	default:
		contenido = calloc(1, strlen("tipo no definido")+1);
		strcpy(contenido, "tipo no definido");
	}
	return contenido;
}
