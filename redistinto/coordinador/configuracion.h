#ifndef COORDINADOR_CONFIGURACION_H
#define COORDINADOR_CONFIGURACION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define m_puerto_escucha "puerto"
#define m_ip_coordinador "ip_coordinador"
#define m_algoritmo "algoritmo"
#define m_cantidad_entradas "cantidad_entradas"
#define m_tamanio_entrada "tamanio_entrada"
#define m_retardo "retardo"

int leer_configuracion();
void inicializar_configuracion();

#endif
