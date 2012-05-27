/*
 *  Interrupciones_Excepciones.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "minikernel.h"


/***************************************************************
 * VARIABLES GLOBALES
 ***************************************************************/

/*
 * Variable global que representa el buffer de tareas pendientes de la 
 * interrupcion software.
 */

TipoListaTareas listaTareasPend;

/*
 * Variable global que contabiliza las interrupciones de reloj desde el inicio
 */
unsigned int ticks = 0;

/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/*---------------------------------------------------------------------*
 * Funciones para gestionar la lista de tareas pendientes de la int. SW
 *---------------------------------------------------------------------*/

/*
 * Inicializar el buffer de tareas pendientes
 * de la interrupciÛn  software.
 */
void iniciarTareasPend(TipoListaTareas *lista)
{
	lista->primero = 0;
	lista->ultimo = 0;
}
/*
 * Inserta una tarea al final del buffer de tareas pendientes
 * de la interrupciÛn  software.
 */
void insertarUltimaTareaPend(TipoListaTareas *lista, void (*funcion)(void *), void *datos)
{
	if (lista->primero == (lista->ultimo+1) % MAX_TAREAS) { 
		/* si el buffer esta lleno, error */
		printk("-> ERROR: LISTA TAREAS PENDIENTE LLENA, SE DESCARTA\n");
	} else { 
		/* cargar la entrada y apuntar que hay uno mas */
		lista->tablaTareas[lista->ultimo].tarea.funcionTareaPend = funcion;
		lista->tablaTareas[lista->ultimo].datos = datos;
		lista->ultimo = (lista->ultimo+1) % MAX_TAREAS;
	}
}

/*
 * Obtener una tarea del principio de la lista de tareas pendientes
 * de la interrupcion  software.
 */
void obtenerPrimeraTareaPend(TipoListaTareas *lista, void (**funcion)(void *), void **datos)
{
	if (lista->primero == lista->ultimo) { 
		/* si el buffer esta vacio, error */
		printk("-> ERROR: LISTA TAREAS PENDIENTE VACIA\n");
	} else { 
		/* obtener la entrada y apuntar que hay uno menos */
		(*funcion) = lista->tablaTareas[lista->primero].tarea.funcionTareaPend;
		(*datos) = lista->tablaTareas[lista->primero].datos;
		lista->primero = (lista->primero+1) % MAX_TAREAS;
	}
}

/* 
 * Devuelve TRUE si hay tareas pendientes
 */
int hayTareasPend(TipoListaTareas *lista)
{
	return (lista->primero != lista->ultimo);
}

/*------------------------------------------------*
 * Manejadores de las interrupciones y excepciones
 *------------------------------------------------*/

/*
 * Tratamiento de interrupciuones software
 */
void interrupcionSoftware()
{
	void (*funcion)(void *);
	void *datos = NULL;

	//printk("-> TRATANDO INT. SW\n");
	while (hayTareasPend(&(listaTareasPend)) ) {

		/* mientras haya tareas en el buffer, ejecutarlas */
		obtenerPrimeraTareaPend(&(listaTareasPend), &(funcion), &(datos));
		funcion (datos);
	}	
	return;
}


/*
 * Espera a que se produzca una interrupcion
 */
void esperaInterrupcion()
{
	int nivel;

	//printk("-> NO HAY LISTOS. ESPERA INT\n");

	/* Baja al mÌnimo el nivel de interrupciÛn mientras espera */
	nivel=fijar_nivel_int(NIVEL_1);
	/* 
	 * Como no se pueden desinhibir totalmente las int 
	 * si hay alguna int. sw. pendiente la hacemos en lugar de parar
	 */
	if (hayTareasPend(&(listaTareasPend)) ) {
		interrupcionSoftware();
	} else {
		//printk("-> EJECUTANDO HALT\n");
		halt();
	}
	fijar_nivel_int(nivel);
}


/*
 *
 * Manejadores de interrupciones y excepciones genericas
 *	
 */

/*
 * Manejador de excepciones aritmeticas
 */
