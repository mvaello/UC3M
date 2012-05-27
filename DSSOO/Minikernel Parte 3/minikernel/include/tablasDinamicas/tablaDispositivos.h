/*
 *  tablaDispositivos.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _TABLA_DISPOSITIVOS_H_
#define _TABLA_DISPOSITIVOS_H_

#include "nucleoBasico/const.h"
#include "tablasDinamicas/tablaDescFicheros.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define MAX_TABLA_DISPOSITIVOS 256

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definición del objeto driver
 */
 
typedef struct {
	/* atributos */
	int ocupado;
	char nombreDispositivo[MAX_STRING];
	int descDriver;
	int hardwareID;
    void *	datosPropiosDispositivo;
	/* metodos */
	int (*crearDescFicheroDispositivo) (int, TipoTablaDescFicheros);
	int (*peticionCaracter) (int, char *, int);
	int (*peticionBloque) (int, int, char *, int);
	int (*interrupcionHW) (int);
	void (*interrupcionSW) (int);
	int (*destruirDispositivo) (int);
	int (*mostrarDispositivo) (int, char *, int);
}	TipoDispositivo; 

 
/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/
/*
 * Variable global de la tabla de Dispositivos
 */
 
extern TipoDispositivo tablaDispositivos[MAX_TABLA_DISPOSITIVOS];
 
 
/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* -----------------------------------------------------*
 * Funciones genericas de gestión del objeto Dispositivo
 * -----------------------------------------------------*/

/*
 * Funcion que crear un nuevo descriptor de fichero vacio 
 * en la tabla de descriptores de ficheros
 */
int crearDescFicheroDispositivo(int descDispositivo, TipoTablaDescFicheros tablaDescFicheros);

/*
 * Funcion que realiza una peticion de lectura/escritura
 * de un caracter al dispositivo
 */
int peticionCaracter(int descDispositivo, char *caracter, int operacion);

/*
 * Funcion que realiza una peticion de lectura/escritura
 * de un bloque al dispositivo
 */
int peticionBloque(int descDispositivo, int numBloque, char *bloque, int operacion);

/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * hardware del Dispositivo 
 */
int interrupcionHW(int descDispositivo);

/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * software desde la interrupcion HW del Dispositivo 
 */
void interrupcionSW(void *datos);

/*
 * Funcion que destruye un objeto Dispositivo y lo
 * elimina de la tabla
 */
int destruirDispositivo(int descDispositivo);

/*
 * Funcion que muestra los componentes de un dispositivo
 */
int mostrarDispositivo(int descDispositivo, char *buffer, int bytesLibres);


/* ------------------------------------------------*
 * Funciones de gestión de la tabla de Dispositivos
 * ------------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Dispositivos
 */
int inicializarTablaDispositivos();

/*
 * Funcion que reserva una entrada en la tabla de 
 * de Dispositivos para el objeto "nombreDispositivo"
 */
int reservarEntradaTablaDispositivos(char *nombreDispositivo);

/*
 * Funcion que Busca un Dispositivo en la tabla de Dispositivos
 * por el nombre.
 */
int buscarEntradaTablaDispositivos(char *nombreDispositivo);

/*
 * Funcion que muestra la tabla de Dispositivos
 */
int mostrarTablaDispositivos(char *buffer, int bytesLibres);

/*
 * Funcion que libera una driver de la tabla de Dispositivos
 */
int liberarEntradaTablaDispositivos(int descDispositivo);

#endif /* _TABLA_DISPOSITIVOS_H_ */

 
