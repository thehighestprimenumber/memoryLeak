#include "instancia.h"


int inicializar();
int manejar_operacion(Message * msg);
t_clave_valor* buscar_clave_valor (char* clave);
int asignar_valor_a_clave(char* clave, int largo_clave, char* valor, int largo_valor);
int agregar_clave_a_lista(char* clave, int largo_clave);
void guardar(t_clave_valor * entrada);
void dump();

int main(){
	int socket_coordinador = inicializar();
	if (socket_coordinador<0)
		return socket_coordinador;
	while (1) {
		Message msg;
		log_debug(log_inst, "esperando mensaje");
		int resultado = await_msg(socket_coordinador, &msg);
		log_debug(log_inst, "llego un mensaje. parseando...");
		if (resultado<0){
			log_debug(log_inst, "error de recepcion");
			return ERROR_DE_RECEPCION;
			//continue;
		}
		switch (msg.header->tipo_mensaje){
		case ACK : printf("ok");
					break;
		case OPERACION:;
			int resultado = manejar_operacion(&msg);
			Message * m = empaquetar_resultado(INSTANCIA, resultado);
			if (send_msg(socket_coordinador, *m)<0)
				return ERROR_DE_ENVIO;
			log_debug(log_inst, "Se envio el mensaje");
			break;
		default: printf("%s: mensaje recibido: %s", instancia.nombre_inst, (char*) msg.contenido);
			break;
		}


	}
	return OK;
}

int inicializar(){
	t_config* config;
	log_inst = log_create("./log_de_instancia.log", "log_instancia", true, LOG_LEVEL_DEBUG);
	config = config_create("./configInstancia.txt");

	if (config == NULL) {
		config = config_create("../configInstancia.txt");
	}
	instancia.nombre_inst = leer_propiedad_string(config, "nombre");

	instancia.ip_coordinador = leer_propiedad_string(config, "IP_coordinador");
	instancia.puerto_coord= leer_propiedad_string(config, "puerto_coordinador");

	instancia.tabla_entradas = list_create();
	instancia.socket_coordinador = conectar_a_coordinador(instancia.ip_coordinador, instancia.puerto_coord);
	if(instancia.socket_coordinador == -1){
	 log_debug(log_inst, "No se pudo conectar con el coordinador");
	 return -10;
	}
	log_debug(log_inst, "Se pudo conectar con el coordinador");
	Message* msg= empaquetar_conexion(INSTANCIA, "id de instancia");
	if (send_msg(instancia.socket_coordinador, (*msg))<0)
		return ERROR_DE_ENVIO;
	log_debug(log_inst, "Se envio el mensaje");
	return instancia.socket_coordinador;
}

int manejar_operacion(Message * msg) {
	t_operacion * operacion = desempaquetar_operacion(msg);
	int resultado;
	switch (operacion->tipo) {
		case op_GET:
			resultado = agregar_clave_a_lista(operacion->clave, operacion->largo_clave);
			break;
		case op_SET:
			resultado = asignar_valor_a_clave(operacion->clave, operacion->largo_clave, operacion->valor, operacion->largo_valor);
			break;
		case op_STORE:
			resultado = guardar_entrada(operacion->clave, operacion->largo_clave);
			break;
	}
	Message* m_resultado= empaquetar_resultado(INSTANCIA, resultado);
		if (send_msg(instancia.socket_coordinador, *m_resultado)<0)
			return ERROR_DE_ENVIO;
		log_debug(log_inst, "Se envio el mensaje");

}

int agregar_clave_a_lista(char* clave, int largo_clave){
	t_clave_valor* clave_valor_existente = buscar_clave_valor (clave);
	if (clave_valor_existente != NULL)
		return CLAVE_DUPLICADA;

	t_clave_valor clave_valor = {.largo_clave = largo_clave, .largo_valor = 0};
	clave_valor.clave = calloc(1,largo_clave);
	memcpy(clave_valor.clave, clave, largo_clave);
	list_add(instancia.tabla_entradas, &clave_valor);
	return OK;
}

int asignar_valor_a_clave(char* clave, int largo_clave, char* valor, int largo_valor){
	t_clave_valor* clave_valor = buscar_clave_valor (clave);
	if (clave_valor == NULL)
			return CLAVE_INEXISTENTE;
	clave_valor->largo_valor = largo_valor;
	clave_valor->valor = calloc(1, largo_valor+1);
	memcpy(clave_valor->valor, valor, largo_valor+1);
	return OK;
}

int guardar_entrada(char* clave, int largo_clave){
	t_clave_valor* clave_valor_existente = buscar_clave_valor (clave);
	if (clave_valor_existente != NULL)
		return CLAVE_DUPLICADA;

	t_clave_valor clave_valor = {.largo_clave = largo_clave, .largo_valor = 0};
	clave_valor.clave = calloc(1,largo_clave);
	memcpy(clave_valor.clave, clave, largo_clave);
	list_add(instancia.tabla_entradas, &clave_valor);
	return OK;
}

t_clave_valor* buscar_clave_valor (char* clave){
	t_link_element *element = instancia.tabla_entradas->head;
	t_clave_valor *fila;
	while (element != NULL) {
		fila = (t_clave_valor*) (element->data);
		if (!strcmp(fila->clave, clave)) {
			free(element);
			return fila;
		}
		element = element->next;
	}
	free(element);
	return 0;
}

void dump(){
	t_link_element *element = instancia.tabla_entradas->head;
	t_clave_valor *fila;
	while (element != NULL) {
		fila = element->data;
		guardar(fila);
		element = element->next;
	}
	free(element);
}

void guardar(t_clave_valor * entrada){
	char* ruta = calloc(1, strlen(instancia.path)+entrada->largo_clave);
	memcpy(ruta, instancia.path, strlen(instancia.path));
	memcpy(ruta+strlen(instancia.path), entrada->clave, entrada->largo_clave);
	FILE* file = fopen(ruta, "w");
	fwrite(entrada->valor, entrada->largo_valor, 1, file);
}
