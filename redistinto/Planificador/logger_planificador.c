#include "logger_planificador.h"

void registrar_conexion(int socket) {
	log_info(log_planificador,
			"el planificador recibe conexion de nueva esi, socket %d", socket);
}



