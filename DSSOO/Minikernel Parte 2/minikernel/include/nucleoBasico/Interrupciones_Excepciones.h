/*
 *  Interrupciones_Excepciones.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _INTERRUPCIONES_EXCEPTIONES_H
#define _INTERRUPCIONES_EXCEPTIONES_H

#include "nucleoBasico/const.h"

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definicion del tipo que corresponde con una entrada en la lista de
 * interrupciones software
 */
typedef struct{
	void (*funcionTareaPend)(void *);
} TipoTareaPend;

/*
 * Definicion del tipo que representa a la lista de tareas pendientes 
 * de la interrupcion software, implementada como un bufer circular con
 * un tamanyo maximo de taras (MAX_TAREAS)
 */

typedef struct {
	TipoTareaPend tarea;
	void *datos;
} TipoTarea;

typedef struct{
	TipoTarea tablaTareas[MAX_TAREAS];
	int primero;
	int ultimo;
} TipoListaTareas;


/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/

/*
 * Variable global que representa el buffer de tareas pendientes de la 
 * interrupcion software.
 */

extern TipoListaTareas listaTareasPend;

/*
 * Variable global que contabiliza las interrupciones de reloj desde el inicio
 */
extern unsigned int ticks;


/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/*---------------------------------------------------------------------*
 * Funciones para gestionar la lista de tareas pendientes de la int. SW
 *---------------------------------------------------------------------*/

/* 
 * Inicializar el buffer de tareas pendientes de la int. sw. 
 */
void iniciarTareasPend(TipoListaTareas *lista);

/*
 * Inserta una tarea al final del buffer de tareas pendientes
 * de la interrupciÛn  software.
 */
void insertarUltimaTareaPend(TipoListaTareas *lista, void (*funcion)(void *), void *datos);

/* 
 * Obtener una tarea del principio de la lista de tareas pendientes de la int.  sw. 
 */
void obtenerPrimeraTareaPend(TipoListaTareas *lista, void (**funcion)(void *), void **datos);

/* 
 * Devuelve TRUE si hay tareas pendientes
 */
int hayTareasPend(TipoListaTareas *lista);

/*------------------------------------------------*
 * Manejadores de las interrupciones y excepciones
 *------------------------------------------------*/

/* Tratamiento de interrupciuones software */
void interrupcionSoftware();

/* Espera a que se produzca una interrupcion */
void esperaInterrupcion();

/* Manejador de excepciones aritmeticas */
void excepcionAritmetica();

/* Manejador de excepciones en el acceso a memoria */
void excepcionMemoria();

/* Manejador de interrupciones de de la tabla de Dispositivos */
void interrupcionDispositivos();

/* Manejador de interrupciones de reloj */
void interrupcionReloj();

/* Manejador de procesos dormidos */
void despertarDormidos();

/* Manejador de Round Robin */
void planificarRodaja();

#endif /* _INTERRUPCIONES_EXCEPCIONES_H */


