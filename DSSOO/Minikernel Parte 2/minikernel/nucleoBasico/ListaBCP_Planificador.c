/*
 *  ListaBCP_Planificador.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "minikernel.h"
#include <string.h>

/***************************************************************
 * VARIABLES GLOBALES
 ***************************************************************/

/*
 * Variable global que representa la tabla de procesos
 */
TipoBCP tablaProcesos[MAX_PROCESOS];

/*
 * Variable global que identifica el proceso actual
 */
TipoBCP * procesoActual = NULL;

/*
 * Variable global que representa la cola de procesos listos
 */
TipoListaBCP listaListos = {NULL, NULL};

/*
 * Variable global que representa la cola de procesos listos con prioridad
 */
TipoListaBCP listaPrioridad = {NULL, NULL};

/*
 * Variable global que representa la cola de procesos listos
 */
TipoListaBCP listaDurmiendo = {NULL, NULL};


/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/*----------------------------------------------------------*
 * Funciones genericas para gestionar las listas de procesos
 *----------------------------------------------------------*/
 
/* 
 * Funcion que inicializa una lista de Procesos
 */ 
int inicializarListaBCP(TipoListaBCP *lista)
{
	lista->primero = NULL;
	lista->ultimo = NULL;
	return (0);
}

/* 
 * Funcion que obtiene el primer BCP de la lista
 * SIN BORRARLO de la lista. 
 * Si la lista esta vacia devuelve NULL
 */
TipoBCP *primeroListaBCP(TipoListaBCP *lista)
{
	return (lista->primero);
}

/*
 * Funcion que elimina el primer BCP de la lista. 
 */
void eliminarPrimeroListaBCP(TipoListaBCP *lista)
{
	if (NULL != lista->primero) {
		if (lista->ultimo==lista->primero) {
			lista->primero=NULL;
			lista->ultimo=NULL;
		} else { 
			lista->primero=lista->primero->siguiente;
		}
	}
}

/*
 * Funcion que inserta un BCP al final de la lista.
 */
void insertarUltimoListaBCP(TipoListaBCP *lista, TipoBCP * proceso)
{
	if (NULL == lista->primero) {
		lista->primero = proceso;
	} else {
		lista->ultimo->siguiente = proceso;
	}
	lista->ultimo = proceso;
	proceso->siguiente = NULL;
}

/*
 * Funcion que inserta un BCP ordenadamente en una lista (atendiendo a ticksDespertar).
 */
void insertarOrdenadoListaBCP(TipoListaBCP *lista, TipoBCP * proceso)
{
	if(lista->primero == NULL) {
		//Al no haber ningún proceso en la lista, apuntamos el último y el primero al proceso a insertar.
		lista->primero = proceso;
		lista->ultimo = proceso;
		proceso->siguiente = NULL;
	}
	else if(lista->primero->ticksDespertar > proceso->ticksDespertar) {
		//Si el proceso a insertar termina antes que el primer proceso de la lista, hacemos que el proceso a insertar sea el primero.
		proceso->siguiente = lista->primero;
		lista->primero = proceso;
	}
	else if(lista->ultimo->ticksDespertar <= proceso->ticksDespertar) {
		//Si el proceso a insertar temina después del último proceso de la lista, hacemos que el proceso a insertar sea el último.
		lista->ultimo->siguiente = proceso;
		lista->ultimo = proceso;
		proceso->siguiente = NULL;
	}
	else {
		//Guardamos en un puntero el proceso desde el primero, avanzando hacia el siguiente para averiguar dónde hay que insertar el proceso.
		//Una vez encontremos un proceso que termina después del proceso a insertar, lo introducimos entre él y el anterior.
		TipoBCP *procesoAux = lista->primero;
		while(procesoAux->siguiente->ticksDespertar <= proceso->ticksDespertar) {
			procesoAux = procesoAux->siguiente;
		}

		proceso->siguiente = procesoAux->siguiente;
		procesoAux->siguiente = proceso;
	}
}

/*
 * Funcion que inserta un BCP ordenadamente en una lista (atendiendo a prioridad).
 */
