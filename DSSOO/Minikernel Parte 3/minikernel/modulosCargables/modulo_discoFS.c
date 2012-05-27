/*
 *  modulo_discoFS.c
 */

#include <string.h>
#include <stdlib.h>
#include "minikernel.h"

/***************************************************************
 * CONSTANTES
 ***************************************************************/

//Superbloque
#define FS_BLOCK_SIZE 1024 //Tamanyo del bloque del SSFF
#define FS_NUM_MAGICO 0x000D5500 //Numero magico
#define PADDING_SB 992 //Relleno del SB para que ocupe un bloque

//Mapas
#define MAX_NUM_INODOS 16*1024 //Suficiente para que quepan inodos de cualquier montaje
#define MAX_NUM_BLOQUES 256*1024 //Suficiente para que quepan bloques de cualquier montaje

//Inodo
#define LONG_NOMBRE 200 //Longitud max. nombre de un archivo/directorio
#define CAPACIDAD_DIRECTORIO 200 //Capacidad de un directorio
#define PADDING_INODO 8 //Relleno del inodo para que ocupe un bloque
#define T_FICHERO 1 //Inodo de tipo fichero
#define T_DIRECTORIO 0 //Inodo de tipo directorio

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/

typedef struct {
	//Número mágico para reconocer el superbloque.
	unsigned int numMagico;
	//Número de bloques del mapa inodos.
	unsigned int numBloquesMapaInodos;
	//Número de bloques del mapa datos.
	unsigned int numBloquesMapaDatos;
	//Número de inodos en el dispositivo.
	unsigned int numInodos;
	//Número bloque del 1º inodo del disp. (inodo raiz).
	unsigned int primerInodo;
	//Número de bloques de datos en el disp.
	unsigned int numBloquesDatos;
	//Número de bloque del 1º bloque de datos.
	unsigned int primerBloqueDatos;
	//Tamaño total del disp. (en bytes).
	unsigned int tamDispositivo;
	//Campo de relleno (para completar un bloque).
	char relleno[PADDING_SB];
} TipoSuperbloque;

typedef struct {
	//Fichero(T_FICHERO) o directorio (T_DIRECTORIO).
	unsigned int tipo;
	//Nombre del fichero o directorio de este inodo.
	char nombre[LONG_NOMBRE];
	//Vector contiene los números de los inodos de los ficheros del directorio.
	unsigned int inodosContenidos[CAPACIDAD_DIRECTORIO];
	//Tamaño actual del fichero en bytes.
	unsigned int tamanyo;
	//Número del bloque directo.
	unsigned int bloqueDirecto;
	//Número del bloque indirecto.
	unsigned int bloqueIndirecto;
	//Campo relleno para llenar un bloque.
	char relleno[PADDING_INODO];
} TipoInodoDisco;

typedef struct {
	//Número del siguiente byte a leer del fichero.
	unsigned int byte_siguiente;
} TipoDatosPropiosDescFichero_discoFS;

typedef struct {
	//Estructura del inodo.
	TipoInodoDisco inodo;
} TipoDatosPropiosInodo_discoFS;

typedef struct {
	//Estructura del superbloque.
	TipoSuperbloque superBloque;
	//Array del mapa de inodos.
	char tablaInodos[MAX_NUM_INODOS];
	//Array del mapa de bloque de datos.
	char tablaBloques[MAX_NUM_BLOQUES];
} TipoDatosPropiosMontaje_discoFS;

/***************************************************************
 * FUNCIONES PUBLICAS
 ***************************************************************/
 
/* -------------------------------------------------------------------*
 * Funciones especificas de gestión del objeto DescFichero de discoFS
 * -------------------------------------------------------------------*/

/*
 * Funcion que abre un nuevo Descriptor de fichero que 
 * previamente se ha creado vacio y lo asocia a "nombre"
 */
