/*
 *  modulo_debugFS.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 ARCOS. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include "minikernel.h"


/***************************************************************
 * CONSTANTES
 ***************************************************************/

/* tamaño del buffer temporal */
#define TAM_BUFFER_TEMP  TAM_BLOQUE*16

/* numero de ficheros existente */
#define NUM_FICHEROS_DEBUGFS  7

/* nombres de los ficheros existentes */
#define FICH_TABLA_PROCESOS "tabla_procesos"
#define FICH_LISTA_PROCESOS_LISTOS "lista_procesos_listos"
#define FICH_TABLA_DRIVERS "tabla_drivers"
#define FICH_TABLA_DISPOSITIVOS "tabla_dispositivos"
#define FICH_TABLA_FS "tabla_fs"
#define FICH_TABLA_MONTAJES "tabla_montajes"
#define FICH_TABLA_INODOS "tabla_inodos"

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Tipo de los attributos especificos del objeto inodo de debugFS
 */
typedef struct {
	char nombre[MAX_STRING];
} TipoDatosPropiosInodo_debugFS;


/*
 * Tipo de los attributos especificos de descriptor de fichero de debugFS
 */
typedef struct {
	int puntero;
} TipoDatosPropiosDescFichero_debugFS;

/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/
 
/* -------------------------------------------------------------------*
 * Funciones especificas de gestión del objeto DescFichero de debugFS
 * -------------------------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha cleero vacio y lo asocia a "nombre"
 */
int abrirFichero_debugFS(int descFichero, char *nombre, int flags)
{
	int descMontaje, descInodo;
	TipoDatosPropiosDescFichero_debugFS *datosPropiosDescFichero;

	
	/* obtener los datos propios del descriptor de fichero */
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_debugFS *) 
							  procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	/* obtener el descriptor del montaje */
	descMontaje = procesoActual->tablaDescFicheros[descFichero].descMontaje;
	
	/* buscar el inodo con el nombre buscado */
	descInodo = buscarNombreInodo(descMontaje, nombre);
	if (-1 == descInodo) {
		return (-1);
	}
	
	/* asignar el inodo al descriptor */
	procesoActual->tablaDescFicheros[descFichero].descInodo = descInodo;
	
	/* poner puntero a cero */
	datosPropiosDescFichero->puntero=0;

	return (0);
}

/*
 * Funcion que lee un buffer de datos del descriptor
 * de fichero.
 */
int leerFichero_debugFS(int descFichero, char *buffer, int tamanyo)
{
	int descInodo;
	TipoDatosPropiosDescFichero_debugFS *datosPropiosDescFichero;
	TipoDatosPropiosInodo_debugFS *datosPropiosInodo;
	char nombre[MAX_STRING];
	char datos[TAM_BUFFER_TEMP];
	int libres=TAM_BUFFER_TEMP;
	int puntero;
	
	/* obtener los datos propios del inodo del fichero para sacar el nombre */
	descInodo = procesoActual->tablaDescFicheros[descFichero].descInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_debugFS *) tablaInodos[descInodo].datosPropiosInodo;
	strcpy(nombre,datosPropiosInodo->nombre);
	
	/* obtener los datos propios del descriptor de fichero */
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_debugFS *) 
							  procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;
							  
	/* copia temporal del puntero del fichero */
	puntero = datosPropiosDescFichero->puntero;

	/* limpiar el buffer temporal */
	bzero(datos,TAM_BUFFER_TEMP);

	/* Elegir el fichero y conseguir el buffer de datos */
	if (0 == strcmp(nombre,FICH_TABLA_PROCESOS)) {
		libres = mostrarTablaProcesos(datos, libres);
		
	} else if (0 == strcmp(nombre,FICH_LISTA_PROCESOS_LISTOS)) {
		libres = mostrarListaListos(datos, libres);
		
	} else if (0 == strcmp(nombre,FICH_TABLA_DRIVERS)) {
		libres = mostrarTablaDrivers(datos, libres);
		
	} else if (0 == strcmp(nombre,FICH_TABLA_DISPOSITIVOS)) {
		libres = mostrarTablaDispositivos(datos, libres);
		
	} else if (0 == strcmp(nombre,FICH_TABLA_FS)) {
		libres = mostrarTablaFS(datos, libres);
		
	} else if (0 == strcmp(nombre,FICH_TABLA_MONTAJES)) {
		libres = mostrarTablaMontajes(datos, libres);
		
	} else if (0 == strcmp(nombre,FICH_TABLA_INODOS)) {
		libres = mostrarTablaInodos(datos, libres);
		
	} else {
		return (-1);
	}
	
	/* limpiar el buffer de salida */
	bzero(buffer,tamanyo);

	/* copiar los datos leidos */
	if (strlen(datos) <= puntero) {
		/* El puntero apunta mas alla del final */
		return (0);
	} else if (strlen(datos) < (puntero + tamanyo)) {
		/* existen menos datos que los requeridos */
		memcpy(buffer,&(datos[puntero]),strlen(datos)-puntero);
		datosPropiosDescFichero->puntero=strlen(datos);
		return (strlen(datos)-puntero);
	} else {
		/* existen todos los datos requeridos */
		memcpy(buffer,&(datos[puntero]),tamanyo);
		datosPropiosDescFichero->puntero=puntero + tamanyo;
		return (tamanyo);
	}
}


