/*
 *  modulo_disco.c
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
 * Tipo de los attributos especificos del dispositivo de disco
 */
typedef struct {
	int puntero;
} TipoDatosPropiosDescFichero_disco;

/*
 * Tipo de los attributos especificos del dispositivo de disco
 */
typedef struct {
	int bloques;
	TipoPeticion *peticionActual;
	TipoListaPeticiones listaPeticiones;
} TipoDatosPropiosDispositivo_disco;

/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/
 
/* -----------------------------------------------------------*
 * Funciones especificas de gestión del DescFichero de disco
 * -----------------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha creado vacio y lo asocia a "nombre"
 */
int abrirFichero_disco(int descFichero, char *nombre, int flags) {
	//Asignar puntero de datos propios del desc. de fichero a una variable local.
	TipoDatosPropiosDescFichero_disco *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_disco *) procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	//Inicializar puntero de siguiente bloque a leer.
	datosPropiosDescFichero->puntero = 0;

	return 0;
}

/*
 * Funcion que lee un buffer de datos del descriptor
 * de fichero.
 */
int leerFichero_disco(int descFichero, char *buffer, int tamanyo) {
	//Comprobar que se está leyendo una longitud igual al tamaño del bloque.
	if(tamanyo != TAM_BLOQUE) {
		printk("El tamaño no coincide con el tamaño de bloque\n");
		return -1;
	}

	//Asignar puntero de datos propios del desc. de fichero a una variable local.
	TipoDatosPropiosDescFichero_disco *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_disco *) procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	//Asignar puntero de datos propios del dispositivo a una variable local.
	int descDispositivo = procesoActual->tablaDescFicheros[descFichero].descDispositivo;
	TipoDatosPropiosDispositivo_disco *datosPropiosDispositivo;
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_disco *) tablaDispositivos[descDispositivo].datosPropiosDispositivo;

	//Si el siguiente bloque a leer >= numeroBloques terminar y devolver 0.
	if(datosPropiosDescFichero->puntero >= datosPropiosDispositivo->bloques) {
		return 0;
	}
	else {
		//Solicitar el siguiente bloque a leer al dispositivo (peticionBloque).
		int ret = peticionBloque(descDispositivo, datosPropiosDescFichero->puntero, buffer, OP_READ);
		if(ret == -1) {
			printk("Error en la petición de bloque al disco\n");
			return -1;
		}

		//Incrementar el siguiente bloque a leer.
		datosPropiosDescFichero->puntero = datosPropiosDescFichero->puntero + 1;

		return tamanyo;
	}
}

/*
 * Funcion que escribe un buffer de datos del descriptor
 * de fichero.
 */
int escribirFichero_disco(int descFichero, char *buffer, int tamanyo) {
	//Comprobar que se está leyendo una longitud igual al tamaño del bloque.
	if(tamanyo != TAM_BLOQUE) {
		printk("El tamaño no coincide con el tamaño de bloque\n");
		return -1;
	}

	//Asignar puntero de datos propios del desc. de fichero a una variable local.
	TipoDatosPropiosDescFichero_disco *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_disco *) procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	//Asignar puntero de datos propios del dispositivo a una variable local.
	int descDispositivo = procesoActual->tablaDescFicheros[descFichero].descDispositivo;
	TipoDatosPropiosDispositivo_disco *datosPropiosDispositivo;
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_disco *) tablaDispositivos[descDispositivo].datosPropiosDispositivo;

	//Si el siguiente bloque a leer >= numeroBloques terminar y devolver 0.
	if(datosPropiosDescFichero->puntero >= datosPropiosDispositivo->bloques) {
		return 0;
	}
	else {
		//Solicitar el siguiente bloque a escribir al dispositivo (peticionBloque).
		int ret = peticionBloque(descDispositivo, datosPropiosDescFichero->puntero, buffer, OP_WRITE);
		if(ret == -1) {
			printk("Error en la petición de bloque al disco\n");
			return -1;
		}

		//Incrementar el siguiente bloque a escribir.
		datosPropiosDescFichero->puntero = datosPropiosDescFichero->puntero + 1;

		return tamanyo;
	}
}

/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero_disco(int descFichero) {
	//Borrar descripor de la tabla.
	liberarEntradaTablaDescFicheros(procesoActual->tablaDescFicheros, descFichero);

	return 0;
}

