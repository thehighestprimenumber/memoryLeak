#ifndef MAIN_H

	#define MAIN_H

	#define NULL 0

	#include <stdbool.h>
	#include <commons/log.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	void configurar_logger();
	void salir(int return_nr);

	t_log* logger;


#endif
