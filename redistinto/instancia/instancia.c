#include "instancia.h"

int main(int argc,char *argv[]) {

	int socket_coordinador = inicializar(argv);
	if (socket_coordinador < 0) return EXIT_FAILURE;
	semTabla = malloc(sizeof(sem_t));
	sem_init(semTabla, 0 , 1);
	pthread_t autoDumps;
	pthread_create(&autoDumps, NULL, dump_automatico, NULL);

	while (1) {
		Message *msg = malloc(sizeof(Message));
		msg->contenido = NULL;
		msg->header = NULL;
		log_debug(log_inst, "esperando mensaje");
		int resultado = await_msg(socket_coordinador, msg);
		log_debug(log_inst, "llego un mensaje. parseando...");
		if (resultado<0){
			log_debug(log_inst, "error de recepcion");
			return ERROR_DE_RECEPCION;
			//continue;
		}

		switch (msg->header->tipo_mensaje){
			case ACK:
				printf("ok");
				break;
			case OPERACION:
				manejar_operacion(msg);
				break;
			default: printf("%s: mensaje recibido: %s", instancia.nombre_inst, (char*) msg->contenido);
				break;

		}
		free_msg(msg);
	}
	free(semTabla);
	return EXIT_SUCCESS;
}

int inicializar(char* argv[]){
	t_config* config;
	log_inst = log_create("./log_de_instancia.log", "log_instancia", true, LOG_LEVEL_DEBUG);
	if (argv == NULL || argv[1] == NULL) {
			argv[1] = "configInstancia.txt";
	}
	config = config_create(argv[1]);
	instancia.nombre_inst = leer_propiedad_string(config, "nombre_instancia");
	log_debug(log_inst, "nombre_instancia: %s", instancia.nombre_inst);
	instancia.ip_coordinador = leer_propiedad_string(config, "IP_coordinador");
	log_debug(log_inst, "IP_coordinador: %s", instancia.ip_coordinador);
	instancia.puerto_coord = leer_propiedad_string(config, "puerto_coordinador");
	log_debug(log_inst, "puerto_coordinador: %s", instancia.puerto_coord);
	char *alg = calloc(1, 5);
	alg = leer_propiedad_string(config, "algoritmo");
	if(strcmp(alg, "LRU")==0){
		instancia.algorimoActual = LRU;
		log_debug(log_inst, "Algoritmo: LRU");
	}
	else if(strcmp(alg, "BSU")==0){
		instancia.algorimoActual = BSU;
		log_debug(log_inst, "Algoritmo: BSU");
	}
	else {
		instancia.algorimoActual = CIRC;
		log_debug(log_inst, "Algoritmo: CIRC");
		inicializar_circular();
	}
	instancia.path = leer_propiedad_string(config, "punto_montaje");//"/home/utnso/instancia1/\0";
	log_debug(log_inst, "punto_montaje: %s", instancia.path);
	char *dir = calloc(1, strlen(instancia.path));
	memcpy(dir, instancia.path, strlen(instancia.path)-1);
	if (stat(dir, &pepito) == -1) {
	    mkdir(dir, 0777);
	}
	free(dir);
	instancia.int_dump = leer_propiedad_int(config, "dump");
	log_debug(log_inst, "intervalo_dump: %d", instancia.int_dump);
	instancia.tabla_entradas = list_create();
	instancia.socket_coordinador = conectar_a_coordinador(instancia.ip_coordinador, instancia.puerto_coord);
	if(instancia.socket_coordinador == -1){
	 log_debug(log_inst, "No se pudo conectar con el coordinador");
	 return -10;
	}
	log_debug(log_inst, "Se pudo conectar con el coordinador");
	Message* msg = empaquetar_conexion(INSTANCIA, instancia.nombre_inst);

	if (send_msg(instancia.socket_coordinador, (*msg))<0)
		return ERROR_DE_ENVIO;
	log_debug(log_inst, "Se notifico al coordinador de nuestro nombre");
	free_msg(&msg);

	if(!recibir_config_storage()) return -10;
	configurar_storage();

	return instancia.socket_coordinador;
}

