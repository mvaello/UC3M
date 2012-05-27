/*usuario/abrir_fichero.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que abre un fichero 
 */

#include <strings.h>
#include "servicios.h"

#define MAX_STRING 256  
#define TAM_BUFFER 128  

int main(){

	char origen[MAX_STRING];
        int ret;
	int fd_origen;

        printf("Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
        ret=montar("disco", "lectura", "discoFS");
        if (0 > ret) {
                printf("montar_vol: Error al montar particion lectura en disco con discoFS\n");
        }

	/* obtener los nombre de los ficheros */
        printf("Introduzca el fichero origen EJ: /FICHEROS/lectura/datos:");
        scanf("%s",origen);

	printf("Abriendo el fichero origen %s\n", origen);

        /* abrir fichero origen */
        fd_origen = abrir (origen,T_FICHERO);
        if (0 > fd_origen) {
                printf("abrir_fichero: Error al abrir fichero origen %s\n", origen);
                return (-1);
        }
	
        cerrar (fd_origen);
	return (0);
}
