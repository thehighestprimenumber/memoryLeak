#ifndef TABLA_INSTANCIAS_H_
#define TABLA_INSTANCIAS_H_

#include <commons/collections/list.h>

#include <semaphore.h>

#define MAX_LEN_INSTANCE_NAME 20

typedef struct {
	int socket_instancia;
	t_list* claves;
	int esta_activa;
	sem_t lock;
	char nombre_instancia[MAX_LEN_INSTANCE_NAME];
} fila_tabla_instancias;

void free_fila_tabla_instancias(fila_tabla_instancias** fila);

int ultima_instancia_usada;


int cambiar_estado_instancia(fila_tabla_instancias* fila, int esta_activa);

fila_tabla_instancias* seleccionar_instancia(char* clave);

fila_tabla_instancias* registrar_instancia(char* nombre_instancia, int socket_instancia);

int desconectar_instancia(int socket);

int criterio_clave(fila_tabla_instancias* fila, void* nombre_clave);
int criterio_nombre(fila_tabla_instancias* fila, void* nombre_instancia);
int criterio_socket(fila_tabla_instancias* fila, void* numero_socket);

fila_tabla_instancias * buscar_instancia_por_valor_criterio (void* valor, int criterio (fila_tabla_instancias*, void*));
void esperar_operacion(fila_tabla_instancias* instancia);

#endif
