/*
 *  kernel/kernel.c
 *
 *  Minikernel. Version 1.0
 *
 *  Fernando Perez Costoya
 *
 */


#include "minikernel.h"
#include <string.h>
#include <getopt.h>

int pru()
{
	return (0);
}
/*
 *
 * Rutina de inicializacion invocada en arranque
 *
 */
int main(int argc, char **argv)
{
	int opt,ret;
	int opcionElegida = 0;
	int descDriver;
	int hardwareId;
	char nombreDisp[MAX_STRING];
	static struct option listaOpciones[] = {
					{"screen", 1, 0, 0},
					{"keyboard", 1, 0, 0},
					{"cdrom", 1, 0, 0},
					{"disk", 1, 0, 0},
					{0, 0, 0, 0}};

	/* se llega con las interrupciones prohibidas */

	/* iniciar Tabla de procesos */
	iniciarTablaProcesos();
		
	/* iniciar tablas dinamicas */
	inicializarTablaDrivers();
	inicializarTablaDispositivos();
	inicializarTablaFS();
	inicializarTablaMontajes();
	inicializarTablaInodos();

	/* iniciar lista de tareas pendientes de la int.sw */
	iniciarTareasPend(&(listaTareasPend));

	/* intalar manejadores para los vectores de interupcion */
	instal_man_int(EXC_ARITMETICA, excepcionAritmetica); 
	instal_man_int(EXC_MEMORIA, excepcionMemoria); 
	instal_man_int(INT_RELOJ, interrupcionReloj); 
	instal_man_int(INT_DISPOSITIVOS, interrupcionDispositivos); 
	instal_man_int(LLAM_SISTEMA, tratarLlamadaSistema); 
	instal_man_int(INT_SW, interrupcionSoftware); 


	/* cargar los modulos e iniciar los dispositivos */
	do {
		/* obtener las opciones de la linea de comando una a una */
		opt = getopt_long_only(argc, argv, "", listaOpciones, &opcionElegida);
		switch (opt) {
		case 0:
			if (0 == opcionElegida) { /* pantalla */
				hardwareId=ID_PANTALLA;
				strcpy(nombreDisp,"pantalla");
			} else if (1 == opcionElegida) {/* teclado */
				hardwareId=ID_TECLADO;
				strcpy(nombreDisp,"teclado");
			} else if (2 == opcionElegida) {/* cdrom */
				hardwareId=ID_CDROM;
				strcpy(nombreDisp,"cdrom");
			} else if (3 == opcionElegida) {/* disco */
				hardwareId=ID_DISCO;
				strcpy(nombreDisp,"disco");
			} else {
				printk("Error: No se puede crear el dispositivo %s\n", listaOpciones[opcionElegida].name);
				panico ("Error fatal creando dispositivo!");
			}
			/* buscar el driver indicado en la tabla de drivers */
			printk("buscando driver %s\n", optarg);
			descDriver = buscarEntradaTablaDrivers(optarg);
			if (-1 == descDriver) {
				
				/* ejecutar la funcion de carga del modulo correspondiente */
				printk("Cargando modulo %s\n", optarg);
				ret = cargar_modulo(optarg);
				if (-1 == ret) panico ("modulo no existe\n");
				
				/*repetir la busqueda despues de cargar el modulo */
				descDriver = buscarEntradaTablaDrivers(optarg);
				if (-1 == descDriver) panico ("driver no existe en tabla\n");
			}
			printk("Crear Dispositivo %s\n", optarg);
			ret = crearDispositivo(descDriver,nombreDisp,hardwareId); 
			if (-1 == ret) panico ("dispositivo no creado\n");
			break;
		case -1:
			/* Ya no hay m‡s opciones */
			break;
		default:
			/* error de parseo */
			panico ("Error: Fallo al parsear las opciones de la linea de comandos\n");
		}
	} while (-1 != opt);

	/* inicializar el hardware */
	iniciar_cont_int();		/* inicia cont. interr. */
	iniciar_cont_reloj(TICK);	/* fija frecuencia del reloj */

	/* crea proceso inicial */
	/* if (crear_tarea((void *)"init")<0) */
	/* cambio */
	if (crearTarea((void *)"init", PRIORIDAD_NORMAL)<0)
		panico("no encontrado el proceso inicial");
	/* activa proceso inicial */
	procesoActual=planificador();
	/* abrir entrada y salida estandard */
	if (0 != abrirFichero("/DISPOSITIVOS/teclado",0)) printk ("Error Al abrir el teclado");
	if (1 != abrirFichero("/DISPOSITIVOS/pantalla",0)) printk ("Error Al abrir la pantalla");

	cambio_contexto(NULL, &(procesoActual->contextoRegs));
	panico("S.O. reactivado inesperadamente");
	return 0;
}
