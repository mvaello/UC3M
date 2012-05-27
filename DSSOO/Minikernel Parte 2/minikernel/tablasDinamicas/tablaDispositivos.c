/*
 *  tablaDispositivos.c
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
 * Variable global de la tabla de Dispositivos
 */
 
TipoDispositivo tablaDispositivos[MAX_TABLA_DISPOSITIVOS];
 
/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* -----------------------------------------------------*
 * Funciones genericas de gestión del objeto Dispositivo
 * -----------------------------------------------------*/

/*
 * Funcion que crear un nuevo descriptor de fichero vacio 
 * en la tabla de descriptores de ficheros
 */
int crearDescFicheroDispositivo(int descDispositivo, TipoTablaDescFicheros tablaDescFicheros)
{
	int descDescFichero;

	if ( (0 > descDispositivo) || (MAX_TABLA_DISPOSITIVOS <= descDispositivo) ||
		 (NULL == tablaDispositivos[descDispositivo].crearDescFicheroDispositivo) ) {
		return (-1);
	}
	descDescFichero = tablaDispositivos[descDispositivo].crearDescFicheroDispositivo(descDispositivo,tablaDescFicheros);
	return (descDescFichero);
}
/*
 * Funcion que realiza una peticion de lectura/escritura
 * de un caracter al dispositivo
 */
int peticionCaracter(int descDispositivo, char *caracter, int operacion)
{
	int ret;
	
	if ( (0 > descDispositivo) || (MAX_TABLA_DISPOSITIVOS <= descDispositivo) ||
		 (NULL == caracter) || 
		 ( (OP_READ != operacion) && (OP_WRITE != operacion) ) ||
		 (NULL == tablaDispositivos[descDispositivo].peticionCaracter) ) {

		return (-1);
	}
	ret=tablaDispositivos[descDispositivo].peticionCaracter(descDispositivo,caracter,operacion);
	return (ret);
}

/*
 * Funcion que realiza una peticion de lectura/escritura
 * de un bloque al dispositivo
 */
int peticionBloque(int descDispositivo, int numBloque, char *bloque, int operacion)
{
	int ret;
		
	if ( (0 > descDispositivo) || (MAX_TABLA_DISPOSITIVOS <= descDispositivo) ||
		 (NULL == bloque) ||
		 ( (OP_READ != operacion) && (OP_WRITE != operacion) ) ||
		 (NULL == tablaDispositivos[descDispositivo].peticionBloque) ) {

		return (-1);
	}
	ret=tablaDispositivos[descDispositivo].peticionBloque(descDispositivo,numBloque, bloque, operacion);
	return (ret);
}

/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * hardware del Dispositivo 
 */
int interrupcionHW(int descDispositivo)
{
	if ( (0 > descDispositivo) || (MAX_TABLA_DISPOSITIVOS <= descDispositivo) ||
		 (NULL == tablaDispositivos[descDispositivo].interrupcionHW) ) {

		return (-1);
	}
	tablaDispositivos[descDispositivo].interrupcionHW(descDispositivo);
	return (0);
}

/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * software desde la interrupcion HW del Dispositivo 
 */
void interrupcionSW(void *datos)
{
	int descDispositivo;
	
	descDispositivo = (int)(datos);
	if ( (0 <= descDispositivo) && (MAX_TABLA_DISPOSITIVOS > descDispositivo) &&
	     (NULL != tablaDispositivos[descDispositivo].interrupcionSW) ) {
		tablaDispositivos[descDispositivo].interrupcionSW(descDispositivo);
	}
}

/*
 * Funcion que destruye un objeto Dispositivo y lo
 * elimina de la tabla
 */
int destruirDispositivo(int descDispositivo)
{
	if ( (0 > descDispositivo) || (MAX_TABLA_DISPOSITIVOS <= descDispositivo) ||
		 (NULL == tablaDispositivos[descDispositivo].destruirDispositivo) ) {

		return (-1);
	}
	tablaDispositivos[descDispositivo].destruirDispositivo(descDispositivo);
	return (0);
}

/*
 * Funcion que muestra los componentes de un dispositivo
 */
int mostrarDispositivo(int descDispositivo, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;

	if ( (NULL == buffer) || (0 >= bytesLibres) ||
		 (0 > descDispositivo) || (MAX_TABLA_DISPOSITIVOS <= descDispositivo) ) {
		return (0);
	}
	
	/* mostart componentes generales */
	libres=sprintk_concat(buffer, libres,"DISPOSITIVO:\tID=%d, nombre=%s, driver=%d, hardwareID=%d\n",
						  descDispositivo, tablaDispositivos[descDispositivo].nombreDispositivo, 
						  tablaDispositivos[descDispositivo].descDriver,
						  tablaDispositivos[descDispositivo].hardwareID); 	
		
	/*mostrar componentes especificos */
	if (NULL != tablaDispositivos[descDispositivo].mostrarDispositivo) {
		libres=tablaDispositivos[descDispositivo].mostrarDispositivo(descDispositivo, 
																	 buffer, libres);
	}
	return (libres);
}


