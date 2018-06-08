#include "handlerCoordinador.h"

int conectar_a_coordinador(char* ip, char* puerto){
	return connect_to_server(ip, puerto);
}

bool recibir_config_storage(){
	Message *msg = malloc(sizeof(msg));
	await_msg(instancia.socket_coordinador, msg);
	if(msg->header->tipo_mensaje != CONFSTORAGE) return false;
	ConfigStorage *cs = desempaquetar_config_storage(msg);
	instancia.cantEntradas = cs->cantEntradas;
	instancia.tamEntrada = cs->tamEntrada;
	free_msg(&msg);
	free(cs);
	return true;
}

void configurar_storage(){
	storage = malloc(instancia.tamEntrada*instancia.cantEntradas);
}
