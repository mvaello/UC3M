/*
 *  tablaDrivers.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _TABLA_DRIVERS_H_
#define _TABLA_DRIVERS_H_

#include "nucleoBasico/const.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define MAX_TABLA_DRIVERS 256

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definición del objeto driver
 */
 
typedef struct {
	/* atributos */
	int ocupado;
	char nombreDriver[MAX_STRING];
    void *	datosPropiosDriver;
	/* metodos */
	int (*crearDispositivo) (int, char *, int);
	int (*destruirDriver) (int);
	int (*mostrarDriver) (int, char *, int);
}	TipoDriver; 

 
/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/
/*
 * Variable global de la tabla de Drivers
 */
 
extern TipoDriver tablaDrivers[MAX_TABLA_DRIVERS];
 
 
/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* ------------------------------------------------*
 * Funciones genericas de gestión del objeto Driver
 * ------------------------------------------------*/

/*
 * Funcion que crear un nuevo Dispositivo  
 * en la tabla de Dispositivos
 */
int crearDispositivo(int descDriver, char *nombreDispositivo, int hardwareID);

/*
 * Funcion que destruye un objeto Driver y lo
 * elimina de la tabla
 */
int destruirDriver(int descDriver);

/*
 * Funcion que muestra los componentes de un driver
 */
int mostrarDriver(int descDriver, char *buffer, int bytesLibres);


/* -------------------------------------------*
 * Funciones de gestión de la tabla de Drivers
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Drivers
 */
int inicializarTablaDrivers();

/*
 * Funcion que reserva una entrada en la tabla de 
 * de Drivers para el objeto "nombreDriver"
 */
int reservarEntradaTablaDrivers(char *nombreDriver);

/*
 * Funcion que Busca un Driver en la tabla de Drivers
 * por el nombre.
 */
int buscarEntradaTablaDrivers(char *nombreDriver);

/*
 * Funcion que muestra la tabla de Drivers
 */
int mostrarTablaDrivers(char *buffer, int bytesLibres);

/*
 * Funcion que libera una driver de la tabla de Drivers
 */
int liberarEntradaTablaDrivers(int descDriver);

#endif /* _TABLA_DRIVERS_H_ */

 
