/*
 *  LlamadasSistema.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _LLAMADASSISTEMA_H
#define _LLAMADASSISTEMA_H

#include "nucleoBasico/const.h"
#include "nucleoBasico/ListaBCP_Planificador.h"


/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/
/*
 *
 * Definicion del tipo que corresponde con una entrada en la tabla de
 * llamadas al sistema.
 *
 */
typedef struct{
	int (*funServicio)();
} servicio;


/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/
 
/*
 * Variable global que contiene las rutinas que realizan cada llamada
 */
extern servicio tablaServicios[];

/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/*
 * Funcion principal a la que se llama para 
 * el tratamiento de cualquier llamada al sistema
 */
void tratarLlamadaSistema();

/*---------------------------------------------------*/

/* 
 * LLAMADAS AL SISTEMA: sis_crearProceso
 */
int sis_crearProceso();

/* 
 * LLAMADAS AL SISTEMA: sis_crearProceso
 */
int sis_crearProcesoAltaPrioridad();

/* 
 * Funcion auxiliar que crea un proceso reservando sus recursos 
 */
int crearTarea(char *prog, int prioridad);

/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_terminarProceso
 */
int sis_terminarProceso();

/* 
 * Funcion auxiliar que termina proceso actual liberando sus recursos 
 */
void liberarProceso();

/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_abrir
 */
int sis_abrir();

/* 
 * LLAMADA AL SISTEMA: sis_leer
 */
int sis_leer();

/* 
 * LLAMADA AL SISTEMA: sis_escribir
 */
int sis_escribir();

/* 
 * LLAMADA AL SISTEMA: sis_buscar
 */
int sis_buscar();

/* 
 * LLAMADA AL SISTEMA: sis_cerrar
 */
int sis_cerrar();

/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_montar
 */
int sis_montar();

/* 
 * LLAMADA AL SISTEMA: sis_sincronizar
 */
int sis_sincronizar();

/* 
 * LLAMADA AL SISTEMA: sis_desmontar
 */
int sis_desmontar();

/*
 * LLAMADA AL SISTEMA: sis_process_pid
 */
int sis_pid();

/*
 * LLAMADA AL SISTEMA: sis_process_time
 */
int sis_time();

/*
 * LLAMADA AL SISTEMA: sis_sleep
 */
int sis_sleep();

#endif /* _LLAMADASSISTEMA_H */


