/*
 *  LlamadasSistema.c
 *  
 *
 *  Created by Javier Fernandez Muñoz on 08/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "minikernel.h"
#include <string.h>

/***************************************************************
 * VARIABLES GLOBALES
 ***************************************************************/
 
/*
 * Variable global que contiene las rutinas que realizan cada llamada
 */
servicio tablaServicios[NUMERO_SERVICIOS]={	
					{sis_crearProceso},
					{sis_crearProcesoAltaPrioridad},
					{sis_terminarProceso},
					{sis_abrir},
					{sis_leer},
					{sis_escribir},
					{sis_buscar},
					{sis_cerrar},
					{sis_montar},
					{sis_sincronizar},
					{sis_desmontar},
					{sis_pid},
					{sis_time},
					{sis_sleep} };


/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/


/*
 * Funcion principal a la que se llama para 
 * el tratamiento de cualquier llamada al sistema
 */
void tratarLlamadaSistema()
{
	int numServicio, ret;

	numServicio=leer_registro(0);
	if (numServicio < NUMERO_SERVICIOS)
		ret=(tablaServicios[numServicio].funServicio)();
	else
		ret=-1;		/* servicio no existente */
	escribir_registro(0,ret);
	return;
}

/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_crearProceso
 */
int sis_crearProceso()
{
	char *programa;
	int ret;

	printk("-> PROC %d: CREAR PROCESO\n", procesoActual->id);
	programa=(char *)leer_registro(1);
	/* ret=crearTarea(programa); */
	ret=crearTarea(programa, PRIORIDAD_NORMAL); /* cambio */
	return ret;
}

/* 
 * LLAMADA AL SISTEMA: sis_crearProceso
 */
int sis_crearProcesoAltaPrioridad()
{
	char *programa;
	int ret;

	printk("-> PROC %d: CREAR PROCESO\n", procesoActual->id);
	programa=(char *)leer_registro(1);
	/* ret=crearTarea(programa); */
	ret=crearTarea(programa, PRIORIDAD_ALTA); /* cambio */
	return ret;
}

/*
 * Funcion auxiliar que crea un proceso reservando sus recursos.
 * Usada por llamada crear_proceso.
 */
/* int crearTarea(char *programa){ */
int crearTarea(char *programa, int prioridad)
{ 
	void * imagen, *pc_inicial;
	int error=0;
	int numProceso;
	TipoBCP *proceso;

	numProceso=buscarBCPLibreTablaProcesos();
	if (numProceso==-1) {
		return -1;	/* no hay entrada libre */
	}
	/* A rellenar el BCP ... */
	proceso=&(tablaProcesos[numProceso]);

	/* crea la imagen de memoria leyendo ejecutable */
	imagen=crear_imagen(programa, &pc_inicial);
	if (imagen)	{
		/* asignar imagen de memoria */
		proceso->infoMemoria=imagen;
		proceso->pila=crear_pila(TAM_PILA);
		/* crear estado inicial de los registros del procesador */ 
		fijar_contexto_ini(proceso->infoMemoria, proceso->pila, TAM_PILA,
						   pc_inicial, &(proceso->contextoRegs));
		/*copiar tabla de descriptores de ficheros del padre */ 
		inicializarTablaDescFicheros(proceso->tablaDescFicheros);
		if (NULL != procesoActual) {
			memcpy(proceso->tablaDescFicheros, procesoActual->tablaDescFicheros,
				    MAX_TABLA_DESCFICHEROS * sizeof(TipoDescFichero) );
		}
		/* asignar el identificador de proceso */
		proceso->id=numProceso;
		/* asignar el estado inicial a LISTO */
		proceso->estado=LISTO;
		/* asignar la prioridad */
		proceso->prioridad=prioridad;
		/* lo inserta al final de cola de listos */
		insertarPrioridadesListaBCP(proceso);
		error= 0;
	} else {
		error= -1; /* fallo al crear imagen */
	}
	return error;
}

/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_terminarProceso
 */
int sis_terminarProceso()
{
	printk("-> FIN PROCESO %d\n", procesoActual->id);
	liberarProceso();
	return (0); /* no deberÌa llegar aqui */
}


/*
 * Funcion auxiliar que termina proceso actual liberando sus recursos.
 * Usada por llamada terminar_proceso y por rutinas que tratan excepciones
 */
void liberarProceso()
{
	TipoBCP * procesoAnterior;

	liberar_imagen(procesoActual->infoMemoria); /* liberar mapa */
	procesoActual->estado=TERMINADO;

	/* Realizar cambio de contexto */
	procesoAnterior=procesoActual;
	procesoActual=planificador();
	while (procesoActual == NULL) {
		//printk("-> liberarProceso: esperaInterrupcion()\n");
		esperaInterrupcion();
        //printk("-> liberarProceso: FIN esperaInterrupcion()\n");
		procesoActual=planificador();
	}	

	printk("-> C.CONTEXTO POR FIN: de %d a %d\n",
			procesoAnterior->id, procesoActual->id);

	liberar_pila(procesoAnterior->pila);
	cambio_contexto(NULL, &(procesoActual->contextoRegs));
	
	return; /* no deberÌa llegar aqui */
}

