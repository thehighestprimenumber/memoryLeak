/*
 * consola.h
 *
 *  Created on: 23 abr. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <commons/string.h>
#include "logger_planificador.h"

typedef enum tipoConsola{CONTINUAR,PAUSAR,BLOQUEAR,DESBLOQUEAR,LISTAR,KILL,STATUS,ERROR} tipoConsola;

char comando_consola[1000];
char** list_comandos;

bool flag_puede_ejecutar;
bool flag_comando_pendiente;

int leer_consola();
int decodificar_comando();

int comando_continuar(char** linea);
int comando_pausar(char** linea);
int comando_bloquear(char** linea);
int comando_desbloquear(char** linea);
int comando_listar(char** linea); //ACA NO ENTIENDO BIEN SI PIDE EL LISTADO DE ESIS QUE QUIEREN EJECUTAR X EJEMPLO
int comando_kill(char** linea);
int comando_status(char** linea);

char* limpiar_comando(char* comando);
int tamanio_array(char** cadena);
void liberar_split(char** array);

#endif /* CONSOLA_H_ */
