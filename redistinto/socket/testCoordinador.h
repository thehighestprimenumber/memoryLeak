#ifndef TESTCOORDINADOR_H_
#define TESTCOORDINADOR_H_

#include "../socket/operacion.h"
#include "../coordinador/conexiones.h"

int correr_tests_instancia();
int test_empaquetado();
int test_ESI_get();
int test_ESI_set(char* nombre_esi);
int test_INST_connect(char*);

#endif /* TESTCOORDINADOR_H_ */
