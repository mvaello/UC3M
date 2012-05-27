/*
 *  bufferCaracteres.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _BUFFER_CARACTERES_H_
#define _BUFFER_CARACTERES_H_

#include "nucleoBasico/const.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/
 
#define TAM_BUFFER_CARACTERES 256

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definicion del tipo que representa al buffer de teclas 
 * implementada como un bufer circular con
 * un tamaÒo m·ximo de teclas (TAM_BUFFER_CARACTERES)
 */
typedef struct{
	char caracter[TAM_BUFFER_CARACTERES];
	int primero;
	int ultimo;
} TipoBufferCaracteres;

/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* ----------------------------------------------*
 * Funciones de gestión de la lista de caracteres
 * ----------------------------------------------*/

/* 
 * Funcion auxiliar para inicializar el buffer de caracteres 
 */
void iniciarBufferCaracteres(TipoBufferCaracteres *buffer);

/* 
 * Funcion auxiliar para insertar un caracter al final del buffer
 */
void insertarBufferCaracteres(TipoBufferCaracteres *buffer, char caracter);

/* 
 * Funcion auxiliar para obtener un caracter del principio del buffer 
 */
int extraerBufferCaracteres(TipoBufferCaracteres *buffer, char *caracter);

/*
 * Funcion que muestra los componentes del buffer de caracteres
 */
int mostrarBufferCaracteres(TipoBufferCaracteres *bufferCar, char *buffer, int bytesLibres);

/* 
 * Funcion para ver si hay caracteres en el buffer
 */
int estaVacioBufferCaracteres(TipoBufferCaracteres *buffer);

#endif /* _BUFFER_CARACTERES_H_ */

 
