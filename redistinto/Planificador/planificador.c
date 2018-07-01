#include "planificador.h"

int main(void) {
	inicializar_logger();

	log_info(log_consola, "\nCargando configuración");
	estructura_planificador();

	log_info(log_consola,"\nPuerto planificador: %s", planificador.puerto_planif);
	log_info(log_consola,"\nAlgoritmo planificador: %s", planificador.algoritmo_planif);
	log_info(log_consola,"\nEstimación: %d", planificador.estimacion_inicial);
	log_info(log_consola,"\nIP Coordinador: %s", planificador.IP_coordinador);
	log_info(log_consola,"\nPuerto Coordinador: %s",planificador.puerto_coordinador);
	log_info(log_consola,"\nAlfa Planificación: %d", planificador.alfaPlanificacion);

	//Crear listas de procesos
	cola_ready = list_create();
	cola_blocked = list_create();
	cola_esi_blocked = list_create();
	cola_finished = list_create();

	//conectar a coordinador
	t_planificador* pConfig = (t_planificador*)&planificador;
	pidCoordinador = conectar_a_coordinador(pConfig);

	log_info(log_consola,"\nInicio de la consola\n");

	//Abrir Consola
	if (pidConsola < 0) {
		log_error(log_consola,"Error al intentar abrir la consola");
		exit_proceso(-1);
	}

	flag_puede_ejecutar = true;
	flag_comando_pendiente = false;
	flag_instruccion = false;

	//Escuchar conexiones ESI
	iniciar();

	log_info(log_planificador,"\nProceso finalizado");
	list_destroy(planificador.clavesBloqueadas);

	exit_proceso(0);
}

int conectar_a_coordinador(t_planificador* pConfig) {
	int pidCoordinador = connect_to_server(pConfig->IP_coordinador,pConfig->puerto_coordinador);
	//Verifico conexion con el coordinador
	if (pidCoordinador < 0) {
		log_error(log_planificador, "Fallo conexión Planificador con el Coordinador");
		return -1;
	} else {
		log_info(log_planificador, "Planificador se conecto con el Coordinador");
	}

	Message * mensaje = empaquetar_conexion(PLANIFICADOR, "planificador\0");
	int resultado = enviar_y_loguear_mensaje(pidCoordinador, *mensaje, "coordinador\0");

	if (resultado < 0) {
		log_error(log_planificador, "Fallo envio mensaje conexión al Coordinador");
		return -1;
	}

	return pidCoordinador;
}

int enviar_y_loguear_mensaje(int socket, Message msg, char* destinatario) {
	int res = send_msg(socket, msg);
		if (res<0) {
			loguear_error_envio(log_planificador, &msg, destinatario);
			return ERROR_DE_ENVIO;
		}
	loguear_envio_OK(log_planificador, &msg, destinatario);
	return OK;
}

int iniciar(){
	log_info(log_consola, "Iniciando proceso planificador");
	char* ipLocal = get_local_ip();
	log_info(log_consola, "IP Local: %s", ipLocal);

	int socket_fd = create_listener(ipLocal,planificador.puerto_planif);
	if (socket_fd <0) return ERROR_DE_CONEXION;

	free(ipLocal);

	start_listening_select(socket_fd, pidCoordinador, *manejador_de_eventos);

	return 0;
}

void exit_proceso(int retorno) {
  log_destroy(log_planificador);
  log_destroy(log_consola);
  exit(retorno);
}

void liberar_split(char** array){
	int i = 0;
	while (array[i] != NULL) {
		free(array[i]);
		i++;
	}
	free(array);
}

char* armarPathScript(char* cadenaPath,char* nombreScript) {
	char* cadenaScript = malloc(strlen(cadenaPath) + strlen(nombreScript) + 1);
	memcpy(cadenaScript,cadenaPath,strlen(cadenaPath) + 1);
	strcat(cadenaScript,nombreScript);
	return cadenaScript;
}