/* ------------------------------------------------*
 * Funciones de gestión de la tabla de Dispositivos
 * ------------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Dispositivos
 */
int inicializarTablaDispositivos()
{
	int i;
	
	for (i=0;i<MAX_TABLA_DISPOSITIVOS;i++) {
		tablaDispositivos[i].ocupado = FALSE;
		strcpy(tablaDispositivos[i].nombreDispositivo,"");
		tablaDispositivos[i].descDriver = -1;
		tablaDispositivos[i].hardwareID = -1;
		tablaDispositivos[i].datosPropiosDispositivo = NULL;
		tablaDispositivos[i].crearDescFicheroDispositivo = NULL;
		tablaDispositivos[i].peticionCaracter = NULL;
		tablaDispositivos[i].peticionBloque = NULL;
		tablaDispositivos[i].interrupcionHW = NULL;
		tablaDispositivos[i].interrupcionSW = NULL;
		tablaDispositivos[i].destruirDispositivo = NULL;
		tablaDispositivos[i].mostrarDispositivo = NULL;
	}
	return (0);
}
	
/*
 * Funcion que reserva una entrada en la tabla de 
 * de Dispositivos para el objeto "nombreDispositivo"
 */
int reservarEntradaTablaDispositivos(char *nombreDispositivo)
{
	int descDispositivo = 0;

	if (NULL == nombreDispositivo) {
		return (-1);
	}
	while ( (TRUE == tablaDispositivos[descDispositivo].ocupado) &&
			(MAX_TABLA_DISPOSITIVOS > descDispositivo) ) {
		descDispositivo = descDispositivo + 1;
	}
	if (MAX_TABLA_DISPOSITIVOS > descDispositivo) {
		tablaDispositivos[descDispositivo].ocupado = TRUE;
		strcpy(tablaDispositivos[descDispositivo].nombreDispositivo,nombreDispositivo);
		return (descDispositivo);
	}
	return (-1);
}

/*
 * Funcion que Busca un Dispositivo en la tabla de Dispositivos
 * por el nombre.
 */
int buscarEntradaTablaDispositivos(char *nombreDispositivo)
{
	int descDispositivo = 0;

	if (NULL == nombreDispositivo) {
		return (-1);
	}
	while ( ( (FALSE == tablaDispositivos[descDispositivo].ocupado) || 
		      (0 != strcmp(tablaDispositivos[descDispositivo].nombreDispositivo,nombreDispositivo)) ) &&
			(MAX_TABLA_DISPOSITIVOS > descDispositivo) ) {
		descDispositivo = descDispositivo + 1;
	}
	if (MAX_TABLA_DISPOSITIVOS <= descDispositivo) {
		return (-1);
	} else {
		return (descDispositivo);
	}
}

/*
 * Funcion que muestra la tabla de Dispositivos
 */
int mostrarTablaDispositivos(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	libres=sprintk_concat(buffer, libres, "TABLA DE DISPOSITIVOS\n"); 

	for (i=0; i<MAX_TABLA_DISPOSITIVOS; i++) {
		if (TRUE == tablaDispositivos[i].ocupado) {
			libres=mostrarDispositivo(i, buffer, libres);
		}
	}
	return (libres);
}



/*
 * Funcion que libera una driver de la tabla de Dispositivos
 */
int liberarEntradaTablaDispositivos(int descDispositivo)
{
	tablaDispositivos[descDispositivo].ocupado = FALSE;
	strcpy(tablaDispositivos[descDispositivo].nombreDispositivo,"");
	tablaDispositivos[descDispositivo].descDriver = -1;
	tablaDispositivos[descDispositivo].hardwareID = -1;
	free(tablaDispositivos[descDispositivo].datosPropiosDispositivo);
	tablaDispositivos[descDispositivo].datosPropiosDispositivo = NULL;
	tablaDispositivos[descDispositivo].crearDescFicheroDispositivo = NULL;
	tablaDispositivos[descDispositivo].peticionCaracter = NULL;
	tablaDispositivos[descDispositivo].peticionBloque = NULL;
	tablaDispositivos[descDispositivo].interrupcionHW = NULL;
	tablaDispositivos[descDispositivo].interrupcionSW = NULL;
	tablaDispositivos[descDispositivo].destruirDispositivo = NULL;
	tablaDispositivos[descDispositivo].mostrarDispositivo = NULL;
	return (0);
}


