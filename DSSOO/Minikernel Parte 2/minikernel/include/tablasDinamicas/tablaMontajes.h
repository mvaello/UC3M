/*
 *  tablaMontajes.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _TABLA_MONTAJES_H_
#define _TABLA_MONTAJES_H_

#include "nucleoBasico/const.h"
#include "tablasDinamicas/tablaDescFicheros.h"


/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define MAX_TABLA_MONTAJES 256

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definición del objeto driver
 */
 
typedef struct {
	/* atributos */
	int ocupado;
	char nombreMontaje[MAX_STRING];
	int descFS;
	int descDispositivo;
    void *	datosPropiosMontaje;
	/* metodos */
	int (*montarVolumen) (int, char *);
	int (*reservarBloque) (int);
	int (*liberarBloque) (int, int);
	int (*reservarInodo) (int);
	int (*liberarInodo) (int, int);
	int (*buscarNombreInodo) (int, char *);
	int (*crearInodo) (int, int);
	int (*crearDescFichero) (int, TipoTablaDescFicheros);
	int (*sincronizarVolumen) (int);	
	int (*desmontarVolumen) (int);
	int (*mostrarMontaje) (int, char *, int);
}	TipoMontaje; 

 
/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/
/*
 * Variable global de la tabla de Montajes
 */
 
extern TipoMontaje tablaMontajes[MAX_TABLA_MONTAJES];
 
 
/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* -------------------------------------------------*
 * Funciones genericas de gestión del objeto Montaje
 * -------------------------------------------------*/

/*
 * Funcion que monta un volumen formateado con  
 * un sistema de ficheros
 */
int montarVolumen(char *nombreDispositivo, char *nombreMontaje, char *nombreFS);

/*
 * Funcion que reserva un nuevo bloque en el volumen 
 */
int reservarBloque(int descMontaje);

/*
 * Funcion que libera un bloque del volumen 
 */
int liberarBloque(int descMontaje, int numBloque);

/*
 * Funcion que reserva un nuevo inodo en el volumen 
 */
int reservarInodo(int descMontaje);

/*
 * Funcion que libera un nuevo inodo del volumen 
 */
int liberarInodo(int descMontaje, int numInodo);

/*
 * Funcion que busca el inodo con el nombre correspondiente 
 * del volumen en la tabla de Inodos
 */
int buscarNombreInodo(int descMontaje, char *nombre);

/*
 * Funcion que crear un nuevo Inodo vacio 
 * en la tabla de Inodos
 */
int crearInodo(int descMontaje, int numInodo);

/*
 * Funcion que crear un nuevo descriptor de fichero vacio 
 * en la tabla de descriptores de ficheros del proceso
 */
int crearDescFichero(int descMontaje, TipoTablaDescFicheros tablaDescFicheros);

/*
 * Funcion que escribe el volumen todos los cambios
 * producidos hasta ahora
 */
int sincronizarVolumen(char *nombreMontaje);

/*
 * Funcion que desmonta el volumen y destruye el 
 * objeto Montaje y lo elimina de la tabla
 */
int desmontarVolumen(char *nombreMontaje);

/*
 * Funcion que muestra los componentes de un Montaje
 */
int mostrarMontaje(int descMontaje, char *buffer, int bytesLibres);


/* --------------------------------------------*
 * Funciones de gestión de la tabla de Montajes
 * --------------------------------------------*/

/*
 * Funcion que inicializa la tabla de Montajes
 */
int inicializarTablaMontajes();

/*
 * Funcion que reserva una entrada en la tabla de 
 * de Montajes para el objeto "nombreMontaje"
 */
int reservarEntradaTablaMontajes(char *nombreMontaje);

/*
 * Funcion que Busca un Montaje en la tabla de Montajes
 * por el nombre.
 */
int buscarEntradaTablaMontajes(char *nombreMontaje);

/*
 * Funcion que muestra la tabla de Montajes
 */
int mostrarTablaMontajes(char *buffer, int bytesLibres);

/*
 * Funcion que libera un Montaje de la tabla de Montajes
 */
int liberarEntradaTablaMontajes(int descMontaje);

#endif /* _TABLA_MONTAJES_H_ */

 