void excepcionAritmetica()
{
	if (!viene_de_modo_usuario())
		panico("excepcion aritmetica cuando estaba dentro del kernel");


	printk("-> EXCEPCION ARITMETICA EN PROC %d\n", procesoActual->id);
	liberarProceso();

        return; /* no deberÌa llegar aqui */
}

/*
 * Manejador de excepciones en el acceso a memoria
 */
void excepcionMemoria(){

	if (!viene_de_modo_usuario())
		panico("excepcion de memoria cuando estaba dentro del kernel");


	printk("-> EXCEPCION DE MEMORIA EN PROC %d\n", procesoActual->id);
	liberarProceso();

        return; /* no deberÌa llegar aqui */
}

/*
 * Manejador de interrupciones para toda la tabla de dispositivos
 */
void interrupcionDispositivos()
{
	int i;

	//printk("-> INTERRUPCION DE DISPOSITIVOS\n");
	for (i=0; i<MAX_TABLA_DISPOSITIVOS; i++) {
		if  ( (TRUE == tablaDispositivos[i].ocupado) && 
			  (NULL != tablaDispositivos[i].interrupcionHW) ) {
			tablaDispositivos[i].interrupcionHW(i);
		}
	}
	return;
}


/*
 * Manejador de interrupciones de reloj
 */
void interrupcionReloj()
{
	//Poner al máximo el nivel de prioridad de interrupción.
	int nivel = fijar_nivel_int(NIVEL_3);

	//printk("-> TRATANDO INT. DE RELOJ\n");
	ticks = ticks + 1;

	//Insertar interrupción de Reloj.
	insertarUltimaTareaPend(&(listaTareasPend), despertarDormidos, 0);

	//Generar interrupción software.
	interrupcionSoftware();

	//Restaurar el nivel de prioridad de interrupción previo.
	fijar_nivel_int(nivel);

	//Insertar interrupción de Round-Robin.
	insertarUltimaTareaPend(&(listaTareasPend), planificarRodaja, 0);

	//Generar interrupción software.
	interrupcionSoftware();

    return;
}

void despertarDormidos() {
	//Cogemos el primer proceso dormido.
	TipoBCP *proceso = primeroListaBCP(&listaDurmiendo);

	while(proceso != NULL && proceso->ticksDespertar < ticks) {
		//Desbloqueamos el proceso.
		proceso->estado = LISTO;

		//Eliminamos el proceso de la lista de dormidos.
		eliminarPrimeroListaBCP(&listaDurmiendo);

		//Insertamos el proceso a la lista de procesos listos.
		//PRE-ROUNDROBIN -> insertarUltimoListaBCP(&listaListos, proceso);
		insertarPrioridadesListaBCP(proceso);

		//Cogemos el siguiente proceso.
		proceso = primeroListaBCP(&listaDurmiendo);
	}

	return;
}

void planificarRodaja() {
	//Comprobar si hay algún proceso ejecutándose.
	if(procesoActual != NULL) {
		procesoActual->rodaja = procesoActual->rodaja + 1;

		//Comprobar si el proceso actual ha cumplido su rodaja de tiempo.
		if(procesoActual->rodaja >= TICKS_POR_RODAJA) {
			//Se reinicia la rodaja y el proceso pasa de EJECUCION a LISTO.
			procesoActual->rodaja = 0;
			procesoActual->estado = LISTO;

			//Insertamos el proceso en la lista de prioridades y lo guardamos en una variable auxiliar.
			insertarPrioridadesListaBCP(procesoActual);
			TipoBCP *procesoAux = procesoActual;

			//Pedimos el siguiente proceso al planificador y cambiamos su estado a EJECUCION.
			procesoActual = planificador();
			procesoActual->estado = EJECUCION;

			//Hacemos un cambio de contexto para guardar los registros del proceso actual y coger los del proceso pedido al planificador.
			cambio_contexto(&(procesoAux->contextoRegs), &(procesoActual->contextoRegs));
		}
	}

	return;
}

