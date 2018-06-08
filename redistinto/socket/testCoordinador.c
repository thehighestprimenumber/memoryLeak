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

int correr_tests_instancia(){
	connect_to_server(IP, PUERTO_COORDINADOR);

	char* inst2 = "inst2";


	test_INST_connect(inst2);

	return 0;
}

int test_operacion(){
	t_operacion * op = crear_operacion("una:clave", strlen("una:clave"), "unValor12345", strlen("unValor12345"), op_GET);
	Message * m = empaquetar_op_en_mensaje(op, ESI);
	t_operacion * desemp = desempaquetar_operacion(m);


	free(desemp);
	free_msg(&m);

	return 0;
}

int mandar_operaciones_test(t_operacion * op){
	int socket_coordinador =  connect_to_server(IP, PUERTO_COORDINADOR);
	if(socket_coordinador == -1)
		return -10;
	Message * m = empaquetar_op_en_mensaje(op, ESI);
	int res = enviar_mensaje_test(socket_coordinador, *m);
			if (res<0) return ERROR_DE_ENVIO;
			free_msg(m);

		while (1) {
			Message *msg;
			int resultado = await_msg(socket_coordinador, msg);
			free_msg(&msg);
			if (resultado<0){
				return ERROR_DE_RECEPCION;
			}

		}

}
int test_ESI_get(){
	t_operacion * op = crear_operacion("una:clave", strlen("una:clave"), " ", 1, op_GET);
	return mandar_operaciones_test(op);
}

int test_ESI_set(){
	t_operacion * op = crear_operacion("una:clave", strlen("una:clave"), "unvalor12345", strlen("unvalor12345")+1, op_SET);
	return mandar_operaciones_test(op);
}

int test_ESI_store(){
	t_operacion * op = crear_operacion("una:clave", strlen("una:clave"), " ", 1, op_STORE);
	return mandar_operaciones_test(op);
}


int test_INST_connect(char* nombre_instancia){
	int socket_coordinador = connect_to_server(IP, PUERTO_COORDINADOR);
	if(socket_coordinador == -1)
		return -10;
	Message * mensaje = empaquetar_conexion(INSTANCIA, nombre_instancia);
	int res = enviar_mensaje_test(socket_coordinador, *mensaje);
		if (res<0)
			return ERROR_DE_ENVIO;

	while (1) {
		Message msg;
		int resultado = await_msg(socket_coordinador, &msg);
		if (resultado<0){
			return ERROR_DE_RECEPCION;
		}
		switch (msg.header->tipo_mensaje){
		case ACK : printf("ok");
					break;
		case OPERACION:;
			Message * m = empaquetar_ack(INSTANCIA);
			enviar_mensaje_test(socket_coordinador, *m);
			break;
		default: printf("%s: mensaje recibido: %s", nombre_instancia, (char*) msg.contenido);
				break;
		}
		free(&msg);

	}
	return res;
}