/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero_debugFS(int descFichero)
{
	/* liberar el descriptor de la tabla */
	liberarEntradaTablaDescFicheros(procesoActual->tablaDescFicheros, descFichero);
	return (0);
}

/*
 * Funcion que muestra los componentes de un desc de fichero de debugFS
 */
int mostrarFichero_debugFS(void *infoFichero, int descFichero, char *buffer, int bytesLibres)
{
	TipoDescFichero *AuxFichero;
	TipoDatosPropiosDescFichero_debugFS *datosPropiosDescFichero;
	int libres=bytesLibres;

	if ( (NULL == infoFichero) || (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	
	/* obtener el desc. de fichero entero */
	/* asi se pueden ver desc. de ficheros que no sean del proceso actual */
	AuxFichero=(TipoDescFichero *)infoFichero;	

	/* obtener los datos propios del descriptor de fichero */
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_debugFS *) 
							  AuxFichero->datosPropiosDescFichero;
				
	/* obtener los datos a imprimir del puntero del desc. de fichero */ 
	libres=sprintk_concat(buffer, libres,"\t\tpuntero=%d\n", (int)(datosPropiosDescFichero->puntero));

	return (libres);
}


/* -------------------------------------------------------------*
 * Funciones especificas de gestión del objeto Inodo de debugFS
 * -------------------------------------------------------------*/

/*
 * Funcion que inicializa un nuevo inodo de debugFS
 * en la tabla de inodos
 */
int iniciarNuevoInodo_debugFS(int descInodo, char *nombre, int flags)
{
	TipoDatosPropiosInodo_debugFS *datosPropiosInodo;

	/* obtener los datos propios del inodo */
	datosPropiosInodo = (TipoDatosPropiosInodo_debugFS *)
						 tablaInodos[descInodo].datosPropiosInodo;

	/* copiar el nombre al inodo */
	strcpy(datosPropiosInodo->nombre,nombre);
	
	return (0);
}
	
/*
 * Funcion que muestra los componentes de un inodo de debugFS
 */
int mostrarInodo_debugFS(int descInodo, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	TipoDatosPropiosInodo_debugFS *datosPropiosInodo;

	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	/* obtener los datos propios del inodo */
	datosPropiosInodo = (TipoDatosPropiosInodo_debugFS *)
						 tablaInodos[descInodo].datosPropiosInodo;

	/* obtener los datos a imprimir del nombre del fichero */ 
	libres=sprintk_concat(buffer, libres,"\tnombre=%s\n", datosPropiosInodo->nombre);

	return (libres);
}


/* ----------------------------------------------------------------*
 * Funciones especificas de gestión del objeto Montaje del debugFS
 * ----------------------------------------------------------------*/

/*
 * Funcion que monta un volumen formateado con el debugFS   
 */
int montarVolumen_debugFS(int descMontaje, char *nombreDispositivo)
{
	int descInodo;
	
	/* crear los inodos de los ficheros existentes*/

	/* fichero de la tabla de Procesos */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_TABLA_PROCESOS,0);
	
	/* fichero de la lista de procesos listos */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_LISTA_PROCESOS_LISTOS,0);

	/* fichero de la tabla de drivers */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_TABLA_DRIVERS,0);

	/* fichero de la tabla de dispositivos */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_TABLA_DISPOSITIVOS,0);

	/* fichero de la tabla de FS */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_TABLA_FS,0);

	/* fichero de la tabla de montajes */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_TABLA_MONTAJES,0);

	/* fichero de la tabla de inodos */
	descInodo = crearInodo(descMontaje, 0);
	iniciarNuevoInodo(descInodo, FICH_TABLA_INODOS,0);
	
	return (0);
}



