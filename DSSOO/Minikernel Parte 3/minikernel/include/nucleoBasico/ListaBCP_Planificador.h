/*
 *  ListaBCP_Planificador.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 07/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _LISTABCP_PLANIFICADOR_H
#define _LISTABCP_PLANIFICADOR_H

#include "nucleoBasico/const.h"
#include "tablasDinamicas/tablaDescFicheros.h"


/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/


/*
 * Definicion del tipo que corresponde con el BCP.
 * Se va a modificar al incluir la funcionalidad pedida.
 */
typedef struct BCP {
    int id;					/* ident. del proceso */
    int estado;				/* TERMINADO|LISTO|EJECUCION|BLOQUEADO*/
    int prioridad;
    contexto_t contextoRegs;/* copia de regs. de UCP */
	void * pila;			/* dir. inicial de la pila */
	struct BCP *siguiente;	/* puntero a otro BCP */
	void *infoMemoria;		/* descriptor del mapa de memoria */
	TipoTablaDescFicheros tablaDescFicheros; /* tablas de descriptores de ficheros */
	unsigned int ticksDespertar;
	unsigned int rodaja;
} TipoBCP;

/*
 * Definicion del tipo que corresponde con la cabecera de una lista
 * de BCPs. Este tipo se puede usar para diversas listas (procesos listos,
 * procesos bloqueados en sem·foro, etc.).
 */
typedef struct{
	TipoBCP *primero;
	TipoBCP *ultimo;
} TipoListaBCP;


/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/

/*
 * Variable global que representa la tabla de procesos
 */
extern TipoBCP tablaProcesos[];

/*
 * Variable global que identifica el proceso actual
 */
extern TipoBCP * procesoActual;

/*
 * Variable global que representa la cola de procesos listos
 */
extern TipoListaBCP listaListos;

/*
 * Variable global que representa la cola de procesos listos
 */
extern TipoListaBCP listaPrioridad;

/*
 * Variable global que representa la cola de procesos listos
 */
extern TipoListaBCP listaDurmiendo;

/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/*----------------------------------------------------------*
 * Funciones genericas para gestionar las listas de procesos
 *----------------------------------------------------------*/
 
/* 
 * Funcion que inicializa una lista de Procesos
 */ 
int inicializarListaBCP(TipoListaBCP *lista);

/* 
 * Funcion que obtiene el primer BCP de la lista
 * SIN BORRARLO de la lista. 
 * Si la lista esta vacia devuelve NULL
 */
TipoBCP *primeroListaBCP(TipoListaBCP *lista);

/* 
 * Funcion que elimina el primer BCP de la lista. 
 */
void eliminarPrimeroListaBCP(TipoListaBCP *lista);

/* 
 * Funcion que Inserta un BCP al final de la lista.
 */
void insertarUltimoListaBCP(TipoListaBCP *lista, TipoBCP * proceso);

/*
 * Funcion que inserta un BCP ordenadamente en una lista (atendiendo a ticksDespertar).
 */
void insertarOrdenadoListaBCP(TipoListaBCP *lista, TipoBCP * proceso);

/*
 * Funcion que inserta un BCP ordenadamente en una lista (atendiendo a prioridad).
 */
void insertarPrioridadesListaBCP(TipoBCP * proceso);

/* 
 * Función que muestra una lista de BCP
 */
int mostrarListaBCP(TipoListaBCP *lista, char *buffer, int bytesLibres);


/*----------------------------------------------*
 * Funciones para gestionar la tabla de procesos
 *----------------------------------------------*/
 
/* 
 * Funcion que inicia la tabla de procesos 
 */  
void iniciarTablaProcesos();  

/* 
 * Funcion que busca una entrada libre en la tabla de procesos 
 */
int buscarBCPLibreTablaProcesos();

/*
 * Funcion que muestra la tabla de procesos
 */
int mostrarTablaProcesos(char *buffer, int bytesLibres);


/*------------------------------------------------------*
 * Funciones para gestionar la planificacion de procesos
 *------------------------------------------------------*/

/* 
 * Funcion que devuelve el siguiente proceso a ejecutar (FIFO). 
 */
TipoBCP * planificador();

/* 
 * Funcion que inserta un proceso en la lista de listos. 
 */
void insertarListaListos(TipoBCP *proceso);

/* 
 * Funcion que inserta un proceso en la lista de prioridad. 
 */
void insertarListaListosAltaPrioridad(TipoBCP *proceso);

/*
 * Funcion de muestra la lista de listos
 */
int mostrarListaListos(char *buffer, int bytesLibres);

#endif /* _LISTABCP_PLANIFICADOR_H */


