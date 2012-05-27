/*
 *  modulo_teclado.c
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


/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

/*
 * Tipo de los attributos especificos del dispositivo de teclado
 */
typedef struct {
	TipoBufferCaracteres bufferCaracteres;
	TipoListaBCP listaProcesosBloqueados;
} TipoDatosPropiosDispositivo_teclado;


/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/
 
/* -----------------------------------------------------------*
 * Funciones especificas de gestión del DescFichero de teclado
 * -----------------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha creado vacio y lo asocia a "nombre"
 */
int abrirFichero_teclado(int descFichero, char *nombre, int flags)
{	
	/* no hay nada mas que hacer */
	return (0);
}

/*
 * Funcion que lee un buffer de datos del descriptor
 * de fichero.
 */
int leerFichero_teclado(int descFichero, char *buffer, int tamanyo)
{
	int descDispositivo;
	int i;

	/* obtener el desc. del dispositivo */
	descDispositivo = procesoActual->tablaDescFicheros[descFichero].descDispositivo;

	for (i=0;i<tamanyo;i++) {
		/* pedir un caracter por cada posicion del buffer */
		peticionCaracter(descDispositivo, &(buffer[i]), OP_READ);
		/* si la tecla es un enter, terminar */
		if ((char)10 == buffer[i]) {
			break;
		}
	}
	return (i);
}

/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero_teclado(int descFichero)
{
	/* borrar descripor de la tabla */
	liberarEntradaTablaDescFicheros(procesoActual->tablaDescFicheros, descFichero);
	return (0);
}


/* ------------------------------------------------------------------*
 * Funciones especificas de gestión del objeto dispositivo de teclado
 * ------------------------------------------------------------------*/

/*
 * Funcion que crear un nuevo descriptor de fichero de teclado vacio 
 * en la tabla de descriptores de ficheros
 */
int crearDescFicheroDispositivo_teclado(int descDispositivo, TipoTablaDescFicheros tablaDescFicheros)
{
	int descFichero;
	
	/* buscar entrada libre de Desc. de fichero en la tabla del proceso*/
	descFichero = reservarEntradaTablaDescFicheros(tablaDescFicheros);
	
	/* cargar tipo y desc. de dispositivo */
	tablaDescFicheros[descFichero].tipo = T_DISPOSITIVO;
	tablaDescFicheros[descFichero].descDispositivo = descDispositivo;

	/* no hay datos propios */
	tablaDescFicheros[descFichero].datosPropiosDescFichero=NULL;

	/* cargar punteros a las funciones del Desc. de fichero del teclado */
	tablaDescFicheros[descFichero].abrirFichero = abrirFichero_teclado;
	tablaDescFicheros[descFichero].leerFichero = leerFichero_teclado;
	tablaDescFicheros[descFichero].escribirFichero = NULL;
	tablaDescFicheros[descFichero].buscarEnFichero = NULL;
	tablaDescFicheros[descFichero].cerrarFichero = cerrarFichero_teclado;
	tablaDescFicheros[descFichero].mostrarFichero = NULL;

	return (descFichero);
}

/*
 * Funcion que realiza una peticion de lectura/escritura
 * de un caracter al dispositivo de teclado
 */
int peticionCaracter_teclado(int descDispositivo, char *caracter, int operacion)
{ 

	TipoDatosPropiosDispositivo_teclado *datosPropiosDispositivo;
	TipoBCP * proceso;
	int ret;
	
	/* si la operacion NO es de lectura abortar */
	if (operacion != OP_READ) {
		return (-1);
	}
	
	/* obtener datos propios del dispositivo */
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_teclado *) 
							   tablaDispositivos[descDispositivo].datosPropiosDispositivo;
							   
    if ( estaVacioBufferCaracteres(&(datosPropiosDispositivo->bufferCaracteres)) ) {
      	/* si esta vacio bloquear el proceso y seleccionar otro */
		proceso = procesoActual;
		insertarUltimoListaBCP (&(datosPropiosDispositivo->listaProcesosBloqueados),proceso);
		proceso->estado = BLOQUEADO;
		procesoActual = planificador();
		while (procesoActual == NULL) {
			esperaInterrupcion();
			procesoActual=planificador();
		}	
        procesoActual->estado = EJECUCION;

        /* saltar al otro proceso */
        cambio_contexto(&(proceso->contextoRegs), &(procesoActual->contextoRegs));
	}
	
	/* cuando vuelva a ejecutar, obtener caracter y terminar con exito*/
	ret = extraerBufferCaracteres(&(datosPropiosDispositivo->bufferCaracteres),caracter);
	
    return (ret);
}


/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * hardware del Dispositivo de teclado
 */
int interrupcionHW_teclado(int descDispositivo)
{
	TipoDatosPropiosDispositivo_teclado *datosPropiosDispositivo;
	char caracter;
	int hardwareID;
		
	/* obtener identificador hardware del teclado */
	hardwareID = tablaDispositivos[descDispositivo].hardwareID;

	/* chequear que la intrrupcion sea de teclado */
	if (test_and_set_interrupcion(hardwareID)) {
	
		/* obtener datos propios del dispositivo */
		datosPropiosDispositivo = (TipoDatosPropiosDispositivo_teclado *) 
								   tablaDispositivos[descDispositivo].datosPropiosDispositivo;
								   
		/* leer el caracter del puerto de teclado */
		caracter = leer_puerto(hardwareID);
		
		/* insertar caracter en el buffer y activar la interrupcion SW para tratarlo */
		insertarBufferCaracteres(&(datosPropiosDispositivo->bufferCaracteres), caracter);
		insertarUltimaTareaPend(&listaTareasPend, interrupcionSW, 
							    (void *) descDispositivo);
		activar_int_SW();
		return (0);
	} else {
		return (-1);
	}
}

