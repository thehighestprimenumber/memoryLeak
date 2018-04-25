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

void* abrir_consola();
void procesar_funcion(char* linea);

void comando_continuar(char** linea);
void comando_pausar(char** linea);
void comando_bloquear(char** linea);
void comando_desbloquear(char** linea);
void comando_listar(char** linea); //ACA NO ENTIENDO BIEN SI PIDE EL LISTADO DE ESIS QUE QUIEREN EJECUTAR X EJEMPLO
void comando_kill(char** linea);
void comando_status(char** linea);
void comando_deadlock(char** linea);

char* limpiar_comando(char* comando);
int tamanio_array(char** cadena);
void liberar_split(char** array);

#endif /* CONSOLA_H_ */
