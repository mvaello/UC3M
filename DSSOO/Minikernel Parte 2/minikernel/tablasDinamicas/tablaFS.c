/*
 *  tablaFS.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#include "minikernel.h"
#include "string.h"
#include "stdlib.h"


/***************************************************************
 * VARIABLES GLOBALES
 ***************************************************************/
/*
 * Variable global de la tabla de FS
 */
 
TipoFS tablaFS[MAX_TABLA_FS];
 
/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/

/* ------------------------------------------------*
 * Funciones genericas de gestión del objeto FS
 * ------------------------------------------------*/

/*
 * Funcion que crear un nuevo Montaje vacio 
 * en la tabla de Montajes
 */
int crearMontaje(int descFS, char *nombreMontaje)
{
	int descMontaje;
	
	if ( (0 > descFS) || (MAX_TABLA_FS <= descFS) ||
		 (NULL == nombreMontaje) ||
		 (NULL == tablaFS[descFS].crearMontaje) ) {
		return (-1);
	}
	descMontaje = tablaFS[descFS].crearMontaje(descFS,nombreMontaje);
	return (descMontaje);
}
/*
 * Funcion que destruye un objeto FS y lo
 * elimina de la tabla
 */
int destruirFS(int descFS)
{
	int ret;
	
	if ( (0 > descFS) || (MAX_TABLA_FS <= descFS)  ||
		 (NULL == tablaFS[descFS].destruirFS) ) {
		return (-1);
	}
	ret=tablaFS[descFS].destruirFS(descFS);
	return (ret);
}

/*
 * Funcion que muestra los componentes de un FS
 */
int mostrarFS(int descFS, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;

	if ( (NULL == buffer) || (0 >= bytesLibres) ||
		 (0 > descFS) || (MAX_TABLA_FS <= descFS) ) {
		return (0);
	}
	
	/* mostart componentes generales */
	libres=sprintk_concat(buffer, libres,"FS:\tID=%d, nombre=%s\n",
						  descFS, tablaFS[descFS].nombreFS);		
	/*mostrar componentes especificos */
	if (NULL != tablaFS[descFS].mostrarFS) {
		libres=tablaFS[descFS].mostrarFS(descFS, buffer, libres);
	}
	return (libres);
}


/* -------------------------------------------*
 * Funciones de gestión de la tabla de FS
 * -------------------------------------------*/

/*
 * Funcion que inicializa la tabla de FS
 */
int inicializarTablaFS()
{
	int i;
	
	for (i=0;i<MAX_TABLA_FS;i++) {
		tablaFS[i].ocupado = FALSE;
		strcpy(tablaFS[i].nombreFS,"");
		tablaFS[i].datosPropiosFS = NULL;
		tablaFS[i].crearMontaje = NULL;
		tablaFS[i].destruirFS = NULL;
		tablaFS[i].mostrarFS = NULL;
	}
	return (0);
}
	
/*
 * Funcion que reserva una entrada en la tabla de 
 * de FS para el objeto "nombreFS"
 */
int reservarEntradaTablaFS(char *nombreFS)
{
	int descFS = 0;

	if (NULL == nombreFS) {
		return (-1);
	}
	while ( (TRUE == tablaFS[descFS].ocupado) &&
			(MAX_TABLA_FS > descFS) ) {
		descFS = descFS + 1;
	}
	if (MAX_TABLA_FS > descFS) {
		tablaFS[descFS].ocupado = TRUE;
		strcpy(tablaFS[descFS].nombreFS,nombreFS);
		return (descFS);
	}
	return (-1);
}

/*
 * Funcion que Busca un FS en la tabla de FS
 * por el nombre.
 */
int buscarEntradaTablaFS(char *nombreFS)
{
	int descFS = 0;

	if (NULL == nombreFS) {
		return (-1);
	}
	while ( ( (FALSE == tablaFS[descFS].ocupado) || 
		      (0 != strcmp(tablaFS[descFS].nombreFS,nombreFS)) ) &&
			(MAX_TABLA_FS > descFS) ) {
		descFS = descFS + 1;
	}
	if (MAX_TABLA_FS <= descFS) {
		return (-1);
	} else {
		return (descFS);
	}
}

/*
 * Funcion que muestra la tabla de FS
 */
int mostrarTablaFS(char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	int i;
	
	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	libres=sprintk_concat(buffer, libres, "TABLA DE FS\n"); 

	for (i=0; i<MAX_TABLA_FS; i++) {
		if (TRUE == tablaFS[i].ocupado) {
			libres=mostrarFS(i, buffer, libres);
		}
	}
	return (libres);
}


/*
 * Funcion que libera una driver de la tabla de FS
 */
int liberarEntradaTablaFS(int descFS)
{
	tablaFS[descFS].ocupado = FALSE;
	strcpy(tablaFS[descFS].nombreFS,"");
	free(tablaFS[descFS].datosPropiosFS);
	tablaFS[descFS].datosPropiosFS = NULL;
	tablaFS[descFS].crearMontaje = NULL;
	tablaFS[descFS].destruirFS = NULL;
	tablaFS[descFS].mostrarFS = NULL;
	return (0);
}


