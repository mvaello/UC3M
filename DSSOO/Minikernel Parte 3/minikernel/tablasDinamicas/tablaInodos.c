/*
 *  tablaInodos.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#include "minikernel.h"
#include "string.h"
#include "stdlib.h"


/***************************************************************
 * VARIABLES GLOBALES
 ***************************************************************/
/*
 * Variable global de la tabla de Inodos
 */
 
TipoInodo tablaInodos[MAX_TABLA_INODOS];
 
/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* ------------------------------------------------*
 * Funciones genericas de gestión del objeto Inodo
 * ------------------------------------------------*/

/*
 * Funcion que inicializa un nuevo inodo 
 * en la tabla de inodos
 */
int iniciarNuevoInodo(int descInodo, char *nombre, int flags)
{
	int ret;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == nombre) ||
		 (NULL == tablaInodos[descInodo].iniciarNuevoInodo) ) {
		return (-1);
	}
	ret = tablaInodos[descInodo].iniciarNuevoInodo(descInodo,nombre, flags);
	return (ret);
}
	
/*
 * Funcion que lee un objeto Inodo del volumen
 */
int leerInodo(int descInodo)
{
	int ret;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == tablaInodos[descInodo].leerInodo) ) {
		return (-1);
	}
	ret = tablaInodos[descInodo].leerInodo(descInodo);
	return (ret);
}

/*
 * Funcion que localiza el bloque de disco 
 * de un bloque del fichero
 */
int mapearBloque(int descInodo, int numBloqueArchivo)
{
	int numBloque;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == tablaInodos[descInodo].mapearBloque) ) {
		return (-1);
	}
	numBloque = tablaInodos[descInodo].mapearBloque(descInodo,numBloqueArchivo);
	return (numBloque);
}


/*
 * Funcion que reserva y asigna un nuevo bloque de disco 
 * para el bloque de fichero correspondiente
 */
int asignarNuevoBloque(int descInodo, int numBloqueArchivo)
{
	int numBloque;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == tablaInodos[descInodo].asignarNuevoBloque) ) {
		return (-1);
	}
	numBloque = tablaInodos[descInodo].asignarNuevoBloque(descInodo,numBloqueArchivo);
	return (numBloque);
}

/*
 * Funcion que busca el primer objeto Inodo
 * que corresponde con el nombre dentro del mismo volumen.
 */
int buscarNombreEnDir(int descInodo, char *nombre)
{
	int descInodoResult;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == nombre) ||
		 (NULL == tablaInodos[descInodo].buscarNombreEnDir) ) {
		return (-1);
	}
	descInodoResult = tablaInodos[descInodo].buscarNombreEnDir(descInodo,nombre);
	return (descInodoResult);
}

/*
 * Funcion que escribe un objeto Inodo en 
 * el volumen
 */
int escribirInodo(int descInodo)
{
	int ret;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == tablaInodos[descInodo].escribirInodo) ) {
		return (-1);
	}
	ret = tablaInodos[descInodo].escribirInodo(descInodo);
	return (ret);
}

/*
 * Funcion que borrar un objeto Inodo y lo
 * elimina de la tabla
 */
