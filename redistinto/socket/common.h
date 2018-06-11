
#ifndef REDISTINTO_COMMON_H_
#define REDISTINTO_COMMON_H_

//Includes
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//Funciones
void free_memory(void **puntero);
char* get_local_ip();


#endif /* REDISTINTO_COMMON_H_ */
