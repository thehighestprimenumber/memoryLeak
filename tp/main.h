#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <commons/log.h>

void configurar_logger();
void salir(int return_nr);

t_log * logger;

#endif