int abrirFichero_discoFS(int descFichero, char *nombre, int flags) {
	//Asignar puntero de datos propios del desc. de fichero a una variable local.
	TipoDatosPropiosDescFichero_discoFS *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_discoFS *) procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	//Buscar el nombre del inodo y sacar el desc. del inodo
	int mount_desc = procesoActual->tablaDescFicheros[descFichero].descMontaje;
	int inode_id = buscarNombreInodo(mount_desc, nombre);

	//Si no existe el inodo: Terminar con error.
	if(inode_id == -1) {
		//Reservar número de inodo en el mapa de inodos (reservarInodo).
		int inode_num = reservarInodo(mount_desc);

		//Crear un nuevo inodo vacío con el número reservado (crearInodo).
		inode_id = crearInodo(mount_desc, inode_num);

		//Inicializar el nuevo inodo (iniciarNuevoInodo).
		iniciarNuevoInodo(inode_id, nombre, flags);
	}

	//Asignar inodo al desc. de fichero.
	procesoActual->tablaDescFicheros[descFichero].descInodo = inode_id;

	//Inicializar puntero de siguiente byte a leer.
	datosPropiosDescFichero->byte_siguiente = 0;

	return 0;
}

/*
 * Funcion que lee un buffer de datos del descriptor
 * de fichero.
 */
int leerFichero_discoFS(int descFichero, char *buffer, int tamanyo) {
	//Asignar puntero de datos propios del desc. de fichero a una variable local.
	TipoDatosPropiosDescFichero_discoFS *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_discoFS *) procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	//Comprobamos que el inodo es correcto.
	int inode_id = procesoActual->tablaDescFicheros[descFichero].descInodo;
	if(inode_id < 0) {
		return -1;
	}

	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[inode_id].datosPropiosInodo;

	//Si el tamaño del fichero > TAM_BLOQUE: Terminar con error.
	if(datosPropiosInodo->inodo.tamanyo > FS_BLOCK_SIZE) {
		return -1;
	}

	//Si el siguiente byte a leer > = tamaño del fichero: Terminar y devolver 0
	if(datosPropiosDescFichero->byte_siguiente >= datosPropiosInodo->inodo.tamanyo){
		return 0;
	}

	//Si el siguiente byte a leer + tamaño >= tamaño del fichero:
	if(datosPropiosDescFichero->byte_siguiente + tamanyo >= datosPropiosInodo->inodo.tamanyo) {
		tamanyo = datosPropiosInodo->inodo.tamanyo - datosPropiosDescFichero->byte_siguiente;
	}

	//Obtener el bloque físico que corresponde con el bloque 0 del archivo (mapearBloque).
	int block_num = mapearBloque(inode_id, 0);

	//Solicitar el bloque físico a leer al dispositivo (peticionBloque).
	int device_id = procesoActual->tablaDescFicheros[descFichero].descDispositivo;
	char block[FS_BLOCK_SIZE];
	peticionBloque(device_id, block_num, block, OP_READ);

	//Copia en buffer el trozo de bloque desde el siguiente byte a leer y que ocupa tamaño.
	memcpy(buffer, &block[datosPropiosDescFichero->byte_siguiente], tamanyo);

	//Sumar al siguiente byte a leer, el tamaño.
	datosPropiosDescFichero->byte_siguiente = datosPropiosDescFichero->byte_siguiente + tamanyo;

	return tamanyo;
}

/*
 * Funcion que escribe un buffer de datos del descriptor
 * de fichero.
 */