void leer_script_completo(char* nombreArchivo) {
	//Abrir archivo para los esis
	FILE *f = fopen(armarPathScript(scriptTxtDebug,nombreArchivo), "rb");
		if (f == NULL) {
			f = fopen(armarPathScript(scriptTxt,nombreArchivo), "rb");
			if (f == NULL) {
				contenidoScript = malloc(strlen(" "));
				memcpy(contenidoScript," ",strlen(" "));

				return;
			}
		}

		fseek(f,0,SEEK_END);
		long fsize = ftell(f);
		fseek(f,0,SEEK_SET);

		contenidoScript = malloc(fsize + 1);
		if (contenidoScript == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		fread(contenidoScript, fsize, 1, f);
		fclose(f);
		contenidoScript[fsize] = '\0';
}

struct_pcb inicializar_pcb() {
	struct_pcb pcb;
	pcb.pid = 0;
	pcb.lineas_restantes = 0;
	pcb.tamanio_script = 0;
	return pcb;
}

int manejador_de_eventos(int socket, Message* msg){
	log_info(log_planificador, "Ocurrio un evento del tipo: %d", msg->header->tipo_mensaje);

	//En el caso de las operaciones el socket ya escucha el mensaje por lo que esto no va
	if (msg->header->remitente != CONSOLA && msg->header->tipo_mensaje != OPERACION && msg->header->tipo_mensaje != RESULTADO && msg->header->tipo_mensaje != DESCONEXION)
	{
		int res = await_msg(socket, msg);
		if (res<0) {
			log_info(log_planificador, "RIP socket %d", socket);
			close(socket);
			return ERROR_DE_RECEPCION;
		}
	}

	if(msg->header->remitente == CONSOLA){
		log_info(log_planificador, "Me hablo LA consola");

		leer_consola();
		nroComando = decodificar_comando();
		ejecutar_comando(nroComando);
	}
	else if(msg->header->remitente == ESI){
		log_info(log_planificador, "Me hablo una ESI");
		switch(msg->header->tipo_mensaje){
			case CONEXION:
				log_info(log_planificador, "Se conecto una ESI");
				aceptar_conexion(socket, msg->contenido);

				//Switcheamos por el algoritmo
				if (strcmp(planificador.algoritmo_planif,"FIFO") == 0)
				{
					manejar_nueva_esi_fifo(socket);
					return 0;
				}
				else if (strcmp(planificador.algoritmo_planif,"SJF-SD") == 0)
				{
					//manejar_nueva_esi_SJF_SD(socket);
					return 0;
				}
				else if (strcmp(planificador.algoritmo_planif,"SJF-CD") == 0)
				{

				}
				else if (strcmp(planificador.algoritmo_planif,"HRRN") == 0)
				{

				}
				else
				{
						return -2;
				}
				break;

			case DESCONEXION:
				if (strcmp(planificador.algoritmo_planif,"FIFO") == 0)
				{
					manejar_desconexion_esi_fifo(socket);
					return -18;
				}
				else if (strcmp(planificador.algoritmo_planif,"SJF-SD") == 0)
				{
					//manejar_desconexion_esi_SJF_SD(socket);
					return 0;
				}
				else if (strcmp(planificador.algoritmo_planif,"SJF-CD") == 0)
				{

				}
				else if (strcmp(planificador.algoritmo_planif,"HRRN") == 0)
				{

				}
				else
				{
					//fuck
					return -2;
				}
				break;

				case RESULTADO:
					buscar_y_correr_comando();
					return manejar_resultado(socket, msg);
					break;

			default:
				//fuck
				return 0;
		}//fin del switch
	}else if(msg->header->remitente == COORDINADOR){
		log_info(log_planificador, "Me hablo el Coordinador");
		switch(msg->header->tipo_mensaje){
			case OPERACION:
				log_info(log_planificador, "Me pidió validar un get o un set");

				sleep(1);

				//Aca diferenció que operación me pide verificar el coordinador
				//y actuo según el caso
				int resultado_operacion = manejar_operacion(socket,msg);

				Message* mensaje = empaquetar_resultado(PLANIFICADOR, resultado_operacion);
				int result = enviar_y_loguear_mensaje(socket, *mensaje, "coordinador\0");
				if (result) {
					log_info(log_planificador, "error al enviar resultado al coordinador");
					return ERROR_DE_ENVIO;
				}

				//Por ahora es texto, en un futuro alguna estructura mas compleja
				return resultado_operacion;
				break;
			default:
				//fuck
			return 0;
		}//fin del switch
	}else if(msg->header->remitente == DESCONOCIDO){
		log_info(log_planificador, "Ocurrio algo con alguien pero no se quien");
	}
	return 0;
	//Aca deberia haber algo para manejar otro tipo de eventos que no sean de la ESI

	return ERROR;
}

//Conexion
void aceptar_conexion(int socket, char* nombreScript) {
	//Por último envío mensaje de confirmación al esi que se conecto
	leer_script_completo(nombreScript);
	Message* mensaje = empaquetar_texto(contenidoScript, strlen(contenidoScript), PLANIFICADOR);
	mensaje->header->tipo_mensaje = CONEXION;
	enviar_y_loguear_mensaje(socket, *mensaje, "desconocido");
	free_msg(&mensaje);
}

//Manejo listas
void agregar_ready(struct_pcb pcb) {
	struct_ready* elemento = malloc(sizeof(struct_ready));
	elemento->pcb = pcb;
	list_add(cola_ready,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, a la lista de listos",pcb.pid);
}

void agregar_blocked(struct_pcb pcb, char* clave) {
	struct_blocked* elemento = malloc(sizeof(struct_blocked));
	elemento->pcb = pcb;
	elemento->clave = (char*)malloc(strlen(clave)+1);
	strcpy(elemento->clave,clave);
	list_add(cola_blocked,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, clave %s a la lista de bloqueados",pcb.pid,clave);
}

void agregar_esi_blocked(struct_pcb pcb, char* clave) {
	struct_blocked* elemento = malloc(sizeof(struct_blocked));
	elemento->pcb = pcb;
	elemento->clave = (char*)malloc(strlen(clave)+1);
	strcpy(elemento->clave,clave);
	list_add(cola_esi_blocked,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, clave %s a la lista de esis bloqueados por clave tomada anteriormente",pcb.pid,clave);
}

void agregar_finished(int idEsi) {
	struct_finished* elemento = malloc(sizeof(struct_finished));
	elemento->pid = idEsi;
	list_add(cola_finished,elemento);
	log_debug(log_planificador,"\nSe agrego el esi %d, a la lista de finalizados",idEsi);
}

void finalizar_esi(int socket_esi) {
	esi_a_eliminar = socket_esi;
	agregar_finished(socket_esi);
	list_remove_by_condition(cola_ready, (void*)buscar_esi_kill);
	list_remove_by_condition(cola_blocked, (void*)buscar_esi_kill);
	list_remove_by_condition(cola_esi_blocked, (void*)buscar_esi_kill);
	if (socket_esi == esiRunning.pid)
		esiRunning = inicializar_pcb();
}

struct_pcb planificar_esis() {
	esiRunning = inicializar_pcb();

	if (strcmp(planificador.algoritmo_planif,"FIFO") == 0) {
		struct_ready* primer_esi = seleccionar_esi_ready_fifo();
		if (primer_esi != NULL){
			//Obtengo el seleccionado para enviar un mensaje, y me lo guardo en variable
			//global como proceso que está ejecutando
			esiRunning = primer_esi->pcb;
		}
	}
	else if (strcmp(planificador.algoritmo_planif,"SJF-SD") == 0) {

	}

	else if (strcmp(planificador.algoritmo_planif,"SJF-CD") == 0) {

	}

	else if (strcmp(planificador.algoritmo_planif,"HRRN") == 0) {

	}

	else
	{
		log_debug(log_planificador,"El algoritmo de planificación %s es inválido",planificador.algoritmo_planif);
		exit_proceso(-1);
	}

	return esiRunning;
}

struct_ready* seleccionar_esi_ready_fifo() {
	struct_ready* esi_seleccionado;

	//Si hay esis esperando, selecciona al primero de la lista
	if (list_size(cola_ready) > 0) {
		esi_seleccionado = list_get(cola_ready, 0);
		list_remove(cola_ready, 0);
		return esi_seleccionado;
	}

	//Si no encuentro nada retorno null
	return NULL;
}



//Funciones dummy para que corra provisionalmente
int manejar_nueva_esi_fifo(int socket){

	//Agrego la nueva conexión a lista de preparados
	struct_pcb pcb = inicializar_pcb();
	pcb.pid = socket;
	agregar_ready(pcb);

	//Verifico si algún esi está corriendo, caso contrario
	//envio un esi a ejecutarse según el algorimto seleccionado
	if (flag_puede_ejecutar == true)
	{
		if (esiRunning.pid == 0) {
			ejecutar_nueva_esi();
		}
	}

	return OK;
}

void manejar_desconexion_esi_fifo(int socket){
	//Le confirmo al esi que se puede desconectar
	//envio_desconexion(socket);
	finalizar_esi(socket);

	log_info(log_planificador, "RIP socket %d", socket);

	if (esiRunning.pid == 0)
		ejecutar_nueva_esi();
}

int manejar_operacion(int socket,Message* msg) {
	operacionEnMemoria = desempaquetar_operacion(msg);

	if (operacionEnMemoria->largo_clave - 2 > 40)
	{
		free_operacion(&operacionEnMemoria);
		return CLAVE_MUY_GRANDE;
	}

	if (operacionEnMemoria->tipo == op_GET) {
		return validar_operacion_get();
	}
	else if (operacionEnMemoria->tipo == op_SET) {
		return validar_operacion_set();
	}
	else if (operacionEnMemoria->tipo == op_STORE) {
		return validar_operacion_store();
	}
	else
	{
		//esto no debería pasar nunca
		free_operacion(&operacionEnMemoria);
		exit_proceso(-1);
	}

	return OK;
}

int manejar_resultado(int socket,Message* msg) {
	int resultado = desempaquetar_resultado(msg);
	loguear_resultado(log_planificador, resultado);

	switch(resultado){
		case OK:
		//Envío mensaje para pedirle al esi que ejecute. El ESI es quien debería abrir su archivo
		//y comenzar a procesar instrucciones
		if (flag_puede_ejecutar == true && socket == esiRunning.pid)
			return envio_ejecutar(socket);
		else
			return 0;

		break;
		//En este caso como ya estaría bloqueado en mi lista queda esperando
		case CLAVE_DUPLICADA:
			return 0;
		//En estos casos envío mensaje al esi para que se desconecte saliendo del bucle
		case CLAVE_INEXISTENTE:
		case CLAVE_MUY_GRANDE:
		case ERROR_VALOR_NULO:
			return envio_desconexion(socket);
		default:
		//fuck
		return -2;
	}

}

int validar_operacion_get() {
	if (list_any_satisfy(planificador.clavesBloqueadas, ((void*) clave_ya_bloqueada_config)))
	{
		agregar_esi_blocked(esiRunning, operacionEnMemoria->clave);
		free_operacion(&operacionEnMemoria);
		return CLAVE_DUPLICADA;
	}
	else
	{
		if (list_any_satisfy(cola_blocked, (void*) clave_ya_bloqueada))
		{
			agregar_esi_blocked(esiRunning, operacionEnMemoria->clave);
			free_operacion(&operacionEnMemoria);
			return CLAVE_DUPLICADA;
		}
	}

	//Si la clave no está tomada, la agrego a lista de bloqueados
	agregar_blocked(esiRunning, operacionEnMemoria->clave);
	free_operacion(&operacionEnMemoria);
	return OK;
}

int validar_operacion_set() {
	if (!list_any_satisfy(cola_blocked, ((void*) clave_set_disponible)))
	{
		free_operacion(&operacionEnMemoria);
		return CLAVE_INEXISTENTE;
	}

	//Si puede tomar la clave sin problemas, simplemente retorna ok
	free_operacion(&operacionEnMemoria);
	return OK;
}

int validar_operacion_store() {
	bool claveExiste = false;

	if (list_any_satisfy(planificador.clavesBloqueadas, ((void*) clave_ya_bloqueada_config)))
	{
		claveExiste = true;
	}

	//Validar si la clave existe
	else if (list_any_satisfy(cola_blocked, ((void*) clave_set_disponible)))
	{
		claveExiste = true;
	}
	else
	{
		return CLAVE_INEXISTENTE;
	}

	if (claveExiste) {
		//Si está entre las claves bloqueadas por config lo libero
		list_remove_by_condition(planificador.clavesBloqueadas, ((void*) clave_ya_bloqueada_config));

		//quito la clave con el respectivo esi que la tenía bloqueada
		list_remove_by_condition(cola_blocked, ((void*) clave_set_disponible));

		//Desbloqueo al primer esi que tuviera la clave tomada
		desbloquear_esi();

		free_operacion(&operacionEnMemoria);
	}


	return OK;
}

void ejecutar_nueva_esi() {
	struct_pcb esi_seleccionado = planificar_esis();

	if (esi_seleccionado.pid > 0) {
	flag_instruccion = true;
	//Envío mensaje para pedirle al esi que ejecute. El ESI es quien debería abrir su archivo
	//y comenzar a procesar instrucciones
	Message* mensajeEjec = empaquetar_texto("Planificador pide ejecutar ESI", strlen("Planificador pide ejecutar ESI") + 1, PLANIFICADOR);
	mensajeEjec->header->tipo_mensaje = EJECUTAR;

	int res_ejecutar = enviar_y_loguear_mensaje(esi_seleccionado.pid, *mensajeEjec, "ESI\0");
	free(mensajeEjec);
	if (res_ejecutar < 0) {exit_proceso(-1);}
	}
}

bool clave_ya_bloqueada_config(char* clave) {
	return (strcmp(operacionEnMemoria->clave,clave) == 0);
}

bool clave_ya_bloqueada(struct_blocked* elemento) {
	return (strcmp(operacionEnMemoria->clave,elemento->clave) == 0);
}

bool clave_set_disponible(struct_blocked* elemento) {
	return (strcmp(operacionEnMemoria->clave,elemento->clave) == 0 && (elemento->pcb.pid == esiRunning.pid));
}

bool buscar_esi_ready(struct_ready* elemento) {
	return (elemento->pcb.pid == esiRunning.pid);
}

bool buscar_esi_a_bloquear(struct_ready* elemento) {
	return (elemento->pcb.pid == atoi(list_comandos[2]));
}

bool buscar_esi_a_desbloquear(struct_blocked* elemento) {
	return strcmp(elemento->clave,list_comandos[1]) == 0;
}

bool buscar_esi_kill(struct_blocked* elemento) {
	return (elemento->pcb.pid == esi_a_eliminar);
}

bool esi_espera_clave(struct_blocked* elemento) {
	string_to_upper(elemento->clave);
	bool resultado = strcmp(list_comandos[1],elemento->clave) == 0;
	return resultado;
}

void desbloquear_esi() {
	struct_blocked* esi_a_desbloquear;
	esi_a_desbloquear = (struct_blocked*)list_find(cola_esi_blocked, (void*)clave_ya_bloqueada);
	if (esi_a_desbloquear != NULL)
	{
		list_remove_by_condition(cola_esi_blocked, ((void*) clave_set_disponible));
		agregar_ready(esi_a_desbloquear->pcb);
	}
}



int envio_ejecutar(int socket) {
	flag_instruccion = true;
	Message* mensajeEjec = empaquetar_texto("Planificador pide ejecutar ESI", strlen("Planificador pide ejecutar ESI") + 1, PLANIFICADOR);
	mensajeEjec->header->tipo_mensaje = EJECUTAR;

	int res_ejecutar = enviar_y_loguear_mensaje(socket, *mensajeEjec, "ESI\0");
	free(mensajeEjec);
	if (res_ejecutar < 0) {exit_proceso(-1);}

	return 0;
}

int envio_desconexion(int socket) {
	Message* mensajeDesc = empaquetar_texto("Planificador pide desconectar al ESI", strlen("Planificador pide ejecutar ESI") + 1, PLANIFICADOR);
	mensajeDesc->header->tipo_mensaje = DESCONEXION;

	int res_desconectar = enviar_y_loguear_mensaje(socket, *mensajeDesc, "ESI\0");
	free(mensajeDesc);
	if (res_desconectar < 0) {exit_proceso(-1);}

	return 0;
}

void ejecutar_comando(int nroComando) {
	if (flag_instruccion == false)
		{
		switch(nroComando){
			case CONTINUAR:
				reanudar_ejecucion();
				break;
			case PAUSAR:
				flag_puede_ejecutar = false;
				break;
			case BLOQUEAR:
				consola_bloquear();
				break;
			case DESBLOQUEAR:
				consola_desbloquear();
				break;
			case LISTAR:
				listar_esis_porClave();
				break;
			case KILL:
				consola_kill();
				break;
			case STATUS:
				obtener_status();
				break;
		}

		liberar_split(list_comandos);
		flag_comando_pendiente = false;
	}
	else
	{
		flag_comando_pendiente = true;
	}
}

void buscar_y_correr_comando() {
	flag_instruccion = false;

	if (flag_comando_pendiente == true)
		ejecutar_comando(nroComando);
}

void reanudar_ejecucion() {
	if (flag_puede_ejecutar == false)
	{
		flag_puede_ejecutar = true;

		if (esiRunning.pid == 0) {
			ejecutar_nueva_esi();
		}
		else
		{
			envio_ejecutar(esiRunning.pid);
		}
	}
}

void listar_esis_porClave(char* clave) {
	int cantidad_esis_clave = list_count_satisfying(cola_esi_blocked, (void*)esi_espera_clave);

	if (cantidad_esis_clave > 0) {
		t_list* esis_bloqueados = list_filter(cola_esi_blocked, (void*)esi_espera_clave);
		log_info(log_consola,"\nListado de ESIS esperando por la clave: %s", list_comandos[1]);

		for (int i = 0; i < cantidad_esis_clave; i++) {
			log_info(log_consola,"\nESI N°%d", ((struct_blocked*)list_get(esis_bloqueados, i))->pcb.pid);
		}

		list_destroy(esis_bloqueados);
	}
	else
	{
		log_info(log_consola,"\nNo hay ESIS esperando por la clave: %s", list_comandos[1]);
	}
}

int obtener_status() {
	//Envío mensaje al esi preguntando en que instancia se encuentra (o se debería)
	//encontrar la clave
	Message * mensaje = empaquetar_STATUS(list_comandos[1], "",strlen(list_comandos[1]) + 1,0,PLANIFICADOR,0);

	if (enviar_y_loguear_mensaje(pidCoordinador, *mensaje, "COORDINADOR\0")<0)
		return ERROR_DE_ENVIO;

	free_msg(&mensaje);
	Message * respuesta = malloc(sizeof(Message));

	if (await_msg(pidCoordinador, respuesta)<0)
		return ERROR_DE_RECEPCION;

	char* clave_resp = NULL;
	char* inst_resp = NULL;

	int es_instancia_real = desempaquetar_status(respuesta,clave_resp,inst_resp);

	//Logueo y muestro resultados por la consola
	//Reutilizando funcionalidad del listar para obtener ESIS que esperan por dicha clave
	if (es_instancia_real == 1)
	{
		log_info(log_planificador,"Instancia Real para la clave %s : %s", clave_resp,inst_resp);
	}
	else if (es_instancia_real == 0)
	{
		log_info(log_planificador,"Instancia Simulada según algoritmo para la clave %s : %s", clave_resp,inst_resp);

	}
	else
	{
		log_error(log_planificador,"\nError al recibir status para la clave: %s", list_comandos[1]);
		return ERROR_DE_RECEPCION;
	}

	free(clave_resp);
	free(inst_resp);
	free_msg(&respuesta);

	listar_esis_porClave(list_comandos[1]);

	return OK;
}

int obtener_id_esi(struct_blocked* elemento) {
	return elemento->pcb.pid;
}

void consola_bloquear() {
	bool bloqueo_ok = false;

	//Verifico si el esi a bloquear está en la cola de ready y si es así lo bloqueo
	if (list_any_satisfy(cola_ready, (void*)buscar_esi_a_bloquear)) {
		struct_ready* esiBloqueado = list_get((void*)list_filter(cola_ready, (void*)buscar_esi_a_bloquear),1);
		list_remove_by_condition(cola_ready, (void*)buscar_esi_a_bloquear);
		agregar_esi_blocked(esiBloqueado->pcb, list_comandos[1]);
		free(esiBloqueado);
		bloqueo_ok = true;
	}

	//Verifico si el esi a bloquear es el esiRunning. Si es así lo bloqueo y mando nuevo ESI a correr
	if (esiRunning.pid == atoi(list_comandos[2])) {
		agregar_esi_blocked(esiRunning, list_comandos[1]);
		esiRunning = inicializar_pcb();
		free_operacion(&operacionEnMemoria);
		bloqueo_ok = true;
	}

	//Informo si pudo bloquear el ESI o no. Caso negativo explicar motivo
	if (bloqueo_ok)
		log_info(log_consola, "Se bloqueó correctamente el esi %s para la clave %s",list_comandos[2],list_comandos[1]);
	else
	{
		log_info(log_consola,"El ESI ingresado es inválido");
	}

	//Si se bloqueo el que estaba corriendo, busco un nuevo ESI para que corra
	//Si no hay ningún ESI corriendo, se asigna uno nuevo
	if (esiRunning.pid == 0)
	{
		esiRunning = planificar_esis();
		if (esiRunning.pid != 0)
		{
			envio_ejecutar(esiRunning.pid);
		}
	}
}

void consola_desbloquear() {

	if (list_any_satisfy(cola_esi_blocked, (void*)buscar_esi_a_desbloquear)) {
		struct_blocked* esi_a_desbloquear;
		esi_a_desbloquear = (struct_blocked*)list_find(cola_esi_blocked, (void*)buscar_esi_a_desbloquear);
		list_remove_by_condition(cola_esi_blocked, ((void*) buscar_esi_a_desbloquear));
		agregar_ready(esi_a_desbloquear->pcb);

		log_info(log_consola,"Se desbloqueo el esi %d para la clave %s",esi_a_desbloquear->pcb.pid,esi_a_desbloquear->clave);

		free(esi_a_desbloquear);

		//Si no hay ningún ESI corriendo, se asigna uno nuevo
		if (esiRunning.pid == 0)
		{
			esiRunning = planificar_esis();
			if (esiRunning.pid != 0)
			{
				envio_ejecutar(esiRunning.pid);
			}
		}
	}
	else
	{
		list_remove_by_condition(cola_blocked, ((void*) buscar_esi_a_desbloquear));
		log_info(log_consola,"La clave %s se encuentra liberada",list_comandos[1]);
	}
}

void consola_kill() {
	bool kill_ok = false;
	esi_a_eliminar = atoi(list_comandos[1]);

	//Verifico si el esi a finalizar está en la cola de ready
	if (list_any_satisfy(cola_ready, (void*)buscar_esi_kill)) {
		envio_desconexion(esi_a_eliminar);
		kill_ok = true;
	}

	//Verifico si el esi a finalizar está en la cola de esis bloqueados
	else if (list_any_satisfy(cola_esi_blocked, (void*)buscar_esi_kill)) {
		envio_desconexion(esi_a_eliminar);
		kill_ok = true;
	}

	//Verifico si el esi a finalizar es el esiRunning.
	else if (esiRunning.pid == esi_a_eliminar) {
		envio_desconexion(esi_a_eliminar);
		free_operacion(&operacionEnMemoria);
		kill_ok = true;
	}

	//Informo si pudo finalizar el ESI o no se encontró
	if (kill_ok)
	{
		log_info(log_consola, "Se eliminó correctamente el esi %s ",list_comandos[1]);
	}
	else
	{
		log_info(log_consola,"No se encontró el ESI ingresado");
	}
}
