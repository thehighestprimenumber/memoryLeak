#ifndef LRU_H_
#define LRU_H_

#include "instancia.h"
#include "circular.h"
#include "compactador.h"
#include "../socket/common.h"


typedef struct {
	long long int instant;
} t_info_lru;

bool cirularFinalizo;

void inicializar_lru();
int guardar_lru(t_clave_valor *entrada, char *valor);
void registrar_set_lru(char *clave);

#endif /* LRU_H_ */