int escribirFichero_discoFS(int descFichero, char *buffer, int tamanyo) {
	//Asignar puntero de datos propios del desc. de fichero a una variable local.
	TipoDatosPropiosDescFichero_discoFS *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_discoFS *) procesoActual->tablaDescFicheros[descFichero].datosPropiosDescFichero;

	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	int inode_id = procesoActual->tablaDescFicheros[descFichero].descInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[inode_id].datosPropiosInodo;

	//Si el tamaño del fichero > TAM_BLOQUE: Terminar con error.
	if(datosPropiosInodo->inodo.tamanyo > FS_BLOCK_SIZE) {
		return -1;
	}

	//Si el siguiente byte a acceder > = TAM_BLOQUE: Terminar y devolver 0
	if(datosPropiosDescFichero->byte_siguiente >= FS_BLOCK_SIZE) {
		return 0;
	}

	//Si el siguiente byte a acceder + tamaño >= TAM_BLOQUE: tamaño = TAM_BLOQUE - siguiente byte a acceder.
	if(datosPropiosDescFichero->byte_siguiente + tamanyo >= FS_BLOCK_SIZE) {
		tamanyo = FS_BLOCK_SIZE - datosPropiosDescFichero->byte_siguiente;
	}

	//Obtener el bloque físico que corresponde con el bloque 0 del archivo (mapearBloque).
	int block_num = mapearBloque(inode_id, 0);

	//Si el bloque físico no existe: Asignar un nuevo bloque físico al bloque 0 del archivo (asignarNuevoBloque).
	if(block_num == -1) {
		block_num = asignarNuevoBloque(inode_id, 0);
	}

	//Solicitar lectura del bloque físico seleccionado al dispositivo (peticionBloque).
	char block[FS_BLOCK_SIZE];
	int device_id = procesoActual->tablaDescFicheros[descFichero].descDispositivo;
	peticionBloque(device_id, block_num, block, OP_READ);

	//Escribir en el bloque el trozo de buffer desde el siguiente byte y que ocupa tamaño.
	memcpy(&block[datosPropiosDescFichero->byte_siguiente], buffer, tamanyo);

	//Solicitar escritura del bloque físico seleccionado al dispositivo (peticionBloque).
	peticionBloque(device_id, block_num, block, OP_WRITE);

	//Sumar al siguiente byte a acceder el valor de tamaño.
	datosPropiosDescFichero->byte_siguiente = datosPropiosDescFichero->byte_siguiente + tamanyo;

	//Si el siguiente byte a acceder > tamaño del fichero: tamaño del fichero = siguiente byte a acceder.
	if(datosPropiosDescFichero->byte_siguiente > datosPropiosInodo->inodo.tamanyo) {
		datosPropiosInodo->inodo.tamanyo = datosPropiosDescFichero->byte_siguiente;
	}

	//Terminar y devolver tamaño.
	return tamanyo;
}

/*
 * Funcion que cierra un descriptor de fichero y lo 
 * elimina de la tabla de descriptores de ficheros.
 */
int cerrarFichero_discoFS(int descFichero) {
	//Borrar descripor de la tabla.
	liberarEntradaTablaDescFicheros(procesoActual->tablaDescFicheros, descFichero);

	return 0;
}

/* -------------------------------------------------------------*
 * Funciones especificas de gestión del objeto Inodo de discoFS
 * -------------------------------------------------------------*/
 
 /*
 * Funcion que lee un inodo de discoFS a partir
 * de la tabla de inodos
 */
int leerInodo_discoFS(int descInodo) {
	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[descInodo].datosPropiosInodo;

	//Solicitar leer en la estructura del inodo de datos propios el bloque físico del inodo al dispositivo (peticionBloque).
	int device_id = tablaMontajes[tablaInodos[descInodo].descMontaje].descDispositivo;
	peticionBloque(device_id, tablaInodos[descInodo].numBloque, (void *) &datosPropiosInodo->inodo, OP_READ);
 
	return 0;
 }
 
 /*
 * Funcion que devuelve el bloque directo del inodo
 * de discoFS
 */
int mapearBloque_discoFS(int descInodo, int numBloqueArchivo) {
	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[descInodo].datosPropiosInodo;

	//Si numBloqueArchivo == 0 y el tamaño del fichero > 0: Terminar y devolver el bloqueDirecto del inodo.
	if(numBloqueArchivo == 0 && datosPropiosInodo->inodo.tamanyo > 0) {
		return datosPropiosInodo->inodo.bloqueDirecto;
	}
	//Sino: Terminar con error.
	else {
		return -1;
	}
}

/*
 * Funcion que inicializa un nuevo inodo de discoFS
 * en la tabla de inodos
 */
int iniciarNuevoInodo_discoFS(int descInodo, char *nombre, int flags) {
	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[descInodo].datosPropiosInodo;

	//Asignar al tipo del inodo el valor de flags.
	datosPropiosInodo->inodo.tipo = flags;

	//Asignar al nombre del inodo el valor de nombre.
	strcpy(datosPropiosInodo->inodo.nombre, nombre);

	//Inicializar el resto de atributos del inodo.
	datosPropiosInodo->inodo.inodosContenidos[0] = 0;
	datosPropiosInodo->inodo.tamanyo = 0;
	datosPropiosInodo->inodo.bloqueDirecto = 0;
	datosPropiosInodo->inodo.bloqueIndirecto = 0;

	return 0;
}

