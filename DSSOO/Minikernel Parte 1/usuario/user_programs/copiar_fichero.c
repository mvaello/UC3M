/*
 * usuario/copiar_fichero.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que copia un fichero en el disco
 */

#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BUFFER 256	

int main(){
	char origen[MAX_STRING];
	char destino[MAX_STRING];
	char buffer[TAM_BUFFER];

	int fd_origen, fd_destino;
	int leidos, escritos;
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
	ret=montar("cdrom","lectura", "cdromFS");
	if (0 > ret) {
		printf("Copiar_fichero: Error al montar particion lectura en cdrom con cdromFS\n");
		return (-1);
	}
	/* montar disco */
	printf("Montando el disco en la particion /FICHEROS/escritura con discoFS\n");
	ret=montar("disco","escritura", "discoFS");
	if (0 > ret) {
		printf("Copiar_fichero: Error al montar particion escritura en disco con discoFS\n");
		return (-1);
	}

	
	

	/* obtener los nombre de los ficheros */
	printf("Introduzca el fichero origen EJ: /FICHEROS/lectura/datos:");
	scanf("%s",origen);
	printf("Introduzca el fichero destino EJ: /FICHEROS/escritura/resultado:");
	scanf("%s",destino);
	
	/* abrir fichero origen */
	fd_origen = abrir (origen, T_FICHERO);
	if (0 > fd_origen) {
		printf("Copiar_fichero: Error al abrir fichero origen %s\n", origen);
		return (-1);
	}
	
	/* abrir fichero destino */
	fd_destino = abrir (destino, T_FICHERO);
	if (0 > fd_destino) {
		printf("Copiar_fichero: Error al abrir fichero destino %s\n", destino);
		return (-1);
	}

	/* copiar origen en destino */
	leidos = leer(fd_origen,buffer,TAM_BUFFER);
	while (0 < leidos) { 
		escritos = escribir(fd_destino,buffer,leidos);
		if (leidos != escritos) {
			printf("Copiar_fichero: Error al escribir fichero destino %s\n", destino);
			return (-1);
		}
		leidos = leer(fd_origen,buffer,TAM_BUFFER);
	}
	
	
	/* cerrar fichero origen */
	cerrar (fd_origen);

	/* cerrar fichero destino */
	cerrar (fd_destino);
	

	/* sincronizar las escrituras */
	printf("Escribiendo los cambios de la particion /FICHEROS/escritura en el disco\n");

	sincronizar("escritura");
	return 0;
}

