/*
 *  tablaInodos.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _TABLA_INODOS_H_
#define _TABLA_INODOS_H_

#include "nucleoBasico/const.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define MAX_TABLA_INODOS 1024

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definición del objeto driver
 */
 
typedef struct {
	/* atributos */
	int ocupado;
	int descMontaje;
	int numInodo;
	int numBloque;
    void *	datosPropiosInodo;
	/* metodos */
	int (*iniciarNuevoInodo) (int, char *, int);
	int (*leerInodo) (int);
	int (*mapearBloque) (int, int);
	int (*asignarNuevoBloque) (int, int);
	int (*buscarNombreEnDir) (int, char *);
	int (*escribirInodo) (int);
	int (*borrarInodo) (int);
	int (*mostrarInodo) (int, char *, int);
}	TipoInodo; 

 
/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/
/*
 * Variable global de la tabla de Inodos
 */
 
extern TipoInodo tablaInodos[MAX_TABLA_INODOS];
 
 
/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* ------------------------------------------------*
 * Funciones genericas de gestión del objeto Inodo
 * ------------------------------------------------*/

/*
 * Funcion que inicializa un nuevo inodo 
 * en la tabla de inodos
 */
int iniciarNuevoInodo(int descInodo, char *nombre, int flags);

/*
 * Funcion que lee un objeto Inodo del volumen
 */
int leerInodo(int descInodo);

/*
 * Funcion que localiza el bloque de disco 
 * de un bloque del fichero
 */
int mapearBloque(int descInodo, int numBloqueArchivo);

/*
 * Funcion que reserva y asigna un nuevo bloque de disco 
 * para el bloque de fichero correspondiente
 */
int asignarNuevoBloque(int descInodo, int numBloqueArchivo);

/*
 * Funcion que busca el primer objeto Inodo
 * que corresponde con el nombre dentro del mismo volumen.
 */
int buscarNombreEnDir(int descInodo, char *nombre);

/*
 * Funcion que escribe un objeto Inodo en 
 * el volumen
 */
int escribirInodo(int descInodo);

/*
 * Funcion que borrar un objeto Inodo y lo
 * elimina de la tabla
 */
int borrarInodo(int descInodo);

/*
 * Funcion que muestra los componentes de un inodo
 */
int mostrarInodo(int descInodo, char *buffer, int bytesLibres);


/* -------------------------------------------*
 * Funciones de gestión de la tabla de Inodos
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Inodos
 */
int inicializarTablaInodos();

/*
 * Funcion que reserva una entrada en la tabla de 
 * de Inodos para dicho numero de inodo y volumen
 */
int reservarEntradaTablaInodos(int descMontaje, int numInodo);

/*
 * Funcion que Busca un Inodo en la tabla de Inodos
 * por el numero de inodo y el volumen.
 */
int buscarEntradaTablaInodos(int descMontaje, int numInodo);

/*
 * Funcion que muestra la tabla de Inodos
 */
int mostrarTablaInodos(char *buffer, int bytesLibres);

/*
 * Funcion que libera una driver de la tabla de Inodos
 */
int liberarEntradaTablaInodos(int descInodo);

#endif /* _TABLA_INODOS_H_ */

 