/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * software desde la interrupcion HW del Dispositivo de teclado
 * para despertar procesos bloqueados esperando una tecla
 */
void interrupcionSW_teclado(int descDispositivo)
{
	TipoDatosPropiosDispositivo_teclado *datosPropiosDispositivo;
    TipoBCP *proceso;
	
	/* obtener datos propios del dispositivo */
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_teclado *) 
							   tablaDispositivos[descDispositivo].datosPropiosDispositivo;

	/* recorrer la lista de procesos esperando y despertar al primero */
    proceso = primeroListaBCP(&(datosPropiosDispositivo->listaProcesosBloqueados));
    if  (proceso != NULL) {
         eliminarPrimeroListaBCP(&(datosPropiosDispositivo->listaProcesosBloqueados));
         proceso->estado = LISTO;
         insertarListaListos(proceso);			
	}
}
	

/*
 * Funcion que muestra los componentes de un inodo de debugFS
 */
int mostrarDispositivo_teclado(int descDispositivo, char *buffer, int bytesLibres)
{
	int libres=bytesLibres;
	TipoDatosPropiosDispositivo_teclado *datosPropiosDispositivo;

	if ( (NULL == buffer) || (0 >= bytesLibres) ) {
		return (0);
	}
	/* obtener datos propios del dispositivo */
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_teclado *)
						 tablaDispositivos[descDispositivo].datosPropiosDispositivo;

	/* obtener los datos a imprimir de lista de procesos y de caracteres */ 
	libres=sprintk_concat(buffer, libres,"\t\t");
	libres=mostrarListaBCP(&(datosPropiosDispositivo->listaProcesosBloqueados),buffer,libres);
	libres=sprintk_concat(buffer, libres,"\t\t");
	libres=mostrarBufferCaracteres(&(datosPropiosDispositivo->bufferCaracteres),buffer,libres);

	return (libres);
}


/* -------------------------------------------------------------*
 * Funciones especificas de gestión del objeto driver de teclado
 * -------------------------------------------------------------*/

/*
 * Funcion que crear un nuevo Dispositivo vacio 
 * en la tabla de Dispositivos
 */
int crearDispositivo_teclado(int descDriver, char *nombreDispositivo, int hardwareID)
{
	int descDispositivo;
	TipoDatosPropiosDispositivo_teclado *datosPropiosDispositivo;
	int ret;	
	
	/* enchufar el Hardware */
	ret = plug_hardware (hardwareID, NULL);
	if (-1 == ret) {
		printk ("Error al activar el teclado\n");
		return (-1);
	}

	/* buscar entrada libre de Dispositivo en la tabla */
	descDispositivo = reservarEntradaTablaDispositivos(nombreDispositivo);
	
	/* cargar descriptor de Driver e identificador de hardware */
	tablaDispositivos[descDispositivo].descDriver = descDriver;
	tablaDispositivos[descDispositivo].hardwareID = hardwareID;

	/* crear y cargar datos propios del teclado (lista caracteres y lista procesos) */
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_teclado *) 
							   malloc(sizeof(TipoDatosPropiosDispositivo_teclado));

	iniciarBufferCaracteres(&(datosPropiosDispositivo->bufferCaracteres));
	inicializarListaBCP(&(datosPropiosDispositivo->listaProcesosBloqueados));
	
	tablaDispositivos[descDispositivo].datosPropiosDispositivo = (void *) datosPropiosDispositivo;

	/* cargar punteros a las funciones del Dispositivo teclado */
	tablaDispositivos[descDispositivo].crearDescFicheroDispositivo = crearDescFicheroDispositivo_teclado;
	tablaDispositivos[descDispositivo].peticionCaracter = peticionCaracter_teclado;
	tablaDispositivos[descDispositivo].peticionBloque = NULL;
	tablaDispositivos[descDispositivo].interrupcionHW = interrupcionHW_teclado;
	tablaDispositivos[descDispositivo].interrupcionSW = interrupcionSW_teclado;
	tablaDispositivos[descDispositivo].destruirDispositivo = NULL;
	tablaDispositivos[descDispositivo].mostrarDispositivo = mostrarDispositivo_teclado;

	return (descDispositivo);
}

/*
 * Funcion que destruye un objeto Driver de teclado y lo
 * elimina de la tabla
 */
int destruirDriver_teclado(int descDriver)
{
	/* no se puede destruir el driver de teclado */
	return (-1);
}


/* -----------------------------------------------------*
 * Funciones especificas de cargar y descarga de modulos
 * -----------------------------------------------------*/

/*
 * Funcion que inicializa el modulo y carga el objeto Driver de teclado
 * en la tabla de Drivers
 */
int cargarModulo_teclado ()
{ 
	int descDriver;

	/* buscar entrada libre de Driver en la tabla */
	descDriver = reservarEntradaTablaDrivers("teclado");

	/* no hay datos propios */
	tablaDrivers[descDriver].datosPropiosDriver = NULL;

	/* cargar punteros a las funciones del driver de teclado */
	tablaDrivers[descDriver].crearDispositivo = crearDispositivo_teclado;
	tablaDrivers[descDriver].destruirDriver = destruirDriver_teclado;
	tablaDrivers[descDriver].mostrarDriver = NULL;
	return (0);
}
