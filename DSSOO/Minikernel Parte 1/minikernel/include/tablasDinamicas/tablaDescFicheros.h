/*
 *  tablaDescFicheros.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _TABLA_DESCFICHEROS_H_
#define _TABLA_DESCFICHEROS_H_

#include "nucleoBasico/const.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define MAX_TABLA_DESCFICHEROS 256

#define T_DISPOSITIVO 0
#define T_FICHERO 1

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definición del objeto descriptor de fichero
 */
 
typedef struct {
	/* atributos */
	int ocupado;
	int tipo;
	int descInodo;
	int descMontaje;
	int descDispositivo;
    void *datosPropiosDescFichero;
	/* metodos */
	int (*abrirFichero) (int, char *, int);
	int (*leerFichero) (int, char *, int);
	int (*escribirFichero) (int, char *, int);
	int (*buscarEnFichero) (int, int);
	int (*cerrarFichero) (int);
	int (*mostrarFichero) (void *, int, char *, int);
}	TipoDescFichero; 

/*
 * Defincion del tipo de la tabla de DescFicheros
 */
 
typedef TipoDescFichero TipoTablaDescFicheros[MAX_TABLA_DESCFICHEROS];
 
/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* -----------------------------------------------------*
 * Funciones genericas de gestión del objeto DescFichero
 * -----------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha creado vacio y lo asocia a "nombre"
 */
int abrirFichero(char *nombre, int flags);

/*
 * Funcion que lee un buffer de datos del descriptor
 * de fichero.
 */
int leerFichero(int descFichero, char *buffer, int tamanyo);

/*
 * Funcion que escribe un buffer de datos en el descriptor
 * de fichero.
 */
int escribirFichero(int descFichero, char *buffer, int tamanyo);

/*
 * Funcion que modifica la posicion de lectura/escritura
 * del descriptor de fichero.
 */
int buscarEnFichero(int descFichero, int offset);

/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero(int descFichero);

/*
 * Funcion que muestra los componentes de un descriptor de fichero
 */
int mostrarFichero(void *infoFichero, int descFichero, char *buffer, int bytesLibres);


/* ------------------------------------------------*
 * Funciones de gestión de la tabla de DescFicheros
 * ------------------------------------------------*/

/*
 * Funcion que inicializa la tabla de descriptores 
 * de ficheros del proceso
 */
int inicializarTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros);

/*
 * Funcion que reserva una entrada en la tabla de 
 * descriptores de ficheros del proceso
 */
int reservarEntradaTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros);

/*
 * Funcion que muestra la tabla de descriptores
 * de ficheros de un proceso.
 */
int mostrarTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros, char *buffer, int bytesLibres);

/*
 * Funcion que libera una entrada en la tabla de 
 * descriptores de ficheros del proceso 
 */
int liberarEntradaTablaDescFicheros(TipoTablaDescFicheros tablaDescFicheros, int descFichero);

#endif /* _TABLA_DESCFICHEROS_H_ */

 
