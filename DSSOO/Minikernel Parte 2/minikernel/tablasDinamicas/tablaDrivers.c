/*
 *  tablaDrivers.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include "minikernel.h"


/***************************************************************
 * VARIABLES GLOBALES
 ***************************************************************/
/*
 * Variable global de la tabla de Drivers
 */
 
TipoDriver tablaDrivers[MAX_TABLA_DRIVERS];
 
/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* ------------------------------------------------*
 * Funciones genericas de gestión del objeto Driver
 * ------------------------------------------------*/

/*
 * Funcion que crear un nuevo Dispositivo vacio 
 * en la tabla de Dispositivos
 */
int crearDispositivo(int descDriver, char *nombreDispositivo, int hardwareID)
{
	int descDispositivo;
	
	if ( (0 > descDriver) || (MAX_TABLA_DRIVERS <= descDriver) ||
		 (0 > hardwareID) || (NULL == nombreDispositivo) ||
		 (NULL == tablaDrivers[descDriver].crearDispositivo) ) {
		return (-1);
	}
	descDispositivo = tablaDrivers[descDriver].crearDispositivo(descDriver,nombreDispositivo, hardwareID);
	return (descDispositivo);
}

/*
 * Funcion que destruye un objeto Driver y lo
 * elimina de la tabla
 */
int destruirDriver(int descDriver)
{
	int ret;
	
	if ( (0 > descDriver) || (MAX_TABLA_DRIVERS <= descDriver) ||
		 (NULL == tablaDrivers[descDriver].destruirDriver) ) {
		return (-1);
	}
	ret=tablaDrivers[descDriver].destruirDriver(descDriver);
	return (ret);
}

/*
 * Funcion que muestra los componentes de un driver
 */
int mostrarDriver(int descDriver, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;

	if ( (NULL == buffer) || (0 >= bytesLibres) ||
		 (0 > descDriver) || (MAX_TABLA_DRIVERS <= descDriver) ) {
		return (0);
	}
	
	/* mostart componentes generales */
	libres=sprintk_concat(buffer, libres, "DRIVER:\tID=%d, nombre=%s\n",
						  descDriver, tablaDrivers[descDriver].nombreDriver);	
	/*mostrar componentes especificos */
	if (NULL != tablaDrivers[descDriver].mostrarDriver) {
		libres=tablaDrivers[descDriver].mostrarDriver(descDriver, buffer, libres);
	}
	return (libres);
}

/* -------------------------------------------*
 * Funciones de gestión de la tabla de Drivers
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Drivers
 */
int inicializarTablaDrivers()
{
	int i;
	
	for (i=0;i<MAX_TABLA_DRIVERS;i++) {
		tablaDrivers[i].ocupado = FALSE;
		strcpy(tablaDrivers[i].nombreDriver,"");
		tablaDrivers[i].datosPropiosDriver = NULL;
		tablaDrivers[i].crearDispositivo = NULL;
		tablaDrivers[i].destruirDriver = NULL;
		tablaDrivers[i].mostrarDriver = NULL;
	}
	return (0);
}
	
/*
 * Funcion que reserva una entrada en la tabla de 
 * de Drivers para el objeto "nombreDriver"
 */
int reservarEntradaTablaDrivers(char *nombreDriver)
{
	int descDriver = 0;

	if (NULL == nombreDriver) {
		return (-1);
	}
	while ( (TRUE == tablaDrivers[descDriver].ocupado) &&
			(MAX_TABLA_DRIVERS > descDriver) ) {
		descDriver = descDriver + 1;
	}
	if (MAX_TABLA_DRIVERS > descDriver) {
		tablaDrivers[descDriver].ocupado = TRUE;
		strcpy(tablaDrivers[descDriver].nombreDriver,nombreDriver);
		return (descDriver);
	}
	return (-1);
}

/*
 * Funcion que Busca un Driver en la tabla de Drivers
 * por el nombre.
 */
int buscarEntradaTablaDrivers(char *nombreDriver)
{
	int descDriver = 0;

	if (NULL == nombreDriver) {
		return (-1);
	}
	while ( ( (FALSE == tablaDrivers[descDriver].ocupado) || 
		      (0 != strcmp(tablaDrivers[descDriver].nombreDriver,nombreDriver)) ) &&
			(MAX_TABLA_DRIVERS > descDriver) ) {
		descDriver = descDriver + 1;
	}
	if (MAX_TABLA_DRIVERS <= descDriver) {
		return (-1);
	} else {
		return (descDriver);
	}
}

/*
 * Funcion que muestra la tabla de Drivers
 */
int mostrarTablaDrivers(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	libres=sprintk_concat(buffer, libres, "TABLA DE DRIVERS\n"); 

	for (i=0; i<MAX_TABLA_DISPOSITIVOS; i++) {
		if (TRUE == tablaDrivers[i].ocupado) {
			libres=mostrarDriver(i, buffer, libres);
		}
	}
	return (libres);
}

/*
 * Funcion que libera una driver de la tabla de Drivers
 */
int liberarEntradaTablaDrivers(int descDriver)
{
	tablaDrivers[descDriver].ocupado = FALSE;
	strcpy(tablaDrivers[descDriver].nombreDriver,"");
	free(tablaDrivers[descDriver].datosPropiosDriver);
	tablaDrivers[descDriver].datosPropiosDriver = NULL;
	tablaDrivers[descDriver].crearDispositivo = NULL;
	tablaDrivers[descDriver].destruirDriver = NULL;
	tablaDrivers[descDriver].mostrarDriver = NULL;
	return (0);
}