/* ------------------------------------------------------------------*
 * Funciones especificas de gestión del objeto dispositivo de disco
 * ------------------------------------------------------------------*/

/*
 * Funcion que crear un nuevo descriptor de fichero de disco vacio 
 * en la tabla de descriptores de ficheros
 */
int crearDescFicheroDispositivo_disco(int descDispositivo, TipoTablaDescFicheros tablaDescFicheros) {
	//Buscar entrada libre de Desc. de fichero en la tabla del proceso.
	int descFichero = reservarEntradaTablaDescFicheros(tablaDescFicheros);
	
	//Cargar tipo y desc. de dispositivo.
	tablaDescFicheros[descFichero].tipo = T_DISPOSITIVO;
	tablaDescFicheros[descFichero].descDispositivo = descDispositivo;

	//No hay datos propios.
	tablaDescFicheros[descFichero].datosPropiosDescFichero = (TipoDatosPropiosDescFichero_disco *) malloc(sizeof(TipoDatosPropiosDescFichero_disco));

	//Cargar punteros a las funciones del Desc. de fichero del disco.
	tablaDescFicheros[descFichero].abrirFichero = abrirFichero_disco;
	tablaDescFicheros[descFichero].leerFichero = leerFichero_disco;
	tablaDescFicheros[descFichero].escribirFichero = escribirFichero_disco;
	tablaDescFicheros[descFichero].buscarEnFichero = NULL;
	tablaDescFicheros[descFichero].cerrarFichero = cerrarFichero_disco;
	tablaDescFicheros[descFichero].mostrarFichero = NULL;

	return descFichero;
}

/*
 * Funcion que realiza una peticion de bloque al dispositivo de disco
 */
int peticionBloque_disco(int descDispositivo, int numBloque, char* bloque, int operacion) {
	//Asignar puntero de datos propios del dispositivo a una variable local.
	TipoDatosPropiosDispositivo_disco *datosPropiosDispositivo;
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_disco *) tablaDispositivos[descDispositivo].datosPropiosDispositivo;

	//Buscar si hay una petición por el mismo numBloque en la lista de peticiones.
	TipoPeticion *peticionAux = buscarListaPeticiones(&datosPropiosDispositivo->listaPeticiones, numBloque);
	if(peticionAux != NULL) {
		//Incrementar el número de peticiones de dicha petición.
		peticionAux->numPeticiones = peticionAux->numPeticiones + 1;

		//Insertar el proceso actual en la lista de proceso bloqueados de la petición.
		insertarUltimoListaBCP(&peticionAux->listaProcesosBloqueados, procesoActual);

		//printk("\n[PeticionBloque] Insertamos el proceso %d en la lista por el bloque %d para el dispositivo %d\n", procesoActual->id, numBloque, descDispositivo);

		if(operacion == OP_WRITE) {
			//Copiar bloque a la petición.
			memcpy(peticionAux->bloque, bloque, TAM_BLOQUE);

			//Modificar la operacion de la peticion a OP_WRITE.
			peticionAux->operacion = OP_WRITE;
		}
	}
	else {
		//printk("\n[PeticionBloque] Creamos una petición para el bloque %d del dispositivo %d\n", numBloque, descDispositivo);

		//Crear una nueva petición y rellenar sus atributos.
		peticionAux = (void *) malloc(sizeof(TipoPeticion));
		peticionAux->descDispositivo = descDispositivo;
		peticionAux->numBloque = numBloque;
		peticionAux->operacion = operacion;
		peticionAux->numPeticiones = 1;
		peticionAux->siguiente = NULL;
		inicializarListaBCP(&peticionAux->listaProcesosBloqueados);

		//Insertar el proceso actual en la lista de proceso bloqueados de la petición.
		insertarUltimoListaBCP(&peticionAux->listaProcesosBloqueados, procesoActual);

		if(operacion == OP_WRITE) {
			//Copiar bloque a la petición.
			memcpy(peticionAux->bloque, bloque, TAM_BLOQUE);
		}

		if(datosPropiosDispositivo->peticionActual == NULL) {
			//Asignar la nueva petición como la petición actual.
			datosPropiosDispositivo->peticionActual = peticionAux;

			//Solicitar una operación DMA para la petición.
			int hardwareID = tablaDispositivos[descDispositivo].hardwareID;
			int numSectoresPorBloque = TAM_BLOQUE / DEVICE_SECTOR_SIZE;
			int sectorInicial = peticionAux->numBloque * numSectoresPorBloque;
			if(operacion == OP_READ) {
				memset(peticionAux->bloque, 0, TAM_BLOQUE);
				int ret = cfg_lectura_DMA(hardwareID, sectorInicial, peticionAux->bloque, numSectoresPorBloque);
				if(ret == -1) {
					printk("Error en la lectura DMA\n");
					return -1;
				}
			}
			else if(operacion == OP_WRITE) {
				int ret = cfg_escritura_DMA(hardwareID, sectorInicial, peticionAux->bloque, numSectoresPorBloque);
				if(ret == -1) {
					printk("Error en la escritura DMA\n");
					return -1;
				}
			}
		}
		else {
			//printk("\n[PeticionBloque] Insertamos la petición para el bloque %d del dispositivo %d en la lista de peticiones\n", numBloque, descDispositivo);
			//Insertar petición en la lista de peticiones.
			insertarListaPeticiones(&datosPropiosDispositivo->listaPeticiones, peticionAux);
		}
	}

	//Cambiar estado del proceso actual de ejecutando a bloqueado.
	procesoActual->estado = BLOQUEADO;

	//Solicitar al planificador el siguiente proceso listo para ejecutar.
	TipoBCP *procesoAux = procesoActual;
	procesoActual = planificador();
	while(procesoActual == NULL) {
		esperaInterrupcion();
		procesoActual = planificador();
	}

	//Ejecutamos el proceso actual.
	procesoActual->estado = EJECUCION;
	cambio_contexto(&(procesoAux->contextoRegs), &(procesoActual->contextoRegs));

	if(operacion == OP_READ) {
		//printk("\nProceso %d leyendo bloque %d\n\n\n", procesoActual->id, numBloque);
		memcpy(bloque, peticionAux->bloque, TAM_BLOQUE);
	}

	//Decrementar el número de peticiones de la petición.
	peticionAux->numPeticiones = peticionAux->numPeticiones - 1;

	if(peticionAux->numPeticiones == 0) {
		//Liberar la memoria de la petición.
		free(peticionAux);
	}

	return 0;
}