int borrarInodo(int descInodo)
{
	int ret;
	
	if ( (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ||
		 (NULL == tablaInodos[descInodo].borrarInodo) ) {
		return (-1);
	}
	ret = tablaInodos[descInodo].borrarInodo(descInodo);
	return (ret);
}

/*
 * Funcion que muestra los componentes de un inodo
 */
int mostrarInodo(int descInodo, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;

	if ( (NULL == buffer) || (0 >= bytesLibres) ||
		 (0 > descInodo) || (MAX_TABLA_INODOS <= descInodo) ) {
		return (0);
	}
	
	/* mostart componentes generales */
	libres=sprintk_concat(buffer, libres,"INODO:\tID=%d, montaje=%d, inodo_volumen=%d, bloque=%d\n",
						  descInodo, tablaInodos[descInodo].descMontaje, 
						  tablaInodos[descInodo].numInodo,
						  tablaInodos[descInodo].numBloque); 	
		
	/*mostrar componentes especificos */
	if (NULL != tablaInodos[descInodo].mostrarInodo) {
		libres=tablaInodos[descInodo].mostrarInodo(descInodo, buffer, libres);
	}
	return (libres);
}


/* -------------------------------------------*
 * Funciones de gestión de la tabla de Inodos
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Inodos
 */
int inicializarTablaInodos()
{
	int i;
	
	for (i=0;i<MAX_TABLA_INODOS;i++) {
		tablaInodos[i].ocupado = FALSE;
		tablaInodos[i].descMontaje = -1;
		tablaInodos[i].numInodo = -1;
		tablaInodos[i].numBloque = -1;
		tablaInodos[i].datosPropiosInodo = NULL;
		tablaInodos[i].iniciarNuevoInodo = NULL;
		tablaInodos[i].leerInodo = NULL;
		tablaInodos[i].mapearBloque = NULL;
		tablaInodos[i].asignarNuevoBloque = NULL;
		tablaInodos[i].buscarNombreEnDir = NULL;
		tablaInodos[i].escribirInodo = NULL;
		tablaInodos[i].borrarInodo = NULL;
	}
	return (0);
}
	
/*
 * Funcion que reserva una entrada en la tabla de 
 * de Inodos para dicho numero de inodo y volumen
 */
int reservarEntradaTablaInodos(int descMontaje, int numInodo)
{
	int descInodo = 0;

	while ( (TRUE == tablaInodos[descInodo].ocupado) &&
			(MAX_TABLA_INODOS > descInodo) ) {
		descInodo = descInodo + 1;
	}
	if (MAX_TABLA_INODOS > descInodo) {
		tablaInodos[descInodo].ocupado = TRUE;
		tablaInodos[descInodo].descMontaje = descMontaje;
		tablaInodos[descInodo].numInodo = numInodo;
		return (descInodo);
	}
	return (-1);
}

/*
 * Funcion que Busca un Inodo en la tabla de Inodos
 * por el numero de inodo y el volumen.
 */
int buscarEntradaTablaInodos(int descMontaje, int numInodo)
{
	int descInodo = 0;

	while ( ( (FALSE == tablaInodos[descInodo].ocupado) || 
		      (descMontaje != tablaInodos[descInodo].descMontaje) ||
			  (numInodo != tablaInodos[descInodo].numInodo) ) &&
			(MAX_TABLA_INODOS > descInodo) ) {
		descInodo = descInodo + 1;
	}
	if (MAX_TABLA_INODOS <= descInodo) {
		return (-1);
	} else {
		return (descInodo);
	}
}


/*
 * Funcion que muestra la tabla de Inodos
 */
int mostrarTablaInodos(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	libres=sprintk_concat(buffer, libres, "TABLA DE INODOS\n"); 

	for (i=0; i<MAX_TABLA_INODOS; i++) {
		if (TRUE == tablaInodos[i].ocupado) {
			libres=mostrarInodo(i, buffer, libres);
		}
	}
	return (libres);
}


/*
 * Funcion que libera una driver de la tabla de Inodos
 */
int liberarEntradaTablaInodos(int descInodo)
{
	tablaInodos[descInodo].ocupado = FALSE;
	tablaInodos[descInodo].descMontaje = -1;
	tablaInodos[descInodo].numInodo = -1;
	tablaInodos[descInodo].numBloque = -1;
	free(tablaInodos[descInodo].datosPropiosInodo);
	tablaInodos[descInodo].datosPropiosInodo = NULL;
	tablaInodos[descInodo].iniciarNuevoInodo = NULL;
	tablaInodos[descInodo].leerInodo = NULL;
	tablaInodos[descInodo].mapearBloque = NULL;
	tablaInodos[descInodo].asignarNuevoBloque = NULL;
	tablaInodos[descInodo].buscarNombreEnDir = NULL;
	tablaInodos[descInodo].escribirInodo = NULL;
	tablaInodos[descInodo].borrarInodo = NULL;
	return (0);
}

