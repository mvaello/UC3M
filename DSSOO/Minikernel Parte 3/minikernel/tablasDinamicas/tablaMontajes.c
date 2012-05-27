/*
 *  tablaMontajes.c
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
 * Variable global de la tabla de Montajes
 */
 
TipoMontaje tablaMontajes[MAX_TABLA_MONTAJES];
 
/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* -------------------------------------------------*
 * Funciones genericas de gestión del objeto Montaje
 * -------------------------------------------------*/

/*
 * Funcion que monta un volumen formateado con  
 * un sistema de ficheros
 */
int montarVolumen(char *nombreDispositivo, char *nombreMontaje, char *nombreFS)
{
	int descFS;
	int descMontaje;
	int ret;
	
	if ( (NULL == nombreDispositivo) || (NULL == nombreMontaje) || (NULL == nombreFS) ) {
		return (-1);
	}

	/*buscar FS en la tabla */
	descFS = buscarEntradaTablaFS(nombreFS);
	if (-1 == descFS) {
		/* ejecutar la funcion de carga del modulo correspondiente */
		printk("Cargando modulo %s\n", nombreFS);
		ret = cargar_modulo(nombreFS);
		if (-1 == ret) {
			printk("Error: modulo %s no encontrado\n", nombreFS);
			return (-1);
		}

		/*repetir la busqueda despues de cargar el modulo */
		descFS = buscarEntradaTablaFS(nombreFS);
		if (-1 == descFS) {
			printk("Error: Sistema de ficheros no encontrado\n");
			return (-1);
		}
	}

	descMontaje = crearMontaje(descFS, nombreMontaje);
	if ( (-1 == descMontaje) || (NULL == tablaMontajes[descMontaje].montarVolumen) ) {
		return (-1);
	}
	
	ret = tablaMontajes[descMontaje].montarVolumen(descMontaje, nombreDispositivo);
	return (ret);
}
/*
 * Funcion que reserva un nuevo bloque en el volumen 
 */
int reservarBloque(int descMontaje)
{
	int numBloque;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == tablaMontajes[descMontaje].reservarBloque) ) {

			return (-1);
	}
	numBloque = tablaMontajes[descMontaje].reservarBloque(descMontaje);
	return (numBloque);
}

/*
 * Funcion que libera un bloque del volumen 
 */
int liberarBloque(int descMontaje, int numBloque)
{
	int ret;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == tablaMontajes[descMontaje].liberarBloque) ) {

			return (-1);
	}
	ret = tablaMontajes[descMontaje].liberarBloque(descMontaje, numBloque);
	return (ret);
}

/*
 * Funcion que reserva un nuevo inodo en el volumen 
 */
int reservarInodo(int descMontaje)
{
	int numInodo;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == tablaMontajes[descMontaje].reservarInodo) ) {

			return (-1);
	}
	numInodo = tablaMontajes[descMontaje].reservarInodo(descMontaje);
	return (numInodo);
}

/*
 * Funcion que libera un nuevo inodo del volumen 
 */
int liberarInodo(int descMontaje, int numInodo)
{
	int ret;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == tablaMontajes[descMontaje].liberarInodo) ) {

			return (-1);
	}
	ret = tablaMontajes[descMontaje].liberarInodo(descMontaje, numInodo);
	return (ret);
}
/*
 * Funcion que busca el inodo con el nombre correspondiente 
 * del volumen en la tabla de Inodos
 */
int buscarNombreInodo(int descMontaje, char *nombre)
{
	int descInodo;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == nombre) ||
		 (NULL == tablaMontajes[descMontaje].buscarNombreInodo) ) {

			return (-1);
	}
	descInodo = tablaMontajes[descMontaje].buscarNombreInodo(descMontaje, nombre);
	return (descInodo);
}

/*
 * Funcion que crear un nuevo Inodo vacio 
 * en la tabla de Inodos
 */
int crearInodo(int descMontaje, int numInodo)
{
	int descInodo;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == tablaMontajes[descMontaje].crearInodo) ) {

			return (-1);
	}
	descInodo = tablaMontajes[descMontaje].crearInodo(descMontaje, numInodo);
	return (descInodo);
}

/*
 * Funcion que crear un nuevo descriptor de fichero vacio 
 * en la tabla de descriptores de ficheros del proceso
 */

