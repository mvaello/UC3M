/*
 *  tablaDescFicheros.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#include "minikernel.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"



/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* -----------------------------------------------------*
 * Funciones genericas de gestión del objeto DescFichero
 * -----------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha creado vacio y lo asocia a "nombre"
 */
int abrirFichero(char *nombre, int flags)
{
	int descDispositivo = -1; 
	int descMontaje= -1; 
	int descFichero= -1;
	char nombreTipo[MAX_STRING];
	char nombreAux[MAX_STRING];
	char nombreMontaje[MAX_STRING];
	char nombreFichero[MAX_STRING];
	int ret;
	
	if (NULL == nombre) {
		return (-1);
	}
	
	/*separar el nombre del tipo de componente del resto */
	sscanf(nombre,"/%[^/]/%s",nombreTipo,nombreAux);

	if (0==strcmp(nombreTipo,"DISPOSITIVOS")) {
		descDispositivo = buscarEntradaTablaDispositivos(nombreAux);
		descFichero = crearDescFicheroDispositivo(descDispositivo, procesoActual->tablaDescFicheros);
	} else if (0==strcmp(nombreTipo,"FICHEROS")) {
		/*separar el nombre del Montaje del nombre del fichero */
		sscanf(nombreAux,"%[^/]/%s",nombreMontaje,nombreFichero);
		descMontaje = buscarEntradaTablaMontajes(nombreMontaje);
		descFichero = crearDescFichero(descMontaje, procesoActual->tablaDescFicheros);
	} else {
		return (-1);
	}
	
	if (NULL == procesoActual->tablaDescFicheros[descFichero].abrirFichero) {
		return (-1);
	}
	
	ret = procesoActual->tablaDescFicheros[descFichero].abrirFichero(descFichero,nombreFichero,flags);
	if (-1 == ret) {
		return (-1);
	} 
	return (descFichero);
}
/*
 * Funcion que lee un buffer de datos del descriptor
 * de fichero.
 */
int leerFichero(int descFichero, char *buffer, int tamanyo)
{
	int ret;
	
	if ( (0 > descFichero) || (MAX_TABLA_DESCFICHEROS <= descFichero) ||
		 (NULL == buffer) ||
		 (NULL == procesoActual->tablaDescFicheros[descFichero].leerFichero) ) {

		return (-1);
	}

	ret=procesoActual->tablaDescFicheros[descFichero].leerFichero(descFichero,buffer,tamanyo);
	return (ret);
}

/*
 * Funcion que escribe un buffer de datos en el descriptor
 * de fichero.
 */
int escribirFichero(int descFichero, char *buffer, int tamanyo)
{
	int ret;
	
	if ( (0 > descFichero) || (MAX_TABLA_DESCFICHEROS <= descFichero) ||
		 (NULL == buffer) ||
		 (NULL == procesoActual->tablaDescFicheros[descFichero].escribirFichero) ) {

		return (-1);
	}
	ret=procesoActual->tablaDescFicheros[descFichero].escribirFichero(descFichero,buffer,tamanyo);
	return (ret);
}

/*
 * Funcion que modifica la posicion de lectura/escritura
 * del descriptor de fichero.
 */
int buscarEnFichero(int descFichero, int offset)
{
	int ret;
	
	if ( (0 > descFichero) || (MAX_TABLA_DESCFICHEROS <= descFichero) ||
		 (NULL == procesoActual->tablaDescFicheros[descFichero].buscarEnFichero) ) {

		return (-1);
	}
	ret=procesoActual->tablaDescFicheros[descFichero].buscarEnFichero(descFichero,offset);
	return (ret);
}

/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero(int descFichero)
{
	int ret;
	
	if ( (0 > descFichero) || (MAX_TABLA_DESCFICHEROS <= descFichero) ||
		 (NULL == procesoActual->tablaDescFicheros[descFichero].cerrarFichero) ) {

		return (-1);
	}
	ret=procesoActual->tablaDescFicheros[descFichero].cerrarFichero(descFichero);
	return (ret);
}

/*
 * Funcion que muestra los componentes de un descriptor de fichero
 */