/*
 * Funcion que asigna nuevo bloque en la tabla de
 * Montajes de discoFS
 */
int asignarNuevoBloque_discoFS(int descInodo, int numBloqueArchivo) {
	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[descInodo].datosPropiosInodo;

	//Si numBloqueArchivo == 0:
	if(numBloqueArchivo == 0) {
		//Reservar un bloque y asignarlo al bloqueDirecto del inodo (reservarBloque).
		int block_num = reservarBloque(tablaInodos[descInodo].descMontaje);
		datosPropiosInodo->inodo.bloqueDirecto = block_num;

		return block_num;
	}
	//Sino: Terminar con error.
	else {
		return -1;
	}
}

/*
 * Funcion que escribe un inodo de discoFS a partir
 * de la tabla de inodos
 */
int escribirInodo_discoFS(int descInodo) {
	//Asignar puntero de datos propios del inodo a una variable local.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) tablaInodos[descInodo].datosPropiosInodo;

	//Solicitar escribir la estructura de inodo al bloque físico del inodo en dispositivo (peticionBloque).
	int device_id = tablaMontajes[tablaInodos[descInodo].descMontaje].descDispositivo;
	peticionBloque(device_id, tablaInodos[descInodo].numBloque, (void *) &(datosPropiosInodo->inodo), OP_WRITE);
	
	return 0;
}

/* ----------------------------------------------------------------*
 * Funciones especificas de gestión del objeto Montaje del discoFS
 * ----------------------------------------------------------------*/

/*
 * Funcion que monta un volumen formateado con el discoFS
 */
int montarVolumen_discoFS(int descMontaje, char *nombreDispositivo) {
	//Buscar nombre dispositivo en la tabla de dispositivos.
	int device_id = buscarEntradaTablaDispositivos(nombreDispositivo);

	//Asignar el desc. del dispositivo en la entrada de montaje.
	tablaMontajes[descMontaje].descDispositivo = device_id;

	//Asignar puntero de datos propios del montaje a una variable local.
	TipoDatosPropiosMontaje_discoFS *datosPropiosMontaje;
	datosPropiosMontaje = (TipoDatosPropiosMontaje_discoFS *) tablaMontajes[descMontaje].datosPropiosMontaje;

	//Solicitar leer en la estructura del superbloque el bloque físico del superbloque al dispositivo (peticionBloque).
	peticionBloque(device_id, 1, (void *) &datosPropiosMontaje->superBloque, OP_READ);

	//Si el núm. mágico del superbloque != FS_NUM_MAGICO: Terminar con error.
	if(datosPropiosMontaje->superBloque.numMagico != FS_NUM_MAGICO) {
		return -1;
	}

	//Recorrer el número de bloques del mapa de inodos:
	int i;
	for(i = 0; i < datosPropiosMontaje->superBloque.numBloquesMapaInodos; i++) {
		//Solicitar leer en el trozo correspondiente del mapa de inodo cada bloque físico del mapa de inodos al dispositivo (peticionBloque).
		peticionBloque(device_id, 2 + i, &datosPropiosMontaje->tablaInodos[i * FS_BLOCK_SIZE], OP_READ);
	}

	//Recorrer el número de bloques del mapa de datos:
	for(i = 0; i < datosPropiosMontaje->superBloque.numBloquesMapaDatos; i++) {
		//Solicitar leer en el trozo correspondiente del mapa de datos cada bloque físico del mapa de inodos al dispositivo (peticionBloque).
		peticionBloque(device_id, 2 + datosPropiosMontaje->superBloque.numBloquesMapaInodos + i, &datosPropiosMontaje->tablaBloques[i * FS_BLOCK_SIZE], OP_READ);
	}

	//Recorrer el mapa de inodos:
	for(i = 0; i < datosPropiosMontaje->superBloque.numInodos; i++) {
		//Si el inodo está ocupado:
		if(datosPropiosMontaje->tablaInodos[i] == 1) {
			//Crear inodo vacio (crearInodo).
			int inode_id = crearInodo(descMontaje, i);

			//Leer inodo del dispositivo (leerInodo).
			leerInodo(inode_id);
		}
	}
	
	return device_id;
}

