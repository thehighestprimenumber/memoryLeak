#include "consola.h"

void* abrir_consola(void* ptr) {
	char* retorno;
	char* linea;
	while(1) {
		linea = readline(">");

		if (!linea) {
			break;
		}

		linea = limpiar_comando(linea);

		if (strcmp(linea,"EXIT") == 0) {
			break;
		}

		if (linea)
		{
			if (strcmp(linea,"\0") != 0)
			{
				add_history(linea);
				procesar_funcion(linea);
			}
		}

		free(linea);
	}

	retorno = (char*) ptr;
	return (void*) retorno;
}

void procesar_funcion(char* comando) {
		char** list_comandos;

		list_comandos = string_split(comando," ");

		if (strcmp(list_comandos[0],"CONTINUAR") == 0)
			comando_continuar(list_comandos);
		else if (strcmp(list_comandos[0],"PAUSAR") == 0)
			comando_pausar(list_comandos);
		else if (strcmp(list_comandos[0],"BLOQUEAR") == 0)
			comando_bloquear(list_comandos);
		else if (strcmp(list_comandos[0],"DESBLOQUEAR") == 0)
			comando_desbloquear(list_comandos);
		else if (strcmp(list_comandos[0],"LISTAR") == 0)
			comando_listar(list_comandos);
		else if (strcmp(list_comandos[0],"KILL") == 0)
			comando_kill(list_comandos);
		else if (strcmp(list_comandos[0],"STATUS") == 0)
			comando_status(list_comandos);
		else if (strcmp(list_comandos[0],"DEADLOCK") == 0)
			comando_deadlock(list_comandos);
		else
			printf("Usted ha ingresado un comando inválido\n");
	free(list_comandos);
}

void comando_continuar(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 1)
		printf("ingresó el comando Continuar\n");
	else
		printf("Error al ingresar comando Continuar, este no debe llevar parámetros\n");
}

void comando_pausar(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 1)
		printf("ingresó el comando Pausar\n");
	else
		printf("Error al ingresar comando Pausar, este no debe llevar parámetros\n");
}

void comando_bloquear(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 3)
		printf("ingresó el comando Bloquear\n");
	else
		printf("Error al ingresar comando Bloquear, este requiere 2 parámetros\n");
}

void comando_desbloquear(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
		printf("Se ingresó el comando Desbloquear\n");
	else
		printf("Error al ingresar comando Desbloquear, este requiere 1 parámetro\n");
}

void comando_listar(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
		printf("Se ingresó el comando Listar\n");
	else
		printf("Error al ingresar comando Listar, este requiere 1 parámetro\n");
}

void comando_kill(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
		printf("Se ingresó el comando Kill\n");
	else
		printf("Error al ingresar comando Kill, este requiere 1 parámetro\n");
}

void comando_status(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
		printf("ingresó el comando Status\n");
	else
		printf("Error al ingresar comando Status, este requiere 1 parámetro\n");
}

void comando_deadlock(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 1)
		printf("ingresó el comando Deadlock\n");
	else
		printf("Erro al ingresar comando Deadlock, este no debe llevar parámetros\n");
}

char* limpiar_comando(char* comando) {
	string_to_upper(comando);
	string_trim(&comando);
	return comando;
}

int tamanio_array(char** cadena) {
	int i = 0;
	while (cadena[i] != NULL) {
		i++;
	}

	return i;
}
