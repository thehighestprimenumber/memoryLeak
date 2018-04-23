#include "consola.h"

void* abrir_consola(void* ptr) {
	char* retorno;
	char* linea;
	while(1) {
		linea = readline(">");

		if (!linea) {
			break;
		}

		string_to_upper(linea);

		if (strcmp(linea,"EXIT") == 0) {
			break;
		}

		if (linea)
		{
			add_history(linea);
			procesar_funcion(linea);
		}

		free(linea);
	}

	retorno = (char*) ptr;
	return (void*) retorno;
}

void procesar_funcion(char* comando) {
		if (strcmp(comando,"CONTINUAR") == 0)
			comando_continuar();
		else if (strcmp(comando,"PAUSAR") == 0)
			comando_pausar();
		else if (strcmp(comando,"BLOQUEAR") == 0)
			comando_bloquear("",0);
		else if (strcmp(comando,"DESBLOQUEAR") == 0)
			comando_desbloquear(0);
		else if (strcmp(comando,"LISTAR") == 0)
			comando_listar("");
		else if (strcmp(comando,"KILL") == 0)
			comando_kill(0);
		else if (strcmp(comando,"STATUS") == 0)
			comando_status();
		else if (strcmp(comando,"DEADLOCK") == 0)
			comando_deadlock();
		else
			printf("Usted ha ingresado un comando inválido\n");
}

void comando_continuar() {
	printf("ingresó el comando Continuar\n");
}

void comando_pausar() {
	printf("ingresó el comando Pausar\n");
}

void comando_bloquear(char* clave, int pid) {
	printf("ingresó el comando Bloquear\n");
}

void comando_desbloquear(char* clave) {
	printf("Se ingresó el comando Desbloquear\n");
}

void comando_listar(char* recurso) {
	printf("Se ingresó el comando Listar\n");
}

void comando_kill(int pid) {
	printf("Se ingresó el comando Kill\n");
}

void comando_status() {
	printf("ingresó el comando Status\n");
}

void comando_deadlock() {
	printf("ingresó el comando Deadlock\n");
}
