#include "main.h"

int main() {
	configurar_logger();
}

void configurar_logger(){
logger = log_create("/home/utnso/tp/redistinto.log", "redistinto", true,
			LOG_LEVEL_INFO);

}