int manejar_operacion(Message * msg) {
	t_operacion * operacion = desempaquetar_operacion(msg);
	int resultado;
	sem_wait(semTabla);
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
	sem_post(semTabla);
	Message* m_resultado= empaquetar_resultado(INSTANCIA, resultado);
	if (send_msg(instancia.socket_coordinador, *m_resultado)<0)
		return ERROR_DE_ENVIO;
	log_debug(log_inst, "Se envio el resultado de la operacion");
	free_msg(&m_resultado);
	free_operacion(&operacion);
	return OK;
}

int agregar_clave_a_lista(char* clave, int largo_clave){
	t_clave_valor* clave_valor_existente = buscar_clave_valor (clave);
	if (clave_valor_existente != NULL) return OK;

	t_clave_valor *clave_valor = malloc(sizeof(t_clave_valor));
	clave_valor->largo_clave=largo_clave;
	clave_valor->largo_valor=0;
	clave_valor->clave = malloc(largo_clave);
	clave_valor->nroEntrada = -1;
	memcpy(clave_valor->clave, clave, largo_clave);
	list_add(instancia.tabla_entradas, clave_valor);
	log_debug(log_inst, "GET %s", clave_valor->clave);
	return OK;
}

int asignar_valor_a_clave(char* clave, int largo_clave, char* valor, int largo_valor){
	t_clave_valor* entrada = buscar_clave_valor (clave);
	if (entrada == NULL)
			return CLAVE_INEXISTENTE;

	entrada->largo_valor = largo_valor;

	switch(instancia.algorimoActual){
		case CIRC:
			log_debug(log_inst, "SET %s", entrada->clave);
			if(guardar_circular(entrada, valor) < 0) return ERROR_VALOR_NULO;
			break;
		default:
			break;
	}
	return OK;
}

int guardar_entrada(char* clave, int largo_clave){
	t_clave_valor* clave_valor_existente = buscar_clave_valor(clave);
	if (clave_valor_existente == NULL)
		return CLAVE_INEXISTENTE;
	if (clave_valor_existente->largo_valor < 1)
		return VALOR_INEXISTENTE;

	log_debug(log_inst, "STORE %s", clave_valor_existente->clave);
	guardar(clave_valor_existente);
	return OK;
}

bool buscador(void *contenido){
	t_clave_valor *entrada = contenido;
	return strcmp(entrada->clave, claveEnBusqueda) == 0 ? true:false;
}

t_clave_valor* buscar_clave_valor (char* clave){
	claveEnBusqueda = clave;
	return list_find(instancia.tabla_entradas, buscador);
}

void dump(){
	list_iterate(instancia.tabla_entradas, guardar);
}

void guardar(void * contenido){
	t_clave_valor *entrada = contenido;
	char* ruta = calloc(1, strlen(instancia.path)+entrada->largo_clave+1);
	memcpy(ruta, instancia.path, strlen(instancia.path));
	memcpy(ruta+strlen(instancia.path), entrada->clave, entrada->largo_clave);
	log_debug(log_inst, "Guardando en %s", ruta);
	FILE* file = fopen(ruta, "w");
	char *ptrLectura = storage+entrada->nroEntrada*instancia.tamEntrada;
	fwrite(ptrLectura, sizeof(char), entrada->largo_valor, file);
	fclose(file);
	free(ruta);
}

void eliminar_entrada(void *contenido){
	t_clave_valor *entrada = contenido;
	free(entrada->clave);
	free(entrada);
}


void* dump_automatico(void *pepe){
	while(true){
		sleep(instancia.int_dump);
		log_debug(log_inst, "Autodump iniciado");
		sem_wait(semTabla);
		list_iterate(instancia.tabla_entradas, guardar);
		sem_post(semTabla);
		log_debug(log_inst, "Autodump finalizado");
	}
	return NULL;
}
