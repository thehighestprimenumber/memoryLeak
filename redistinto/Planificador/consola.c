#include "consola.h"

int leer_consola() {
	memset(comando_consola, '\0', 1000);
	fgets(comando_consola, 1000, stdin);
	return 0;
}

void quitar_salto_linea(char *cadena) {
	char *cadena2 = cadena;
	while (*cadena != '\0') {
		if (*cadena != '\n') {
			*cadena2++ = *cadena++;
		} else {
			++cadena;
		}
	}
	*cadena2 = '\0';
}

int decodificar_comando() {
	int i = 0;
	int resultado = 0;

	log_info(log_consola,"consola: %s", comando_consola);

	list_comandos = string_split(comando_consola," ");

	char* nombreComando = malloc(strlen(list_comandos[0]) + 1);
	strcpy(nombreComando,list_comandos[0]);
	string_to_upper(nombreComando);

	while (list_comandos[i] != NULL) {
		quitar_salto_linea(list_comandos[i]); //saco el \n si es que lo tiene
		i++;
	}

	if (strcmp(nombreComando,"CONTINUAR") == 0)
		resultado = comando_continuar(list_comandos);
	else if (strcmp(nombreComando,"PAUSAR") == 0)
		resultado = comando_pausar(list_comandos);
	else if (strcmp(nombreComando,"BLOQUEAR") == 0)
		resultado = comando_bloquear(list_comandos);
	else if (strcmp(nombreComando,"DESBLOQUEAR") == 0)
		resultado = comando_desbloquear(list_comandos);
	else if (strcmp(nombreComando,"LISTAR") == 0)
		resultado = comando_listar(list_comandos);
	else if (strcmp(nombreComando,"KILL") == 0)
		resultado = comando_kill(list_comandos);
	else if (strcmp(nombreComando,"STATUS") == 0)
		resultado = comando_status(list_comandos);
	else
	{
		log_error(log_consola,"Usted ha ingresado un comando inválido\n");
		resultado = ERROR;
	}

	free(nombreComando);
	return resultado;
}

int comando_continuar(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 1)
	{
		log_info(log_consola,"Se continua con la ejecución de las instrucciones de las esi\n");
		return CONTINUAR;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Continuar, este no debe llevar parámetros\n");
		return ERROR;
	}
}

int comando_pausar(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 1)
	{
		log_info(log_consola,"Se pauso la ejecución de las instrucciones de las esi\n");
		return PAUSAR;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Pausar, este no debe llevar parámetros\n");
		return ERROR;
	}
}

int comando_bloquear(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 3)
	{
		log_info(log_consola,"ingresó el comando Bloquear\n");
		return BLOQUEAR;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Bloquear, este requiere 2 parámetros\n");
		return ERROR;
	}
}

int comando_desbloquear(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
	{
		log_info(log_consola,"Se ingresó el comando Desbloquear\n");
		return DESBLOQUEAR;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Desbloquear, este requiere 1 parámetro\n");
		return ERROR;
	}
}

int comando_listar(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
	{
		log_info(log_consola,"Se ingresó el comando Listar\n");
		return LISTAR;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Listar, este requiere 1 parámetro\n");
		return ERROR;
	}
}

int comando_kill(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
	{
		log_info(log_consola,"Se ingresó el comando Kill\n");
		return KILL;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Kill, este requiere 1 parámetro\n");
		return ERROR;
	}
}

int comando_status(char** linea) {
	int tamanio = tamanio_array(linea);

	if (tamanio == 2)
	{
		log_info(log_consola,"ingresó el comando Status\n");
		return STATUS;
	}
	else
	{
		log_error(log_consola,"Error al ingresar comando Status, este requiere 1 parámetro\n");
		return ERROR;
	}
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