/*
 * Funcion que se ejecuta cuando llega una interrupcion
 * hardware del Dispositivo de disco
 */
int interrupcionHW_disco(int descDispositivo) {
	//Obtener identificador hardware del disco.
	int hardwareID = tablaDispositivos[descDispositivo].hardwareID;

	//Comprobar si la interrupción de disco está activada.
	if(test_and_set_interrupcion(hardwareID)) {
		//Generar interrupción software.
		insertarUltimaTareaPend(&listaTareasPend, interrupcionSW, (void *) descDispositivo);
		activar_int_SW();

		return 0;
	}
	else {
		return -1;
	}
}

/*
 * Funcion que se ejecuta cundo llega una interrupcion
 * software desde la interrupcion HW del Dispositivo de teclado
 * para despertar procesos bloqueados esperando una tecla
 */
void interrupcionSW_disco(int descDispositivo) {
	//Obtener datos propios del dispositivo.
	TipoDatosPropiosDispositivo_disco *datosPropiosDispositivo = (TipoDatosPropiosDispositivo_disco *) tablaDispositivos[descDispositivo].datosPropiosDispositivo;

	//Guardar petición actual en una variable local.
	TipoPeticion *peticionActual = datosPropiosDispositivo->peticionActual;

	//Asignar a petición actual la primera petición de la lista de peticiones.
	datosPropiosDispositivo->peticionActual = extraerListaPeticiones(&datosPropiosDispositivo->listaPeticiones);

	if(datosPropiosDispositivo->peticionActual != NULL) {
		//Solicitar una operación DMA para la petición
		int hardwareID = tablaDispositivos[descDispositivo].hardwareID;
		int numSectoresPorBloque = TAM_BLOQUE / DEVICE_SECTOR_SIZE;
		int sectorInicial = datosPropiosDispositivo->peticionActual->numBloque * numSectoresPorBloque;
		if(datosPropiosDispositivo->peticionActual->operacion == OP_READ) {
			int ret = cfg_lectura_DMA(hardwareID, sectorInicial, datosPropiosDispositivo->peticionActual->bloque, numSectoresPorBloque);
			if(ret == -1) {
				printk("Error en la lectura DMA\n");
				return;
			}
		}
		else if(datosPropiosDispositivo->peticionActual->operacion == OP_WRITE) {
			int ret = cfg_escritura_DMA(hardwareID, sectorInicial, datosPropiosDispositivo->peticionActual->bloque, numSectoresPorBloque);
			if(ret == -1) {
				printk("Error en la escritura DMA\n");
				return -1;
			}
		}
	}

	//Poner a listo el estado de todos los procesos en la lista de bloqueados de la petición.
	TipoBCP *proceso = primeroListaBCP(&peticionActual->listaProcesosBloqueados);
	while(proceso != NULL) {
		eliminarPrimeroListaBCP(&peticionActual->listaProcesosBloqueados);
		proceso->estado = LISTO;

		//Insertar todos estos procesos con insertar_En_Listas_Prioridades(proceso).
		insertarPrioridadesListaBCP(proceso);

		//Coger siguiente proceso.
		proceso = primeroListaBCP(&peticionActual->listaProcesosBloqueados);
	}
}

