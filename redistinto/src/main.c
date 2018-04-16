#include "main.h"
#include <commons/log.h>
#include "planificador.h"

int main(void) {
	estructura_planificador();
	configurar_logger();

	printf("OK!");
}

void configurar_logger(){
	logger = log_create("/home/utnso/tp/redistinto.log", "redistinto", true, LOG_LEVEL_INFO);

}
