/*
 *  listaCaracteres.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <string.h>
#include "minikernel.h"


/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* ----------------------------------------------*
 * Funciones de gestión de la lista de peticiones
 * ----------------------------------------------*/

/* 
 * Funcion auxiliar para inicializar la lista de peticiones 
 */
int iniciarListaPeticiones(TipoListaPeticiones *lista)
{
	lista->primero = NULL;
	lista->ultimo = NULL;
	return (0);
}


/* 
 * Funcion auxiliar para insertar una peticion al final de la lista 
 */
int insertarListaPeticiones(TipoListaPeticiones *lista, TipoPeticion *peticion)
{
	if (NULL == lista->primero) {
		lista->primero = peticion;
	} else {
		lista->ultimo->siguiente = peticion;
	}
	lista->ultimo = peticion;
	peticion->siguiente = NULL;
	return (0);
}

/* 
 * Funcion auxiliar para obtener una peticion del principio de la lista 
 */
TipoPeticion *extraerListaPeticiones(TipoListaPeticiones *lista)
{
	TipoPeticion *peticion;
	
	peticion = lista->primero;
	if (NULL != lista->primero) {
		if (lista->ultimo==lista->primero) {
			lista->primero=NULL;
			lista->ultimo=NULL;
		} else { 
			lista->primero=lista->primero->siguiente;
		}
	}
	return (peticion);
}

/* 
 * Funcion auxiliar para busca una peticion al mismo bloque en la lista 
 */
TipoPeticion *buscarListaPeticiones(TipoListaPeticiones *lista, int numBloque)
{
	TipoPeticion *peticionAux=NULL; 

	if (NULL != lista->primero) {
		peticionAux = lista->primero;
		while ( (NULL != peticionAux) && 
				(numBloque != peticionAux->numBloque) ) {
			peticionAux = peticionAux->siguiente;
		}
	}
	return (peticionAux);
}

/*
 * Funcion que muestra los componentes de la lista
 */
int mostrarListaPeticiones(TipoListaPeticiones *lista, char *buffer, int bytesLibres)
{
	TipoPeticion *peticionAux=NULL; 
	int libres=bytesLibres;

	if ( (NULL == lista) || (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}

    libres=sprintk_concat(buffer, libres,"LISTA PETICIONES\n");

	if (NULL != lista->primero) {
		peticionAux = lista->primero;
		while (NULL != peticionAux) {
		    libres=sprintk_concat(buffer, libres,
								  "PETICION: dispositivo=%d, numBloque=%d,operacion=%d,numpeticiones=%d\n",
								  peticionAux->descDispositivo, peticionAux->numBloque,
								  peticionAux->operacion, peticionAux->numPeticiones);
			libres=sprintk_concat(buffer, libres,"Tabla de bloqueados de la peticion\n");
			libres=mostrarListaBCP(&(peticionAux->listaProcesosBloqueados), buffer, libres);
			peticionAux = peticionAux->siguiente;
		}
	}
	return (libres);
}

/* 
 * Funcion para ver si hay peticiones en la lista 
 */
int estaVaciaListaPeticiones(TipoListaPeticiones *lista) 
{
	return (NULL == lista->primero);
}

