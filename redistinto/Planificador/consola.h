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

void comando_continuar();
void comando_pausar();
void comando_bloquear(char* clave, int pid);
void comando_desbloquear(char* clave);
void comando_listar(char* recurso); //ACA NO ENTIENDO BIEN SI PIDE EL LISTADO DE ESIS QUE QUIEREN EJECUTAR X EJEMPLO
void comando_kill(int pid);
void comando_status();
void comando_deadlock();

#endif /* CONSOLA_H_ */
