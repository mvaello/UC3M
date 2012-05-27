/*
 *  bufferCaracteres.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <string.h>
#include "minikernel.h"


/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* 
 * Funcion auxiliar para inicializar el buffer de caracteres 
 */
void iniciarBufferCaracteres(TipoBufferCaracteres *buffer)
{
	buffer->primero = 0;
    buffer->ultimo = 0;
}

/* 
 * Funcion auxiliar para insertar un caracter al final del buffer
 */
void insertarBufferCaracteres(TipoBufferCaracteres *buffer, char caracter)
{
	if (buffer->primero == (buffer->ultimo+1) % TAM_BUFFER_CARACTERES) {
        /* si el buffer esta llena, error */
        printk("-> ERROR: BUFFER CARACTERES LLENO, SE DESCARTA\n");
    } else {
        /* cargar la entrada y apuntar que hay uno mas */
        buffer->caracter[buffer->ultimo] = caracter;
        buffer->ultimo = (buffer->ultimo+1) % TAM_BUFFER_CARACTERES;
    }
}

/* 
 * Funcion auxiliar para obtener un caracter del principio del buffer 
 */
int extraerBufferCaracteres(TipoBufferCaracteres *buffer, char *caracter)
{
	if (buffer->primero == buffer->ultimo) {
        /* si el buffer esta vacia, error */
        printk("-> ERROR: BUFFER TECLAS VACIO\n");
		return (-1);
    } else {
        /* obtener la entrada y apuntar que hay uno menos */
        (*caracter) = buffer->caracter[buffer->primero];
        buffer->primero = (buffer->primero+1) % TAM_BUFFER_CARACTERES;
		return (0);
    }
}

/*
 * Funcion que muestra los componentes del buffer de caracteres
 */
int mostrarBufferCaracteres(TipoBufferCaracteres *bufferCar, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int aux;
	int totalCar=0;

	if ( (NULL == bufferCar) || (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
    libres=sprintk_concat(buffer, libres,"buffer caracteres(max=%d): ",TAM_BUFFER_CARACTERES);
	if (bufferCar->primero == bufferCar->ultimo) {
        /* el buffer esta vacia */
        libres=sprintk_concat(buffer, libres,"VACIO\n");
    } else {
		aux = bufferCar->primero;
		while (aux != bufferCar->ultimo) {
			libres=sprintk_concat(buffer, libres,"'%c', ",bufferCar->caracter[aux]);
			aux = (aux+1) % TAM_BUFFER_CARACTERES;
			totalCar=totalCar+1;
		}
		libres=sprintk_concat(buffer, libres,": Total=%d\n",totalCar);
	}
	return (libres);
}


/* 
 * Funcion para ver si hay caracteres en el buffer
 */
int estaVacioBufferCaracteres(TipoBufferCaracteres *buffer)
{
	return (buffer->primero == buffer->ultimo);
}


