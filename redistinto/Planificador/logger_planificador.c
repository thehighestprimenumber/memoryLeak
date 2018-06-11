#include "logger_planificador.h"

//todo lo demas se mudo a logger_comun

void registrar_conexion(int socket) {
	log_info(log_planificador,
			"el planificador recibe conexion de nueva esi, socket %d", socket);
}



