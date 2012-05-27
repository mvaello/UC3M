/*usuario/leer_fichero.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que lee un fichero 
 */

#include <strings.h>
#include "servicios.h"

#define MAX_STRING 256  
#define TAM_BUFFER 128  

int main(){

	char origen[MAX_STRING];
        char buffer[TAM_BUFFER+1];

	int leidos;
        int ret;
	int fd_origen;

        printf("Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
        ret=montar("disco", "lectura", "discoFS");
        if (0 > ret) {
                printf("leer_fichero: Error al montar particion lectura en disco con discoFS\n");
        }
	

	/* obtener los nombre de los ficheros */
        printf("Introduzca el fichero origen EJ: /FICHEROS/lectura/datos:");
        scanf("%s",origen);

        /* abrir fichero origen */
        fd_origen = abrir (origen,T_FICHERO);
        if (0 > fd_origen) {
                printf("leer_fichero: Error al abrir fichero origen %s\n", origen);
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

        cerrar(fd_origen);
	return (0);
}