/*
 * Funcion que busca un inodo de discoFS con el nombre correspondiente
 * del volumen en la tabla de Inodos
 */
int buscarNombreInodo_discoFS(int descMontaje, char *nombre) {
	//Recorrer la tabla de inodos, buscando un inodo con este montaje que no esté ocupado (campo ocupado tablaInodos.h):
	int inode_id;
	for(inode_id = 0; inode_id < MAX_TABLA_INODOS; inode_id++) {
		if(tablaInodos[inode_id].ocupado == TRUE && tablaInodos[inode_id].descMontaje == descMontaje) {
			//Asignar puntero de datos propios del inodo a una variable local.
			TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
			datosPropiosInodo = tablaInodos[inode_id].datosPropiosInodo;

			//Si el nombre del inodo es igual al nombre (strcmp):
			if(strcmp(datosPropiosInodo->inodo.nombre,nombre) == 0) {
				//Terminar y devolver el descriptor del inodo.
				return inode_id;
			}
		}
	}

	//Si no se ha encontrado el inodo: Terminar con error.
	return -1;
}

/*
 * Funcion que crear un nuevo Inodo vacio de discoFS
 * en la tabla de Inodos
 */
int crearInodo_discoFS(int descMontaje, int numInodo) {
	//Asignar puntero de datos propios del montaje a una variable local.
	TipoDatosPropiosMontaje_discoFS *datosPropiosMontaje;
	datosPropiosMontaje = (TipoDatosPropiosMontaje_discoFS *) tablaMontajes[descMontaje].datosPropiosMontaje;

	//Comprobar el número del inodo.
	if(numInodo < 0 || numInodo >= datosPropiosMontaje->superBloque.numInodos) {
		return -1;
	}

	//Reservar entrada en la tabla de inodos.
	int inode_id = reservarEntradaTablaInodos(descMontaje, numInodo);

	//Asignar bloque de inodo como el número de inodo + el bloque del primer inodo.
	int block_num = datosPropiosMontaje->superBloque.primerInodo + numInodo;
	tablaInodos[inode_id].numBloque = block_num;

	//Reservar memoria dinámica y asignarla para los datos propios del inodo.
	TipoDatosPropiosInodo_discoFS *datosPropiosInodo;
	datosPropiosInodo = (TipoDatosPropiosInodo_discoFS *) malloc(sizeof(TipoDatosPropiosInodo_discoFS));
	tablaInodos[inode_id].datosPropiosInodo = (void *) datosPropiosInodo;

	//Asignar a dicha entrada los punteros de las funciones del inodo.
	tablaInodos[inode_id].iniciarNuevoInodo = iniciarNuevoInodo_discoFS;
	tablaInodos[inode_id].leerInodo = leerInodo_discoFS;
	tablaInodos[inode_id].mapearBloque = mapearBloque_discoFS;
	tablaInodos[inode_id].asignarNuevoBloque = asignarNuevoBloque_discoFS;
	tablaInodos[inode_id].buscarNombreEnDir = NULL;
	tablaInodos[inode_id].escribirInodo = escribirInodo_discoFS;
	tablaInodos[inode_id].borrarInodo = NULL;
	tablaInodos[inode_id].mostrarInodo = NULL;

	return inode_id;
}

/*
 * Funcion que crea un nuevo descriptor de fichero vacio de discoFS
 * en la tabla de descriptores de ficheros del proceso
 */
