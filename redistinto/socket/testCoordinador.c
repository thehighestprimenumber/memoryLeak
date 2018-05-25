#include "testCoordinador.h"
#include "../coordinador/conexiones.h"
#include "operacion.h"

int enviar_mensaje_test(int socket, Message msg) {
	int res = send_msg(socket, (msg));
	if (res < 0) {
		return ERROR_DE_ENVIO;
	}
	//FIXME free_msg(&msg);

	return 0;
}

int test_operacion(){
	t_operacion op = {.valor="farafa\0", .clave="claracla\0", .tipo = op_GET};
	op.largo_clave = strlen(op.clave)+1;
	op.largo_valor = strlen(op.valor)+1;
	Message * m = empaquetar_op_en_mensaje(&op, ESI);
	t_operacion * desemp = (t_operacion *) m->contenido;
	printf("%d", desemp->tipo);
	printf("%s", desemp->clave);
	printf("%s", desemp->valor);
	return 0;
}

int test_ESI_get(char * nombre_ESI){
	int socket_coordinador = connect_to_server(IP, PUERTO_COORDINADOR);
	if(socket_coordinador == -1) return -10;

	t_operacion op = {.valor=nombre_ESI, .clave="claracla\0"};
		op.tipo = op_GET;
		op.largo_clave=strlen(op.clave)+1;
		op.largo_valor=strlen(op.valor)+1;
		Message * m = empaquetar_op_en_mensaje(&op, ESI);

	int res = enviar_mensaje_test(socket_coordinador, *m);
		if (res<0) return ERROR_DE_ENVIO;

	while (1) {
		Message msg;
		int resultado = await_msg(socket_coordinador, &msg);
		if (resultado<0){
			return ERROR_DE_RECEPCION;
		}
	}
	return 0;
}


int test_INST_connect(char* nombre_instancia){
	int socket_coordinador = connect_to_server(IP, PUERTO_COORDINADOR);
	if(socket_coordinador == -1) return -10;
	sleep(5);
	Message * mensaje = empaquetar_conexion(COORDINADOR);
	int res = enviar_mensaje_test(socket_coordinador, *mensaje);
				//if (res<0) return ERROR_DE_ENVIO;

	while (1) {
		Message msg;
		int resultado = await_msg(socket_coordinador, &msg);
		if (resultado<0){
			return ERROR_DE_RECEPCION;
		}
		if (msg.header->tipo_mensaje == ACK)
			printf("ok");
//TODO validar otros tipos
		else
			printf("%s: mensaje recibido: %s", nombre_instancia, (char*) msg.contenido);

	}
	return res;
}