/* -------------------------------------------------------------*
 * Funciones especificas de gestión del objeto driver de disco
 * -------------------------------------------------------------*/

/*
 * Funcion que crear un nuevo Dispositivo vacio 
 * en la tabla de Dispositivos
 */
int crearDispositivo_disco(int descDriver, char *nombreDispositivo, int hardwareID) {
	//Enchufar el Hardware.
	int ret = plug_hardware(hardwareID, "disco.img");
	if (ret == -1) {
		printk("Error al activar el disco\n");
		return -1;
	}

	//Buscar entrada libre de Dispositivo en la tabla.
	int descDispositivo = reservarEntradaTablaDispositivos(nombreDispositivo);
	
	//Cargar descriptor de Driver e identificador de hardware.
	tablaDispositivos[descDispositivo].descDriver = descDriver;
	tablaDispositivos[descDispositivo].hardwareID = hardwareID;

	//Reservar memoria dinámica y asignarla para los datos propios del dispositivo.
	TipoDatosPropiosDispositivo_disco *datosPropiosDispositivo;
	datosPropiosDispositivo = (TipoDatosPropiosDispositivo_disco *) malloc(sizeof(TipoDatosPropiosDispositivo_disco));	
	tablaDispositivos[descDispositivo].datosPropiosDispositivo = (void *) datosPropiosDispositivo;

	//Inicializar la lista de peticiones y la petición actual de los datos propios.
	datosPropiosDispositivo->bloques = (ret * DEVICE_SECTOR_SIZE) / TAM_BLOQUE;
	iniciarListaPeticiones(&datosPropiosDispositivo->listaPeticiones);
	datosPropiosDispositivo->peticionActual = NULL;

	//Cargar punteros a las funciones del Dispositivo disco.
	tablaDispositivos[descDispositivo].crearDescFicheroDispositivo = crearDescFicheroDispositivo_disco;
	tablaDispositivos[descDispositivo].peticionCaracter = NULL;
	tablaDispositivos[descDispositivo].peticionBloque = peticionBloque_disco;
	tablaDispositivos[descDispositivo].interrupcionHW = interrupcionHW_disco;
	tablaDispositivos[descDispositivo].interrupcionSW = interrupcionSW_disco;
	tablaDispositivos[descDispositivo].destruirDispositivo = NULL;
	tablaDispositivos[descDispositivo].mostrarDispositivo = NULL;

	return descDispositivo;
}

/* -----------------------------------------------------*
 * Funciones especificas de cargar y descarga de modulos
 * -----------------------------------------------------*/

/*
 * Funcion que inicializa el modulo y carga el objeto Driver de disco
 * en la tabla de Drivers
 */
int cargarModulo_disco() {
	//Buscar entrada libre de Driver en la tabla.
	int descDriver = reservarEntradaTablaDrivers("disco");

	//No hay datos propios.
	tablaDrivers[descDriver].datosPropiosDriver = NULL;

	//Cargar punteros a las funciones del driver de disco.
	tablaDrivers[descDriver].crearDispositivo = crearDispositivo_disco;
	tablaDrivers[descDriver].destruirDriver = NULL;
	tablaDrivers[descDriver].mostrarDriver = NULL;

	return 0;
}