/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_abrir
 */
int sis_abrir()
{
	char *nombre;
	int flags;
	int descFichero;

	printk("-> Llamada abrir. id= %d\n", procesoActual->id);
	nombre=(char *)leer_registro(1);
	flags=(int)leer_registro(2);

	descFichero = abrirFichero(nombre,flags);
	return (descFichero);
}

/* 
 * LLAMADA AL SISTEMA: sis_leer
 */
int sis_leer()
{
	int descFichero;
	char *buffer;
	int tamanyo;
	int ret;

	printk("-> Llamada leer. id= %d\n", procesoActual->id);
	descFichero=(int)leer_registro(1);
	buffer=(char *)leer_registro(2);
	tamanyo=(int)leer_registro(3);

	ret = leerFichero(descFichero,buffer,tamanyo);
	return (ret);
}

/* 
 * LLAMADA AL SISTEMA: sis_escribir
 */
int sis_escribir()
{
	int descFichero;
	char *buffer;
	int tamanyo;
	int ret;

	printk("-> Llamada escribir. id= %d\n", procesoActual->id);
	descFichero=(int)leer_registro(1);
	buffer=(char *)leer_registro(2);
	tamanyo=(int)leer_registro(3);

	ret = escribirFichero(descFichero,buffer,tamanyo);
	return (ret);
}

/* 
 * LLAMADA AL SISTEMA: sis_buscar
 */
int sis_buscar()
{
	int descFichero;
	int offset;
	int ret;

	printk("-> Llamada buscar. id= %d\n", procesoActual->id);
	descFichero=(int)leer_registro(1);
	offset=(int)leer_registro(2);

	ret = buscarEnFichero(descFichero,offset);
	return (ret);
}

/* 
 * LLAMADA AL SISTEMA: sis_cerrar
 */
int sis_cerrar()
{
	int descFichero;
	int ret;

	printk("-> Llamada cerrar. id= %d\n", procesoActual->id);
	descFichero=(int)leer_registro(1);

	ret = cerrarFichero(descFichero);
	return (ret);
}


/*---------------------------------------------------*/

/* 
 * LLAMADA AL SISTEMA: sis_montar
 */
int sis_montar()	
{
	char *nombreDispositivo;
	char *nombreMontaje;
	char *nombreFS;
	int ret;

	printk("-> Llamada montar. id= %d\n", procesoActual->id);
	nombreDispositivo=(char *)leer_registro(1);
	nombreMontaje=(char *)leer_registro(2);
	nombreFS=(char *)leer_registro(3);

	ret = montarVolumen(nombreDispositivo, nombreMontaje,nombreFS);
	return (ret);
}

/* 
 * LLAMADA AL SISTEMA: sis_sincronizar
 */
int sis_sincronizar()
{
	char *nombreMontaje;
	int ret;

	printk("-> Llamada sincronizar. id= %d\n", procesoActual->id);
	nombreMontaje=(char *)leer_registro(1);

	ret = sincronizarVolumen(nombreMontaje);
	return (ret);
}

/* 
 * LLAMADA AL SISTEMA: sis_desmontar
 */
int sis_desmontar()
{
	char *nombreMontaje;
	int ret;

	printk("-> Llamada desmontar. id= %d\n", procesoActual->id);
	nombreMontaje=(char *)leer_registro(1);

	ret = desmontarVolumen(nombreMontaje);
	return (ret);
}

/*
 * LLAMADA AL SISTEMA: sis_process_pid
 */
int sis_pid()
{
	//Devolvemos el identificador del proceso actual.
	return procesoActual->id;
}

/*
 * LLAMADA AL SISTEMA: sis_process_time
 */
int sis_time()
{
	//TICK (constante): Número de ticks por segundo.
	//ticks (variable): Número de ticks desde el inicio del minikernel.
	return ticks / TICK;
}

/*
 * LLAMADA AL SISTEMA: sis_sleep
 */
int sis_sleep()
{
	//Cogemos el primer argumento de la llamada al sistema.
	unsigned int sleep_time = (unsigned int) leer_registro(1);

	//Calculamos los ticks (límite) hasta los que debe dormir el proceso.
	procesoActual->ticksDespertar = ticks + sleep_time * TICK;

	//Insertamos el proceso a la lista de dormidos.
	insertarOrdenadoListaBCP(&listaDurmiendo, procesoActual);
	procesoActual->estado = BLOQUEADO;

	//Realizamos un cambio de contexto, guardando el proceso actual en una variable auxiliar.
	TipoBCP *procesoAux = procesoActual;
	procesoActual = planificador();
	while(procesoActual == NULL) {
		esperaInterrupcion();
		procesoActual = planificador();
	}

	//Ejecutamos el proceso actual.
	procesoActual->estado = EJECUCION;
	cambio_contexto(&(procesoAux->contextoRegs), &(procesoActual->contextoRegs));

	return 0;
}

