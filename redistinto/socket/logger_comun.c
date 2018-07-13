#include "logger_comun.h"
#include "socket.h"

char* nombres_operacion[] = { "GET", "SET", "STORE" };

const char* nombres_resultados[] = {
								[-OK] = "OK" ,
								[-ERROR_DE_CONEXION] = "ERROR DE CONEXION" ,
								[-ERROR_DE_ENVIO] = "ERROR DE ENVIO" ,
								[-ERROR_DE_RECEPCION] = "ERROR DE RECEPCION" ,
								[-CLAVE_DUPLICADA] = "CLAVE DUPLICADA" ,
								[-CLAVE_INEXISTENTE] = "CLAVE INEXISTENTE" ,
								[-CLAVE_MUY_GRANDE] = "CLAVE MUY GRANDE" ,
								[-ERROR_ARCHIVO_NO_ENCONTRADO] = "ERROR ARCHIVO NO ENCONTRADO" ,
								[-ERROR_VALOR_NULO] = "ERROR VALOR NULO" ,
								[-FIN_ARCHIVO] = "FIN ARCHIVO" ,
								[-VALOR_INEXISTENTE] = "VALOR INEXISTENTE" ,
								[-ERROR_COORDINADOR] = "ERROR COORDINADOR" ,
								[-NO_HAY_INSTANCIAS] = "NO HAY INSTANCIAS" ,
								[-CLAVE_BLOQUEADA] = "CLAVE BLOQUEADA" ,
							};

char* nombres_modulos[] = {"DESCONOCIDO", "ESI", "PLANIFICADOR", "INSTANCIA", "COORDINADOR", "CONSOLA"};



void loguear_conexion(t_log* log, Message * m, char* id_socket) {
	char* nombre;
	desempaquetar_conexion(m, &nombre);
	log_info(log,
			"recibe conexion de %s", nombre);
	free(nombre);
}

void loguear_recepcion_remitente(t_log* log, Message * m, int tipoRemitente) {

	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op;
		desempaquetar_operacion(m, &op);
		log_info(log,
				"recibio un mensaje de %s para la operacion %s %s", nombres_modulos[tipoRemitente],
				nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char * contenido;
		desempaquetar_varios(m, &contenido);
		log_info(log,
				"recibio un mensaje de %s: %s", nombres_modulos[tipoRemitente],
				contenido);
		free(contenido);
	}
}

void loguear_recepcion(t_log* log, Message * m, char* id_socket) {

	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op;
		desempaquetar_operacion(m, &op);
		log_info(log,
				"recibio un mensaje de %s para la operacion %s %s", id_socket,
				nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char * contenido;
		desempaquetar_varios(m, &contenido);
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
		t_operacion * op;
		desempaquetar_operacion(m, &op);
		log_info(log,
				"error al enviar el mensaje a %s para la operacion: %s %s",
				id_socket, nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char* contenido;
		desempaquetar_varios(m, &contenido);
		log_warning(log, "error al enviar mensaje a %s: %s", id_socket,
				contenido);
		free(contenido);
	}
}

void loguear_envio_OK(t_log* log, Message * m, char* id_socket) {
	if (m->header->tipo_mensaje == OPERACION) {
		t_operacion * op;
		desempaquetar_operacion(m, &op);
		log_info(log,
				"se envia el mensaje a %s para la operacion: %s %s",
				id_socket, nombres_operacion[op->tipo], op->clave);
		free_operacion(&op);
	} else {
		char* contenido;
		desempaquetar_varios(m, &contenido);
		log_info(log,
				"envio mensaje a %s: %s",
				id_socket, contenido);
		free(contenido);
		//free_msg(&contenido);
	}
}
void loguear_operacion_no_soportada(t_log* log, Message * m, int id_socket) {
	char* contenido;
	desempaquetar_varios(m, &contenido);
	log_info(log, "mensaje no soportado: %s", &contenido); //TODO agregar id_socket
	free(contenido);
}

void desempaquetar_varios(Message * m, char** output) {
	int r;
	char* contenido;
	switch (m->header->tipo_mensaje) {
	case TEXTO:
		//desempaquetar_texto(m, &contenido);
		contenido = calloc(1, strlen("texto")+1);
		memcpy(contenido, "texto\0", strlen("operacion"));
		*output = contenido;
		break;
	case OPERACION:
		contenido = calloc(1, strlen("operacion")+1);
		memcpy(contenido, "operacion\0", strlen("operacion"));
		*output = contenido;
		break;
	case ACK:
		contenido = calloc(1, strlen("ACK")+1);
		memcpy(contenido, "ACK\0", strlen("ACK"));
		*output = contenido;
		break;
	case RESULTADO:
		r = desempaquetar_resultado(m);
		if (r<=0) {
			contenido = calloc(1, strlen(nombres_resultados[-r])+1);
			memcpy(contenido, nombres_resultados[-r], strlen(nombres_resultados[-r]));
		} else {
			contenido = calloc(1,3);
			sprintf(contenido, "%d", r);
		}
		*output = contenido;
		break;
	case CONEXION:
		contenido = calloc(1, strlen("conexion")+1);
		memcpy(contenido, "conexion\0", strlen("conexion"));
		*output = contenido;
		break;
	case DESCONEXION:
		contenido = calloc(1, strlen("desconexion")+1);
		memcpy(contenido, "desconexion\0", strlen("desconexion"));
		*output = contenido;
		break;
	case CONFSTORAGE:
		contenido = calloc(1, strlen("storageConfig")+1);
		memcpy(contenido, "storageConfig\0", strlen("storageConfig"));
		*output = contenido;
		break;
	case EJECUTAR:
		contenido = calloc(1, strlen("ejecutar")+1);
		memcpy(contenido, "ejecutar\0", strlen("ejecutar"));
		*output = contenido;
		break;
	case STATUS_CLAVE:
		contenido = calloc(1, strlen("status")+1);
		strcpy(contenido, "status");
		*output = contenido;
		break;
	default:
		contenido = calloc(1, strlen("tipo no definido")+1);
		memcpy(contenido, "tipo no definido\0", strlen("tipo definido"));
		*output = contenido;
	}
}
