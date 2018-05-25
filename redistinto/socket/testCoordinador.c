#include "testCoordinador.h"
#include "../coordinador/conexiones.h"
#include "operacion.h"

int enviar_mensaje_test(int socket, Message msg) {
	int res = send_msg(socket, (msg));
	if (res < 0) {
		return ERROR_DE_ENVIO;
	}
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

	free_msg(desemp);
	free_msg(m);
}

int test_ESI_get(){
	int socket_coordinador =  connect_to_server(IP, PUERTO_COORDINADOR);
	if(socket_coordinador == -1) return -10;
	char* nombre_ESI = calloc(1, strlen("ESI1")+1);
	strcpy(nombre_ESI, "ESI1");
	t_operacion * op = (t_operacion *) malloc(sizeof(t_operacion));
		op->clave = calloc(1, strlen("claracla")+1);
		memcpy(op->clave, "claracla", strlen("claracla"));
		op->largo_clave=strlen(op->clave)+1;
		op->tipo = op_GET;
		op->valor = calloc(1, strlen(nombre_ESI)+1);
		strcpy(op->valor, nombre_ESI);
		op->largo_valor=strlen(op->valor)+1;
		Message * m = empaquetar_op_en_mensaje(op, ESI);

	int res = enviar_mensaje_test(socket_coordinador, *m);
		if (res<0) return ERROR_DE_ENVIO;
		//free_msg(m); FIXME

	while (1) {
		Message msg;
		int resultado = await_msg(socket_coordinador, &msg);
		free_msg(&msg);
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
	Message * mensaje = empaquetar_conexion(INSTANCIA, nombre_instancia);
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
