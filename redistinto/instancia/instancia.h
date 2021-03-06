#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/log.h>
#include "../socket/operacion.h"
#include "configuracionInstancia.h"
#include "handlerCoordinador.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <commons/txt.h>
#include <pthread.h>
#include <dirent.h>

#define OK 0
#define HACER_DUMPS true
#define LOGUEAR_DUMPS true
#define configuracionDefault "configInstancia.txt\0"

typedef enum algorimoReemplazo{CIRC, LRU, BSU} algorimoReemplazo;

typedef struct{
	int largo_clave;
	char* clave;
	int largo_valor;
	int nroEntrada;
	void *datos;
} t_clave_valor;

#include "compactador.h"
#include "circular.h"
#include "lru.h"
#include "bsu.h"


typedef struct {
	int identificador;
	char* ip_coordinador;
	char* puerto_coord;
	int socket_coordinador;
	algorimoReemplazo algorimoActual;
	char* nombre_inst;
	int int_dump;
	char* path;
	int tamEntrada;
	int cantEntradas;
	t_list* tabla_entradas;
} t_instancia;

t_log * log_inst;
t_instancia instancia;
char *storage;
char *claveEnBusqueda;
struct stat pepito;
sem_t *semTabla;


int inicializar();
int manejar_operacion(Message * msg);
t_clave_valor* buscar_clave_valor (char* clave);
int asignar_valor_a_clave(char* clave, int largo_clave, char* valor, int largo_valor);
int agregar_clave_a_lista(char* clave, int largo_clave);
void guardar(void * entrada);
void dump();
void eliminar_entrada(void *contenido);
int guardar_entrada(char* clave, int largo_clave);
void* dump_automatico(void*);
void recuperar_claves();
void sumardor_parcial_espacio_usado(void *contenido);

#endif /* INSTANCIA_INSTANCIA_H_ */
