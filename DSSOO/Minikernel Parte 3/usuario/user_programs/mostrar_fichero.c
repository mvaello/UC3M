/*
 * usuario/mostrar_fichero.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que muestra un fichero por pantalla
 */

#include <strings.h>
#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BUFFER 128	

int main(){
	char origen[MAX_STRING];
	char buffer[TAM_BUFFER+1];

	int fd_origen;
	int leidos;
	int ret;

	/* montar debugfs */
	printf("Montando la depuracion en la particion /FICHEROS/proc con debugFS\n");
	ret=montar("", "proc", "debugFS");
	if (0 > ret) {
		printf("mostrar_fichero: Error al montar particion de depuracion con debugFS\n");
		return (-1);
	}
	/* montar cdrom */
	printf("Montando el cdrom en la particion /FICHEROS/lectura con  cdromFS\n");
	ret=montar("cdrom", "lectura", "cdromFS");
	if (0 > ret) {
		printf("mostrar_fichero: Error al montar particion lectura en cdrom con cdromFS\n");
	}
	
	/* obtener los nombre de los ficheros */
	printf("Introduzca el fichero origen EJ: /FICHEROS/lectura/datos:");
	scanf("%s",origen);
	
	/* abrir fichero origen */
	fd_origen = abrir (origen,T_FICHERO);
	if (0 > fd_origen) {
		printf("mostrar_fichero: Error al abrir fichero origen %s\n", origen);
		return (-1);
	}

	/* mostrar fichero por pantalla */
	bzero(buffer, TAM_BUFFER+1);
	leidos = leer(fd_origen,buffer,TAM_BUFFER);
	while (0 < leidos) { 
		printf("%s",buffer);
		bzero(buffer, TAM_BUFFER+1);
		leidos = leer(fd_origen,buffer,TAM_BUFFER);
	}
	
	/* cerrar fichero origen */
	cerrar (fd_origen);

	return 0;
}