int mostrarFichero(void *infoFichero, int descFichero, char *buffer, int bytesLibres)
{
	TipoDescFichero *AuxFichero;
	int libres=bytesLibres;

	if ( (NULL == infoFichero) || (NULL == buffer) || (0 >= bytesLibres) ||
		 (0 > descFichero) || (MAX_TABLA_DESCFICHEROS <= descFichero) ) {
		return (0);
	}
	
	AuxFichero=(TipoDescFichero *)infoFichero;	

	/* mostrar componentes generales */
	libres=sprintk_concat(buffer, libres,"DESCFICHERO:\tID=%d, tipo=%d, inodo=%d, montaje=%d, Dispositivo=%d\n",
						  descFichero, AuxFichero->tipo, AuxFichero->descInodo,
						  AuxFichero->descMontaje, AuxFichero->descDispositivo); 	
		
	/*mostrar componentes especificos */
	if (NULL != AuxFichero->mostrarFichero) {
		libres=AuxFichero->mostrarFichero(infoFichero, descFichero, buffer, libres);
	}
	
	return (libres);
}


/* -------------------------------------------*
 * Funciones de gestión de la tabla de DescFicheros
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de descriptores 
 * de ficheros del proceso
 */
int inicializarTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros)
{
	int i;
	
	for (i=0;i<MAX_TABLA_DESCFICHEROS;i++) {
		tablaDescFicheros[i].ocupado = FALSE;
		tablaDescFicheros[i].tipo = -1;
		tablaDescFicheros[i].descInodo = -1;
		tablaDescFicheros[i].descMontaje = -1;
		tablaDescFicheros[i].descDispositivo = -1;
		tablaDescFicheros[i].datosPropiosDescFichero = NULL;
		tablaDescFicheros[i].abrirFichero = NULL;
		tablaDescFicheros[i].leerFichero = NULL;
		tablaDescFicheros[i].escribirFichero = NULL;
		tablaDescFicheros[i].buscarEnFichero = NULL;
		tablaDescFicheros[i].cerrarFichero = NULL;
		tablaDescFicheros[i].mostrarFichero = NULL;
	}
	return (0);
}
	
/*
 * Funcion que reserva una entrada en la tabla de 
 * descriptores de ficheros del proceso
 */
int reservarEntradaTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros)
{
	int descFichero = 0;

	while ( (TRUE == tablaDescFicheros[descFichero].ocupado) &&
			(MAX_TABLA_DESCFICHEROS > descFichero) ) {
		descFichero = descFichero + 1;
	}
	if (MAX_TABLA_DESCFICHEROS > descFichero) {
		tablaDescFicheros[descFichero].ocupado = TRUE;
		return (descFichero);
	}
	return (-1);
}

/*
 * Funcion que muestra la tabla de descriptores
 * de ficheros de un proceso.
 */
int mostrarTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	libres=sprintk_concat(buffer, libres, "TABLA DE DESCRIPTORES DE FICHEROS\n"); 

	for (i=0; i<MAX_TABLA_DESCFICHEROS; i++) {
		if (TRUE == tablaDescFicheros[i].ocupado) {
			libres=mostrarFichero((void *)&(tablaDescFicheros[i]), i, buffer, libres);
		}
	}
	return (libres);
}

/*
 * Funcion que libera una driver de la tabla de DescFicheros
 */
int liberarEntradaTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros, int descFichero)
{
	tablaDescFicheros[descFichero].ocupado = FALSE;
	tablaDescFicheros[descFichero].tipo = -1;
	tablaDescFicheros[descFichero].descInodo = -1;
	tablaDescFicheros[descFichero].descMontaje = -1;
	tablaDescFicheros[descFichero].descDispositivo = -1;
	free(tablaDescFicheros[descFichero].datosPropiosDescFichero);
	tablaDescFicheros[descFichero].datosPropiosDescFichero = NULL;
	tablaDescFicheros[descFichero].abrirFichero = NULL;
	tablaDescFicheros[descFichero].leerFichero = NULL;
	tablaDescFicheros[descFichero].escribirFichero = NULL;
	tablaDescFicheros[descFichero].buscarEnFichero = NULL;
	tablaDescFicheros[descFichero].cerrarFichero = NULL;
	tablaDescFicheros[descFichero].mostrarFichero = NULL;
	return (0);
}

