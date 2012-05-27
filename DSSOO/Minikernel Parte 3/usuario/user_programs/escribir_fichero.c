/*usuario/escribir_fichero.c
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

void llenar_bloque(char letra, char * bloque){
        int i = 0;
        for (i = 0; i < TAM_BUFFER; i++){
                bloque[i] = letra;
        }
}


int main(){

	char origen[MAX_STRING];
        char buffer[TAM_BUFFER];

	int escritos;
        int ret;
	int fd_origen;

        printf("Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
        ret=montar("disco", "lectura", "discoFS");
        if (0 > ret) {
                printf("escribir_fichero: Error al montar particion lectura en disco con discoFS\n");
        }
	

	/* obtener los nombre de los ficheros */
        printf("Introduzca el fichero destino EJ: /FICHEROS/lectura/datos:");
        scanf("%s",origen);

        /* abrir fichero origen */
        fd_origen = abrir (origen,T_FICHERO);
        if (0 > fd_origen) {
                printf("escribir_fichero: Error al abrir fichero destino %s\n", origen);
                return (-1);
        }

	llenar_bloque('A', buffer);
        
        escritos = escribir(fd_origen,buffer,TAM_BUFFER);
		printf("%d\n", escritos);
        
        cerrar(fd_origen);

	return (0);
}
