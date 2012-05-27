/*
 *  listaPeticiones.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _LISTA_PETICIONES_H_
#define _LISTA_PETICIONES_H_

#include "nucleoBasico/const.h"
#include "nucleoBasico/ListaBCP_Planificador.h"


/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define TAM_BLOQUE 1024
#define OP_READ 0
#define OP_WRITE 1

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definicion del tipo que representa una 
 * peticion a un dispositivo de bloques
 */
typedef struct peticion{
	int descDispositivo;
	int numBloque;
	int operacion;
	char bloque[TAM_BLOQUE];
	int numPeticiones;
	TipoListaBCP listaProcesosBloqueados;
	struct peticion *siguiente;
} TipoPeticion;

/* 
 * Definicion del tipo que representa una
 * lista de peticiones
 */
typedef struct{
	TipoPeticion *primero;
	TipoPeticion *ultimo;
} TipoListaPeticiones;


/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* ----------------------------------------------*
 * Funciones de gestión de la lista de peticiones
 * ----------------------------------------------*/

/* 
 * Funcion auxiliar para inicializar la lista de peticiones 
 */
int iniciarListaPeticiones(TipoListaPeticiones *lista);

/* 
 * Funcion auxiliar para insertar una peticion al final de la lista 
 */
int insertarListaPeticiones(TipoListaPeticiones *lista, TipoPeticion *peticion);

/* 
 * Funcion auxiliar para obtener una peticion del principio de la lista 
 */
TipoPeticion *extraerListaPeticiones(TipoListaPeticiones *lista);

/* 
 * Funcion auxiliar para busca una peticion al mismo bloque en la lista 
 */
TipoPeticion *buscarListaPeticiones(TipoListaPeticiones *lista, int numBloque);

/*
 * Funcion que muestra los componentes de la lista
 */
int mostrarListaPeticiones(TipoListaPeticiones *lista, char *buffer, int bytesLibres);

/* 
 * Funcion para ver si hay peticiones en la lista 
 */
int estaVaciaListaPeticiones(TipoListaPeticiones *lista);

#endif /* _LISTA_PETICIONES_H_ */

 