/*
 * Funcion que busca un inodo de debugFS con el nombre correspondiente 
 * del volumen en la tabla de Inodos
 */
int buscarNombreInodo_debugFS(int descMontaje, char *nombre)
{
	int descInodo;
	TipoDatosPropiosInodo_debugFS *datosPropiosInodo;

	/* recorre la tabla de inodos y ver solo los del montaje de debugFS */
	for (descInodo=0; descInodo < MAX_TABLA_INODOS; descInodo++) {
		if  (  ( TRUE == tablaInodos[descInodo].ocupado) &&
			   (tablaInodos[descInodo].descMontaje == descMontaje) ) {

			/* obtener los datos propios del inodo */
			datosPropiosInodo = (TipoDatosPropiosInodo_debugFS *)
								tablaInodos[descInodo].datosPropiosInodo;

			/* si el nombre coincide devolverlo y terminar */ 
			if (0 == strcmp(datosPropiosInodo->nombre,nombre)) {
				return (descInodo);
			}
		}
	}
	/* Si llega hasta aqui es que no lo encontro */
	return (-1);
}

/*
 * Funcion que crear un nuevo Inodo vacio de debugFS
 * en la tabla de Inodos
 */
int crearInodo_debugFS(int descMontaje, int numInodo)
{
	int descInodo;
	TipoDatosPropiosInodo_debugFS *datosPropiosInodo;

	/* ver si el numero de inodo es correcto */
	if ( (0 > numInodo) || (numInodo >= NUM_FICHEROS_DEBUGFS) ) {
	 		return (-1);
	}

	/* buscar entrada libre de inodo en la tabla */
	descInodo = reservarEntradaTablaInodos(descMontaje, numInodo);
	
	/* crear y cargar datos propios del inodo */
	datosPropiosInodo = (TipoDatosPropiosInodo_debugFS *)
						 malloc(sizeof(TipoDatosPropiosInodo_debugFS));
	tablaInodos[descInodo].datosPropiosInodo=(void *) datosPropiosInodo;

	/* cargar punteros a las funciones del inodo */
	tablaInodos[descInodo].iniciarNuevoInodo = iniciarNuevoInodo_debugFS;
	tablaInodos[descInodo].leerInodo = NULL;
	tablaInodos[descInodo].mapearBloque = NULL;
	tablaInodos[descInodo].asignarNuevoBloque = NULL;
	tablaInodos[descInodo].buscarNombreEnDir = NULL;
	tablaInodos[descInodo].escribirInodo = NULL;
	tablaInodos[descInodo].borrarInodo = NULL;
	tablaInodos[descInodo].mostrarInodo = mostrarInodo_debugFS;

	return (descInodo);
}

/*
 * Funcion que crear un nuevo descriptor de fichero vacio de debugFS
 * en la tabla de descriptores de ficheros del proceso
 */
