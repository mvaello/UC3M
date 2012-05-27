/*
 * usuario/test_e2_4_escribir_bloque.c
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

	int fd_disco = -1;
	int ret = -1;

	char bufferE[1025];

	memset(bufferE, 'c', 1024);
	bufferE[1024] = '\0';

	/* abrir fichero disco */
	fd_disco = abrir ("/DISPOSITIVOS/disco", 0);
	if (0 > fd_disco) {
		printf("test_e2_4_escribir_bloque: Error al abrir el disco \n");
		printf("### Result: Fail\n");
		return (-1);
	}

	ret = escribir(fd_disco, bufferE, 1024);
	if ( 0 > ret){
		printf("test_e2_4_escribir_bloque: Error al escribir en el disco \n");
		printf("### Result: Fail\n");
		return (-1);
	}
	
	printf("### Result: OK\n");
	return 0;
}
