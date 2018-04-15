#include "main.h"
#include <commons/log.h>

int main(void) {
	configurar_logger();
	printf("OK!");
}

void configurar_logger(){
	logger = log_create("/home/utnso/tp/redistinto.log", "redistinto", true, LOG_LEVEL_INFO);

}