int crearDescFichero_debugFS(int descMontaje, TipoTablaDescFicheros tablaDescFicheros)
{
	int descFichero;
	TipoDatosPropiosDescFichero_debugFS *datosPropiosDescFichero;
	
	/* buscar entrada libre de Desc. de fichero en la tabla del proceso*/
	descFichero = reservarEntradaTablaDescFicheros(tablaDescFicheros);

	/* cargar tipo y desc. de montaje */
	tablaDescFicheros[descFichero].tipo= T_FICHERO;
	tablaDescFicheros[descFichero].descMontaje= descMontaje;

	/* crear y cargar datos propios del desc. de fichero */
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_debugFS *)
							   malloc(sizeof(TipoDatosPropiosDescFichero_debugFS));
	tablaDescFicheros[descFichero].datosPropiosDescFichero= (void *) datosPropiosDescFichero;

	/* cargar punteros a las funciones del desc. de fichero */
	tablaDescFicheros[descFichero].abrirFichero = abrirFichero_debugFS;
	tablaDescFicheros[descFichero].leerFichero = leerFichero_debugFS;
	tablaDescFicheros[descFichero].escribirFichero = NULL;
	tablaDescFicheros[descFichero].buscarEnFichero = NULL;
	tablaDescFicheros[descFichero].cerrarFichero = cerrarFichero_debugFS;
	tablaDescFicheros[descFichero].mostrarFichero = mostrarFichero_debugFS;

	return (descFichero);
}


/* -------------------------------------------------------*
 * Funciones especificas de gestión del objeto FS de disco
 * -------------------------------------------------------*/

/*
 * Funcion que crear un nuevo Montaje vacio de debugFS 
 * en la tabla de Montajes
 */
int crearMontaje_debugFS(int descFS, char *nombreMontaje)
{
	int descMontaje;
	
	/* buscar entrada libre de Montaje en la tabla */
	descMontaje = reservarEntradaTablaMontajes(nombreMontaje);
	
	/* cargar descriptor de FS */
	tablaMontajes[descMontaje].descFS = descFS;

	/* no hay datos propios */
	tablaMontajes[descMontaje].datosPropiosMontaje = NULL;

	/* cargar punteros a las funciones del montajeo */
	tablaMontajes[descMontaje].montarVolumen = montarVolumen_debugFS;
	tablaMontajes[descMontaje].reservarBloque = NULL;
	tablaMontajes[descMontaje].liberarBloque = NULL;
	tablaMontajes[descMontaje].reservarInodo = NULL;
	tablaMontajes[descMontaje].liberarInodo = NULL;
	tablaMontajes[descMontaje].buscarNombreInodo = buscarNombreInodo_debugFS;
	tablaMontajes[descMontaje].crearInodo = crearInodo_debugFS;
	tablaMontajes[descMontaje].crearDescFichero = crearDescFichero_debugFS;
	tablaMontajes[descMontaje].sincronizarVolumen = NULL;
	tablaMontajes[descMontaje].desmontarVolumen = NULL;
	tablaMontajes[descMontaje].mostrarMontaje = NULL;

	return (descMontaje);
}


/* -----------------------------------------------------*
 * Funciones especificas de cargar y descarga de modulos
 * -----------------------------------------------------*/

/*
 * Funcion que inicializa el modulo y carga el objeto FS  de debugFS
 * en la tabla de sistemºas de ficheros
 */
int cargarModulo_debugFS ()
{ 
	int descFS;

	/* buscar entrada libre de FS en la tabla */
	descFS = reservarEntradaTablaFS("debugFS");
	
	/* no hay datos propios */
	tablaFS[descFS].datosPropiosFS = NULL;

	/* cargar punteros a las funciones del driver de teclado */
	tablaFS[descFS].crearMontaje = crearMontaje_debugFS;
	tablaFS[descFS].destruirFS = NULL;
	tablaFS[descFS].mostrarFS = NULL;
	return (0);
}