void insertarPrioridadesListaBCP(TipoBCP * proceso)
{
	//Comprobamos si el proceso tiene prioridad de ejecución alta.
	if(proceso->prioridad == PRIORIDAD_ALTA) {
		insertarListaListosAltaPrioridad(proceso);
	}
	else {
		insertarListaListos(proceso);
	}
}

/*
 * Función que muestra una lista de BCP
 */
int mostrarListaBCP(TipoListaBCP *lista, char *buffer, int bytesLibres)
{
	TipoBCP *procesoAux=NULL; 
	int libres=bytesLibres;

	if ( (NULL == buffer) || (0 >= bytesLibres) ){
		return (0);
	}
	
	if (NULL == lista->primero) {
		libres=sprintk_concat(buffer, libres,
						   "lista vacia: p=%p, d=%p\n",lista->primero,lista->ultimo); 
	} else {
		libres=sprintk_concat(buffer, libres, 
						   "lista: p=%d, d=%d: ",lista->primero->id,lista->ultimo->id); 
		procesoAux = lista->primero;
		while (NULL != procesoAux) {
			libres=sprintk_concat(buffer, libres, "%d, ",procesoAux->id); 
			procesoAux = procesoAux->siguiente;
		}
		libres=sprintk_concat(buffer, libres, "\n");
	}
	return (libres);
}


/*----------------------------------------------*
 * Funciones para gestionar la tabla de procesos
 *----------------------------------------------*/
 
/* 
 * Funcion que inicia la tabla de procesos 
 */  
void iniciarTablaProcesos()
{
	int i;

	for (i=0; i<MAX_PROCESOS; i++) {
		tablaProcesos[i].estado=NO_USADA;
	}
}

/*
 * Funcion que busca una entrada libre en la tabla de procesos
 */

int buscarBCPLibreTablaProcesos()
{
	int i;

	for (i=0; i<MAX_PROCESOS; i++)
		if (tablaProcesos[i].estado==NO_USADA)
			return i;
	return -1;
}

/*
 * Funcion que muestra la tabla de procesos
 */
int mostrarTablaProcesos(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;

	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	libres=sprintk_concat(buffer, libres, "TABLA DE PROCESOS\n"); 

	for (i=0; i<MAX_PROCESOS; i++) {
		if (NO_USADA != tablaProcesos[i].estado) {
			libres=sprintk_concat(buffer, libres,
							"ID=%d, estado=%d\n",tablaProcesos[i].id, tablaProcesos[i].estado); 	
			libres=mostrarTablaDescFicheros(tablaProcesos[i].tablaDescFicheros, buffer, libres);
		}
	}
	return (libres);
}


/*------------------------------------------------------*
 * Funciones para gestionar la planificacion de procesos
 *------------------------------------------------------*/

/*
 * Funcion de planificacion que implementa un algoritmo FIFO.
 */
TipoBCP * planificador()
{
	TipoBCP * procesoAux;

	//Obtener el primer proceso con prioridad.
	procesoAux = primeroListaBCP(&listaPrioridad);
	if(procesoAux != NULL) {
		eliminarPrimeroListaBCP(&listaPrioridad);
	}
	else {
		procesoAux = primeroListaBCP(&listaListos);	/*obtener el primero proceso listo */
		if (NULL != procesoAux) {
			eliminarPrimeroListaBCP(&listaListos);		/* proc. fuera de listos */
		}
	}

	return (procesoAux);
}

/*
 * Funcion de planificacion que implementa un algoritmo FIFO.
 */
void insertarListaListos(TipoBCP *proceso)
{
	insertarUltimoListaBCP(&listaListos, proceso); 
	return;
}

/*
 * Funcion de planificacion que implementa un algoritmo FIFO.
 */
void insertarListaListosAltaPrioridad(TipoBCP *proceso)
{
	insertarUltimoListaBCP(&listaPrioridad, proceso); 
	return;
}

/*
 * Funcion de muestra la lista de listos
 */
int mostrarListaListos(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}

	/* mostrar lista listos */
	libres=sprintk_concat(buffer, libres, "LISTA DE PROCESOS LISTOS\n"); 
    libres = mostrarListaBCP(&(listaListos), buffer, libres);

	return (libres);
}