int crearDescFichero_discoFS(int descMontaje, TipoTablaDescFicheros tablaDescFicheros) {
	//Reservar entrada en la tabla descriptores de fichero.
	int file_id = reservarEntradaTablaDescFicheros(tablaDescFicheros);

	//Asignar tipo del descriptor, el desc. de montaje y el de dispositivo.
	tablaDescFicheros[file_id].tipo = T_FICHERO;
	tablaDescFicheros[file_id].descMontaje = descMontaje;
	tablaDescFicheros[file_id].descDispositivo = tablaMontajes[descMontaje].descDispositivo;

	//Reservar memoria dinámica y asignarla para los datos propios del desc. de fichero.
	TipoDatosPropiosDescFichero_discoFS *datosPropiosDescFichero;
	datosPropiosDescFichero = (TipoDatosPropiosDescFichero_discoFS *) malloc(sizeof(TipoDatosPropiosDescFichero_discoFS));
	tablaDescFicheros[file_id].datosPropiosDescFichero = (void *) datosPropiosDescFichero;

	//Asignar a dicha entrada los punteros de las funciones del desc. de fichero.
	tablaDescFicheros[file_id].abrirFichero = abrirFichero_discoFS;
	tablaDescFicheros[file_id].leerFichero = leerFichero_discoFS;
	tablaDescFicheros[file_id].escribirFichero = escribirFichero_discoFS;
	tablaDescFicheros[file_id].buscarEnFichero = NULL;
	tablaDescFicheros[file_id].cerrarFichero = cerrarFichero_discoFS;
	tablaDescFicheros[file_id].mostrarFichero = NULL;

	return file_id;
}

/*
 * Funcion que reserva un bloque del mapa de datos
 * en la tabla de Montajes de discoFS
 */
int reservarBloque_discoFS(int descMontaje) {
	//Asignar puntero de datos propios del montaje a una variable local.
	TipoDatosPropiosMontaje_discoFS *datosPropiosMontaje;
	datosPropiosMontaje = (TipoDatosPropiosMontaje_discoFS *) tablaMontajes[descMontaje].datosPropiosMontaje;

	//Recorrer el mapa de datos hasta encontrar una posición vacía.
	int block_num = 0;
	while(datosPropiosMontaje->tablaBloques[block_num] == 1 && block_num < MAX_NUM_BLOQUES) {
		block_num++;
	}

	//Comprobamos que hay una posición vacía.
	if(block_num >= MAX_NUM_BLOQUES) {
		return -1;
	}

	//Reservar dicha posición del mapa de datos.
	datosPropiosMontaje->tablaBloques[block_num] = 1;

	//Devolver el índice del mapa de datos + el número del primer bloque de datos.
	return block_num + datosPropiosMontaje->superBloque.primerBloqueDatos;
}

/*
 * Funcion que reserva un inodo del mapa de inodos
 * en la tabla de Montajes de discoFS
 */
int reservarInodo_discoFS(int descMontaje) {
	//Asignar puntero de datos propios del montaje a una variable local.
	TipoDatosPropiosMontaje_discoFS *datosPropiosMontaje;
	datosPropiosMontaje = (TipoDatosPropiosMontaje_discoFS *) tablaMontajes[descMontaje].datosPropiosMontaje;

	//Recorrer el mapa de inodos hasta inodos una posición vacía.
	int inode_num = 0;
	while(datosPropiosMontaje->tablaInodos[inode_num] == 1 && inode_num < MAX_NUM_INODOS) {
		inode_num++;
	}

	//Comprobamos que hay una posición vacía.
	if(inode_num >= MAX_NUM_INODOS) {
		return -1;
	}

	//Reservar dicha posición del mapa de datos.
	datosPropiosMontaje->tablaInodos[inode_num] = 1;

	//Devolver el índice del mapa de inodos.
	return inode_num;
}

/*
 * Funcion que sincroniza el volumen de discoFS
 */
