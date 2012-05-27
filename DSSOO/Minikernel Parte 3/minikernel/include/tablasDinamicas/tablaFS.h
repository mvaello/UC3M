/*
 *  tablaFS.h
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#ifndef _TABLA_FS_H_
#define _TABLA_FS_H_

#include "nucleoBasico/const.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/

#define MAX_TABLA_FS 256

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Definición del objeto FS
 */
 
typedef struct {
	/* atributos */
	int ocupado;
	char nombreFS[MAX_STRING];
    void *	datosPropiosFS;
	/* metodos */
	int (*crearMontaje) (int, char *);
	int (*destruirFS) (int);
	int (*mostrarFS) (int, char *, int);
}	TipoFS; 

 
/***************************************************************
 * EXPORTAR VARIABLES GLOBALES DECLARADAS EN EL FICHERO C
 ***************************************************************/
/*
 * Variable global de la tabla de FS
 */
 
extern TipoFS tablaFS[MAX_TABLA_FS];
 
 
/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* --------------------------------------------*
 * Funciones genericas de gestión del objeto FS
 * --------------------------------------------*/

/*
 * Funcion que crear un nuevo Montaje vacio 
 * en la tabla de Montaje
 */
int crearMontaje(int descFS, char *nombreMontaje);

/*
 * Funcion que destruye un objeto FS y lo
 * elimina de la tabla
 */
int destruirFS(int descFS);

/*
 * Funcion que muestra los componentes de un FS
 */
int mostrarFS(int descFS, char *buffer, int bytesLibres);

/* ---------------------------------------*
 * Funciones de gestión de la tabla de FS
 * ---------------------------------------*/

/*
 * Funcion que inicializa la tabla de FS
 */
int inicializarTablaFS();

/*
 * Funcion que reserva una entrada en la tabla de 
 * de FS para el objeto "nombreFS"
 */
int reservarEntradaTablaFS(char *nombreFS);

/*
 * Funcion que Busca un FS en la tabla de FS
 * por el nombre.
 */
int buscarEntradaTablaFS(char *nombreFS);

/*
 * Funcion que muestra la tabla de FS
 */
int mostrarTablaFS(char *buffer, int bytesLibres);

/*
 * Funcion que libera una FS de la tabla de FS
 */
int liberarEntradaTablaFS(int descFS);

#endif /* _TABLA_FS_H_ */

 
