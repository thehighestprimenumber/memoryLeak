#include "instancia.h"
#include "configuracionInstancia.h"
#include "handlerCoordinador.h"

int main(){
	t_config* config = config_create("configuracionInstancia.txt");
	char* ip = leer_propiedad_string(config, "IP_coordinador");
	char* puerto = leer_propiedad_string(config, "puerto_coordinador");
	int socket_coordinador = conectar_a_coordinador(ip, puerto);
	if(socket_coordinador == -1){
	 printf("No se pudo conectar con el coordinador");
	 return -1;
	}
	printf("Se pudo conectar con el coordinador");
	return 0;
}
