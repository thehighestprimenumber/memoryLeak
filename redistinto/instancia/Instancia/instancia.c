#include "instancia.h"
#include "configuracionInstancia.h"
#include "handlerCoordinador.h"

int main(){
	t_config* config;
	log_inst = log_create("./log_de_instancia.log", "log_instancia", true, LOG_LEVEL_DEBUG);
	config = config_create("./configInstancia.txt");

	if (config == NULL) {
		config = config_create("../configInstancia.txt");
	}

	char* ip = leer_propiedad_string(config, "IP_coordinador");
	char* puerto = leer_propiedad_string(config, "puerto_coordinador");
	int socket_coordinador = conectar_a_coordinador(ip, puerto);
	if(socket_coordinador == -1){
	 log_debug(log_inst, "No se pudo conectar con el coordinador");
	 return -10;
	}
	log_debug(log_inst, "Se pudo conectar con el coordinador");
	Message* msg= (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen("Holaquetal coordinador") + 1);
	strcpy(msg->contenido,"Holaquetal coordinador");
	//msg->contenido = "Holaquetal coordinador";
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader));
	msg->header->remitente = INSTANCIA;
	msg->header->tipo_mensaje = TEST;
	msg->header->size = strlen(msg->contenido)+1;


	if (send_msg(socket_coordinador, (*msg))<0) return ERROR_DE_ENVIO;
	log_debug(log_inst, "Se envio el mensaje");
	while (1) {
		Message msg;
		log_debug(log_inst, "esperando mensaje");
		int resultado = await_msg(socket_coordinador, &msg);
		log_debug(log_inst, "llego un mensaje. parseando...");
		if (resultado<0){
			log_debug(log_inst, "error de recepcion");
			continue;
			//return ERROR_DE_RECEPCION;
		}
		//TODO parsear mensaje y hacer algo.
		char * request = malloc(msg.header->size);
		strncpy(request, (char *) msg.contenido, strlen(msg.contenido) + 1);
		//strcpy(request, (char *) msg.contenido);
		log_debug(log_inst, "mensaje recibido: %s", request); //FIXME aparecen caracteres de mas al final del mensaje ???
		//log_debug(log_inst, "%s", request);


	}
	return socket_OK;
}