int crearDescFichero(int descMontaje, TipoTablaDescFicheros tablaDescFicheros)
{
	int descFichero;
	
	if ( (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ||
		 (NULL == tablaMontajes[descMontaje].crearDescFichero) ) {

			return (-1);
	}
	descFichero = tablaMontajes[descMontaje].crearDescFichero(descMontaje, tablaDescFicheros);
	return (descFichero);
}

/*
 * Funcion que escribe el volumen todos los cambios
 * producidos hasta ahora
 */
int sincronizarVolumen(char *nombreMontaje)
{
	int descMontaje;
	int ret;
	
	if (NULL == nombreMontaje) {
		return (-1);
	}

	descMontaje = buscarEntradaTablaMontajes(nombreMontaje);
	if ( (-1 == descMontaje) || (NULL == tablaMontajes[descMontaje].sincronizarVolumen) ) {
		return (-1);
	}

	ret=tablaMontajes[descMontaje].sincronizarVolumen(descMontaje);
	return (ret);
}

/*
 * Funcion que desmonta el volumen y destruye el 
 * objeto Montaje y lo elimina de la tabla
 */
int desmontarVolumen(char *nombreMontaje)
{
	int descMontaje;
	int ret;
	
	if (NULL == nombreMontaje) {
		return (-1);
	}

	descMontaje = buscarEntradaTablaMontajes(nombreMontaje);
	if ( (-1 == descMontaje) || (NULL == tablaMontajes[descMontaje].desmontarVolumen) ) {
		return (-1);
	}

	ret=tablaMontajes[descMontaje].desmontarVolumen(descMontaje);
	return (ret);
}

/*
 * Funcion que muestra los componentes de un Montaje
 */
int mostrarMontaje(int descMontaje, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;

	if ( (NULL == buffer) || (0 >= bytesLibres) ||
		 (0 > descMontaje) || (MAX_TABLA_MONTAJES <= descMontaje) ) {
		return (0);
	}
	
	/* mostart componentes generales */
	libres=sprintk_concat(buffer, libres, "MONTAJE:\tID=%d, nombre=%s, FS=%d, dispositivo=%d\n",
						  descMontaje, tablaMontajes[descMontaje].nombreMontaje, 
						  tablaMontajes[descMontaje].descFS,
						  tablaMontajes[descMontaje].descDispositivo); 	
		
	/*mostrar componentes especificos */
	if (NULL != tablaMontajes[descMontaje].mostrarMontaje) {
		libres=tablaMontajes[descMontaje].mostrarMontaje(descMontaje, buffer, libres);
	}
	return (libres);
}


/* -------------------------------------------*
 * Funciones de gestión de la tabla de Montajes
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Montajes
 */
int inicializarTablaMontajes()
{
	int i;
	
	for (i=0;i<MAX_TABLA_MONTAJES;i++) {
		tablaMontajes[i].ocupado = FALSE;
		strcpy(tablaMontajes[i].nombreMontaje,"");
		tablaMontajes[i].descFS = -1;
		tablaMontajes[i].descDispositivo = -1;
		tablaMontajes[i].datosPropiosMontaje = NULL;
		tablaMontajes[i].montarVolumen = NULL;
		tablaMontajes[i].reservarBloque = NULL;
		tablaMontajes[i].liberarBloque = NULL;
		tablaMontajes[i].reservarInodo = NULL;
		tablaMontajes[i].liberarInodo = NULL;
		tablaMontajes[i].buscarNombreInodo = NULL;
		tablaMontajes[i].crearInodo = NULL;
		tablaMontajes[i].crearDescFichero = NULL;
		tablaMontajes[i].sincronizarVolumen = NULL;
		tablaMontajes[i].desmontarVolumen = NULL;
	}
	return (0);
}
	
/*
 * Funcion que reserva una entrada en la tabla de 
 * de Montajes para el objeto "nombreMontaje"
 */
int reservarEntradaTablaMontajes(char *nombreMontaje)
{
	int descMontaje = 0;

	if (NULL == nombreMontaje) {
		return (-1);
	}
	while ( (TRUE == tablaMontajes[descMontaje].ocupado) &&
			(MAX_TABLA_MONTAJES > descMontaje) ) {
		descMontaje = descMontaje + 1;
	}
	if (MAX_TABLA_MONTAJES > descMontaje) {
		tablaMontajes[descMontaje].ocupado = TRUE;
		strcpy(tablaMontajes[descMontaje].nombreMontaje,nombreMontaje);
		return (descMontaje);
	}
	return (-1);
}

/*
 * Funcion que Busca un Montaje en la tabla de Montajes
 * por el nombre.
 */
int buscarEntradaTablaMontajes(char *nombreMontaje)
{
	int descMontaje = 0;

	if (NULL == nombreMontaje) {
		return (-1);
	}
	while ( ( (FALSE == tablaMontajes[descMontaje].ocupado) || 
		      (0 != strcmp(tablaMontajes[descMontaje].nombreMontaje,nombreMontaje)) ) &&
			(MAX_TABLA_MONTAJES > descMontaje) ) {
		descMontaje = descMontaje + 1;
	}
	if (MAX_TABLA_MONTAJES <= descMontaje) {
		return (-1);
	} else {
		return (descMontaje);
	}
}


/*
 * Funcion que muestra la tabla de Montajes
 */
int mostrarTablaMontajes(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	libres=sprintk_concat(buffer, libres, "TABLA DE MONTAJES\n"); 

	for (i=0; i<MAX_TABLA_MONTAJES; i++) {
		if (TRUE == tablaMontajes[i].ocupado) {
			libres=mostrarMontaje(i, buffer, libres);
		}
	}
	return (libres);
}


/*
 * Funcion que libera una driver de la tabla de Montajes
 */
int liberarEntradaTablaMontajes(int descMontaje)
{
	tablaMontajes[descMontaje].ocupado = FALSE;
	strcpy(tablaMontajes[descMontaje].nombreMontaje,"");
	tablaMontajes[descMontaje].descFS = -1;
	tablaMontajes[descMontaje].descDispositivo = -1;
	free(tablaMontajes[descMontaje].datosPropiosMontaje);
	tablaMontajes[descMontaje].datosPropiosMontaje = NULL;
	tablaMontajes[descMontaje].montarVolumen = NULL;
	tablaMontajes[descMontaje].reservarBloque = NULL;
	tablaMontajes[descMontaje].liberarBloque = NULL;
	tablaMontajes[descMontaje].reservarInodo = NULL;
	tablaMontajes[descMontaje].liberarInodo = NULL;
	tablaMontajes[descMontaje].buscarNombreInodo = NULL;
	tablaMontajes[descMontaje].crearInodo = NULL;
	tablaMontajes[descMontaje].crearDescFichero = NULL;
	tablaMontajes[descMontaje].sincronizarVolumen = NULL;
	tablaMontajes[descMontaje].desmontarVolumen = NULL;
	return (0);
}