int sincronizarVolumen_discoFS(int descMontaje) {
	//Asignar puntero de datos propios del montaje a una variable local.
	TipoDatosPropiosMontaje_discoFS *datosPropiosMontaje;
	datosPropiosMontaje = (TipoDatosPropiosMontaje_discoFS *) tablaMontajes[descMontaje].datosPropiosMontaje;

	//Solicitar escribir la estructura del superbloque al bloque físico del superbloque en dispositivo (peticionBloque).
	int device_id = tablaMontajes[descMontaje].descDispositivo;
	peticionBloque(device_id, 1, (void *) &datosPropiosMontaje->superBloque, OP_WRITE);

	//Recorrer el número de bloques del mapa de inodos: Solicitar escribir cada bloque del mapa de inodos en los bloques físicos del mapa de inodos al dispositivo (peticionBloque).
	int i;
	for(i = 0; i < datosPropiosMontaje->superBloque.numBloquesMapaInodos; i++) {
		peticionBloque(device_id, i + 2, &datosPropiosMontaje->tablaInodos[i * FS_BLOCK_SIZE], OP_WRITE);
	}

	//Recorrer el número de bloques del mapa de datos: Solicitar escribir cada bloque del mapa de datos en los bloques físicos del mapa de datos al dispositivo (peticionBloque).
	for(i = 0; i < datosPropiosMontaje->superBloque.numBloquesMapaDatos; i++) {
		peticionBloque(device_id, i + 2 + datosPropiosMontaje->superBloque.numBloquesMapaInodos, &datosPropiosMontaje->tablaBloques[i * FS_BLOCK_SIZE], OP_WRITE);
	}

	//Recorrer el mapa de inodos: 
	for(i = 0; i < datosPropiosMontaje->superBloque.numInodos; i++) {
		//Buscar el inodo con este montaje en la tabla de inodos.
		int inode_id = buscarEntradaTablaInodos(descMontaje,i);

		//Escribir inodo en el dispositivo (escribirInodo).
		escribirInodo(inode_id);
	}

	return device_id;
}

/* -------------------------------------------------------*
 * Funciones especificas de gestión del objeto FS de disco
 * -------------------------------------------------------*/

/*
 * Funcion que crear un nuevo Montaje vacio de discoFS
 * en la tabla de Montajes
 */
int crearMontaje_discoFS(int descFS, char *nombreMontaje) {
	//Reservar entrada en la tabla de montaje.
	int mount_id = reservarEntradaTablaMontajes(nombreMontaje);

	//Asignar a dicha entrada el descriptor de FS.
	tablaMontajes[mount_id].descFS = descFS;
	
	//Reservar memoria dinámica y asignarla para los datos propios del montaje.
	TipoDatosPropiosMontaje_discoFS *datosPropiosMontaje;
	datosPropiosMontaje = (TipoDatosPropiosMontaje_discoFS *) malloc(sizeof(TipoDatosPropiosMontaje_discoFS));
	tablaMontajes[mount_id].datosPropiosMontaje = (void *) datosPropiosMontaje;

	//Asignar a dicha entrada los punteros de las funciones del montaje.
	tablaMontajes[mount_id].montarVolumen = montarVolumen_discoFS;
	tablaMontajes[mount_id].reservarBloque = reservarBloque_discoFS;
	tablaMontajes[mount_id].liberarBloque = NULL;
	tablaMontajes[mount_id].reservarInodo = reservarInodo_discoFS;
	tablaMontajes[mount_id].liberarInodo = NULL;
	tablaMontajes[mount_id].buscarNombreInodo = buscarNombreInodo_discoFS;
	tablaMontajes[mount_id].crearInodo = crearInodo_discoFS;
	tablaMontajes[mount_id].crearDescFichero = crearDescFichero_discoFS;
	tablaMontajes[mount_id].sincronizarVolumen = sincronizarVolumen_discoFS;
	tablaMontajes[mount_id].desmontarVolumen = NULL;
	tablaMontajes[mount_id].mostrarMontaje = NULL;

	return mount_id;
}

/* -----------------------------------------------------*
 * Funciones especificas de cargar y descarga de modulos
 * -----------------------------------------------------*/

/*
 * Funcion que inicializa el modulo y carga el objeto FS de discoFS
 * en la tabla de sistemas de ficheros
 */
int cargarModulo_discoFS() {
	//Reservar entrada en la tabla de FS.
	int fs_id = reservarEntradaTablaFS("discoFS");
	
	//Inicializarla con los métodos y atributos del objeto FS de discoFS.
	tablaFS[fs_id].datosPropiosFS = NULL;
	tablaFS[fs_id].crearMontaje = crearMontaje_discoFS;
	tablaFS[fs_id].destruirFS = NULL;
	tablaFS[fs_id].mostrarFS = NULL;

	return 0;
}

