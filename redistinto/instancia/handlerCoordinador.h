#ifndef INSTANCIA_HANDLERCOORDINADOR_H_
#define INSTANCIA_HANDLERCOORDINADOR_H_

#include "../socket/socket.h"
#include "../socket/operacion.h"
#include "../socket/common.h"
#include "instancia.h"

int conectar_a_coordinador(char* ip, char* puerto);
bool recibir_config_storage();
void configurar_storage();

#endif
