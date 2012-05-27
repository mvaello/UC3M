/*
 * usuario/test_e4_1_abrir_disco.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 */
#include <string.h>
#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BUFFER 256	

int main(){

	int retDebug;
	int fd_origen = -1;
	int fd_disco = -1;
	int leidos = -1;
	int seek = 0;

	//PUEDE NECESITAR CAMBIO EN FUNCION DEL NUMERO DE DESCRIPTOR Y DEL NUMERO DE DISPOSITIVO
	char cadenaDescFichero[200] = "DESCFICHERO:\tID=2, tipo=0, inodo=-1, montaje=-1, Dispositivo=3\n";
	//char cadenaDescFichero[200] = "DESCFICHERO:\tID=2, tipo=0, inodo=-1, montaje=-1, Dispositivo=2\n";

	char buffer[1024];
	char *encontrado = NULL;

	buffer[1023] = '\0';


	/* montar debugfs */
	//printf("Montando la depuracion en la particion /FICHEROS/proc con debugFS\n");
	retDebug=montar("", "proc", "debugFS");
	if (0 > retDebug) {
		printf("test_e2_1_abrir_disco: Error al montar particion de depuracion con debugFS\n");
		printf("### Result: Fail\n");
		return (-1);
	}

	/* abrir fichero disco */
	fd_disco = abrir ("/DISPOSITIVOS/disco", 0);
	if (0 > fd_disco) {
		printf("test_e2_1_abrir_disco: Error al abrir el disco \n");
		printf("### Result: Fail\n");
		return (-1);
	}


	/* abrir fichero tabla de descriptores (tabla_procesos) */
	fd_origen = abrir ("/FICHEROS/proc/tabla_procesos", T_FICHERO);
	if (0 > fd_origen) {
		printf("test_e2_1_abrir_disco: Error al abrir fichero tabla_descriptores \n");
		printf("### Result: Fail\n");
		return (-1);
	}

	/*leemos la tabla de descriptores*/
	bzero(buffer, 1023);
	seek = 0;
	leidos = leer(fd_origen,buffer,1023);
	
	while (0 < leidos && leidos != 1023) { 
		seek += leidos;
		leidos = leer(fd_origen,&(buffer[seek]),1023-seek);
	}

	printf("%s\n\n\n", buffer);	//Falla con ficheros de imagen que tengan archivos de tamaño mayor que un bloque

	cerrar(fd_origen);
	cerrar(fd_disco);
	
	/*buscamos datos en la tabla de descriptores*/
	encontrado = strstr(buffer, cadenaDescFichero);

	if(encontrado == NULL)
	{
		printf("test_e2_1_abrir_disco: Datos en la tabla de descriptores no encontrados \n");
		printf("### Result: Fail\n");
		return -1;
	}

	printf("### Result: OK\n");
	return 0;
}
