#include "esi.h"
#include "parser.h"

int main(int argc,char *argv[]) {

	// Levantamos configuracion

	esi_configuracion* pConfig = (esi_configuracion*) malloc(sizeof(esi_configuracion));;
	t_config* config = config_create("./configESI.txt");

	if (config == NULL) {
		config = config_create("../configESI.txt");
	}

	pConfig->coordinador_ip = leer_propiedad_string(config, "IP_coordinador");
	pConfig->coordinador_puerto = leer_propiedad_string(config, "puerto_coordinador");
	pConfig->planificador_ip = leer_propiedad_string(config, "IP_planificador");
	pConfig->planificador_puerto = leer_propiedad_string(config, "puerto_planificador");

	// Levantamos el archivo de log y guardamos IP y Puerto
	log_esi = log_create("log_esi.log", "ESI", true, LOG_LEVEL_TRACE);
	log_trace(log_esi,"Inicia el proceso ESI");
	log_trace(log_esi,"El puerto del coordinador es: %s",pConfig->coordinador_puerto);
	log_trace(log_esi,"La ip del coordinador es: %s",pConfig->coordinador_ip);
	log_trace(log_esi,"El puerto del planificador es: %s",pConfig->planificador_puerto);
	log_trace(log_esi,"La ip del planificador es: %s",pConfig->planificador_ip);

	/*printf("Inicia el proceso ESI\n");
	printf("El puerto del coordinador es: %s\n",pConfig->coordinador_puerto);
	printf("La ip del coordinador es: %s\n",pConfig->coordinador_ip);
	printf("El puerto del plnificador es: %s\n",pConfig->planificador_puerto);
	printf("La ip del planificador es: %s\n",pConfig->planificador_ip);*/

	// Nos conectamos y pedimos handshake al planificador, este nos asigna un identificador
	//conectar_a_planificador(pConfig);

	// Nos conectamos y pedimos handshake al coordinador
	conectar_a_coordinador(pConfig);

	return EXIT_SUCCESS;
}

// Funciones de inicialización

char* leer_propiedad_string (t_config *configuracion, char* propiedad){
	if (config_has_property(configuracion, propiedad)){
		return config_get_string_value(configuracion,propiedad);
	}
	return "";
}

void conectar_a_planificador(esi_configuracion* pConfig) {

	t_paquete paquete;

	int resultado = connect_to_server(pConfig->planificador_ip,pConfig->planificador_puerto);
	//Verifico conexion con el planificador
	if (resultado != 0) {
		log_error(log_esi, "Fallo conexion con el Planificador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Planificador");
	}

	/*pedir_handshake(&cliente_planificador, ESI);

	paquete = recibir_paquete(&cliente_planificador);*/

	memcpy(&identificador,(int*)paquete.pBuffer, sizeof(int));
	printf("El planificador asigno el id:%d a este ESI.\n", identificador);
	log_trace(log_esi,"EL planificador asigno el id:%d a este ESI.", identificador);
	destruir_paquete(paquete);
}

void conectar_a_coordinador(esi_configuracion* pConfig) {

	int resultado = connect_to_server(pConfig->coordinador_ip,pConfig->coordinador_puerto);
	//Verifico conexion con el coordinador
	if (resultado == -1) {
		log_error(log_esi, "Fallo conexion con el Coordinador");
		exit(EXIT_FAILURE);
	} else {
		log_info(log_esi, "ESI se conecto con el Coordinador");
	}

	Message* msg= (Message*) malloc(sizeof(Message));
	msg->contenido = (char*) malloc(strlen("Hola coordinador"));
	msg->contenido = "Hola coordinador";
	msg->header = (ContentHeader*) malloc(sizeof(ContentHeader*));
	msg->header->remitente = ESI;
	msg->header->tipo_mensaje = TEST;
	msg->header->size = strlen(msg->contenido);

	if (send_msg(resultado, (*msg))<0) log_debug(log_esi, "Error al enviar el mensaje");
	log_debug(log_esi, "Se envio el mensaje");
	while (1) {
		Message msg;
		log_debug(log_esi, "esperando mensaje");
		int result = await_msg(resultado, &msg);
		log_debug(log_esi, "llego un mensaje. parseando...");
		if (result<0){
			log_debug(log_esi, "error de recepcion");
			continue;
			//return ERROR_DE_RECEPCION;
		}

	char * request = malloc(msg.header->size);
	strcpy(request, (char *) msg.contenido);
	log_debug(log_esi, "mensaje recibido: %s", request);
	//log_debug(log_inst, "%s", request);

	}
}

int interpretar(int argc, char **argv){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[1], "r");
    if (fp == NULL){
        perror("Error al abrir el archivo: ");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        t_esi_operacion parsed = parse(line);

        if(parsed.valido){
            switch(parsed.keyword){
                case GET:
                    printf("GET\tclave: <%s>\n", parsed.argumentos.GET.clave);
                    break;
                case SET:
                    printf("SET\tclave: <%s>\tvalor: <%s>\n", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
                    break;
                case STORE:
                    printf("STORE\tclave: <%s>\n", parsed.argumentos.STORE.clave);
                    break;
                default:
                    fprintf(stderr, "No pude interpretar <%s>\n", line);
                    exit(EXIT_FAILURE);
            }

            destruir_operacion(parsed);
        } else {
            fprintf(stderr, "La linea <%s> no es valida\n", line);
            exit(EXIT_FAILURE);
        }
    }

    fclose(fp);
    if (line)
        free(line);

    return EXIT_SUCCESS;
}
