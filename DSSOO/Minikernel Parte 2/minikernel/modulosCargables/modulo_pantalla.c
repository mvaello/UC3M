/*
 *  modulo_pantalla.c
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
 * CONSTANTES
 ***************************************************************/

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/
 
/* ------------------------------------------------------------*
 * Funciones especificas de gestión del DescFichero de pantalla
 * ------------------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha cleero vacio y lo asocia a "nombre"
 */
int abrirFichero_pantalla(int descFichero, char *nombre, int flags)
{	
	return (0);
}

/*
 * Funcion que escribe un buffer de datos en la pantalla.
 */
int escribirFichero_pantalla(int descFichero, char *buffer, int tamanyo)
{
	int descDispositivo;
	int i;

	/* obtener el desc. del dispositivo */
	descDispositivo = procesoActual->tablaDescFicheros[descFichero].descDispositivo;

	for (i=0; i<tamanyo;i++) {
		/* imprimir uno a uno, los caracteres del buffer */
		peticionCaracter(descDispositivo, &(buffer[i]), OP_WRITE);
	}
	return (i);
}

/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero_pantalla(int descFichero)
{
	/* borrar descripor de la tabla */
	liberarEntradaTablaDescFicheros(procesoActual->tablaDescFicheros, descFichero);
	return (0);
}
 

/* -------------------------------------------------------------------*
 * Funciones especificas de gestión del objeto dispositivo de pantalla
 * -------------------------------------------------------------------*/

/*
 * Funcion que crear un nuevo descriptor de fichero de pantalla vacio 
 * en la tabla de descriptores de ficheros
 */
int crearDescFicheroDispositivo_pantalla(int descDispositivo, TipoTablaDescFicheros tablaDescFicheros)
{
	int descFichero;
	
	/* buscar entrada libre de Desc. de fichero en la tabla del proceso*/
	descFichero = reservarEntradaTablaDescFicheros(tablaDescFicheros);

	/* cargar tipo y desc. de dispositivo */
	tablaDescFicheros[descFichero].tipo = T_DISPOSITIVO;
	tablaDescFicheros[descFichero].descDispositivo = descDispositivo;

	/* no hay datos propios */
	tablaDescFicheros[descFichero].datosPropiosDescFichero=NULL;

	/* cargar punteros a las funciones del Desc. de fichero del teclado */
	tablaDescFicheros[descFichero].abrirFichero = abrirFichero_pantalla;
	tablaDescFicheros[descFichero].leerFichero = NULL;
	tablaDescFicheros[descFichero].escribirFichero = escribirFichero_pantalla;
	tablaDescFicheros[descFichero].buscarEnFichero = NULL;
	tablaDescFicheros[descFichero].cerrarFichero = cerrarFichero_pantalla;
	tablaDescFicheros[descFichero].mostrarFichero = NULL;

	return (descFichero);
}

/*
 * Funcion que realiza una peticion de lectura/escritura
 * de un caracter al dispositivo de pantalla
 */
int peticionCaracter_pantalla(int descDispositivo, char *caracter, int operacion)
{ 
	int hardwareID;
	int ret;

	/* obtener identificador hardware de la pantalla */
	hardwareID = tablaDispositivos[descDispositivo].hardwareID;

	/* si la operacion NO es de lectura abortar */
	if (operacion != OP_WRITE) {
		return (-1);
	}
	
	/* escribir el caracter al puerto de la pantalla */
	ret=escribir_puerto(hardwareID, (*caracter));
    return (ret);
}

/* --------------------------------------------------------------*
 * Funciones especificas de gestión del objeto driver de pantalla
 * --------------------------------------------------------------*/

/*
 * Funcion que crear un nuevo Dispositivo vacio 
 * en la tabla de Dispositivos
 */
int crearDispositivo_pantalla(int descDriver, char *nombreDispositivo, int hardwareID)
{
	int descDispositivo;
	int ret;
	
	/* enchufar el Hardware */
	ret = plug_hardware (hardwareID, NULL);
	if (-1 == ret) {
		printk ("Error al activar la pantalla\n");
		return (-1);
	}

	/* buscar entrada libre de Dispositivo en la tabla */
	descDispositivo = reservarEntradaTablaDispositivos(nombreDispositivo);

	/* cargar descriptor de Driver e identificador de hardware */
	tablaDispositivos[descDispositivo].descDriver = descDriver;
	tablaDispositivos[descDispositivo].hardwareID = hardwareID;

	/* no hay datos propios */
	tablaDispositivos[descDispositivo].datosPropiosDispositivo=NULL;

	/* cargar punteros a las funciones del Dispositivo teclado */
	tablaDispositivos[descDispositivo].crearDescFicheroDispositivo = crearDescFicheroDispositivo_pantalla;
	tablaDispositivos[descDispositivo].peticionCaracter = peticionCaracter_pantalla;
	tablaDispositivos[descDispositivo].peticionBloque = NULL;
	tablaDispositivos[descDispositivo].interrupcionHW = NULL;
	tablaDispositivos[descDispositivo].interrupcionSW = NULL;
	tablaDispositivos[descDispositivo].destruirDispositivo = NULL;
	tablaDispositivos[descDispositivo].mostrarDispositivo = NULL;

	return (descDispositivo);
}


/* -----------------------------------------------------*
 * Funciones especificas de cargar y descarga de modulos
 * -----------------------------------------------------*/

/*
 * Funcion que inicializa el modulo y carga el objeto Driver de pantalla
 * en la tabla de Drivers
 */
int cargarModulo_pantalla ()
{ 
	int descDriver;

	/* buscar entrada libre de Driver en la tabla */
	descDriver = reservarEntradaTablaDrivers("pantalla");
	
	/* no hay datos propios */
	tablaDrivers[descDriver].datosPropiosDriver = NULL;

	/* cargar punteros a las funciones del driver de teclado */
	tablaDrivers[descDriver].crearDispositivo = crearDispositivo_pantalla;
	tablaDrivers[descDriver].destruirDriver = NULL;
	tablaDrivers[descDriver].mostrarDriver = NULL;
	return (0);
}



