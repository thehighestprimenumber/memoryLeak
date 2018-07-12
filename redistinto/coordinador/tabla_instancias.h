#ifndef TABLA_INSTANCIAS_H_
#define TABLA_INSTANCIAS_H_

#include <commons/collections/list.h>

#include <semaphore.h>

#define MAX_LEN_INSTANCE_NAME 20
#define MAX_LEN_NOMBRE_CLAVE 40
#define MAX_CLAVES_INSTANCIA 100


typedef struct {
	int cantidad_entradas;
	int tamanio_entrada;
} t_tabla_instancias;
t_tabla_instancias tabla_instancias;

typedef struct {
	int socket_instancia;
	int max_claves_instancia;
	char claves[MAX_CLAVES_INSTANCIA][MAX_LEN_NOMBRE_CLAVE];
	int esta_activa;
	sem_t lock;
	char nombre_instancia[MAX_LEN_INSTANCE_NAME];
	int entradas_libres;
} fila_tabla_instancias;

void free_fila_tabla_instancias(fila_tabla_instancias** fila);
void agregarClave(fila_tabla_instancias *instancia, char* clave);
int ultima_instancia_usada;

int cambiar_estado_instancia(fila_tabla_instancias* fila, int esta_activa);

fila_tabla_instancias* seleccionar_instancia(char* clave);

fila_tabla_instancias* registrar_instancia(char* nombre_instancia, int socket_instancia);

int desconectar_instancia(int socket);

int criterio_clave(fila_tabla_instancias* fila, void* nombre_clave);
int criterio_nombre(fila_tabla_instancias* fila, void* nombre_instancia);
int criterio_socket(fila_tabla_instancias* fila, void* numero_socket);

void buscar_instancia_por_valor_criterio (void* valor, int criterio (fila_tabla_instancias*, void*), fila_tabla_instancias** output);
void esperar_operacion(fila_tabla_instancias* instancia);

#endif
