
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
#include <time.h>
#include <sys/timeb.h>

//Funciones
void free_memory(void **puntero);
char* get_local_ip();
int cantidad_lineas_script(char* script);
long long int get_time_millisec();
void liberar_split(char** array);

#endif /* REDISTINTO_COMMON_H_ */
