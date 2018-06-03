#include "common.h"

void free_memory(void **puntero){
	if((*puntero) != NULL){
		free(*puntero);
		*puntero = NULL;
	}
}
