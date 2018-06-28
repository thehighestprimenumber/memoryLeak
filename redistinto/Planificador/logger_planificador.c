#include "logger_planificador.h"

void registrar_conexion(int socket) {
	log_info(log_planificador,
			"el planificador recibe conexion de nueva esi, socket %d", socket);
}

void inicializar_logger() {
	log_planificador = log_create("./Planificador.log", "Planificador: ", false, LOG_LEVEL_INFO);
	log_consola = log_create("./Consola.log", "Consola Planificador: ", true, LOG_LEVEL_INFO);
}


