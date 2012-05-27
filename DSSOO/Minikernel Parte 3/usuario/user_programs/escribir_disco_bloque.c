/*
 * usuario/escribir_disco.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que escribe en disco
 */

#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BLOQUE 1024	


void llenar_bloque(char letra, char * bloque){
	int i = 0;
	for (i = 0; i < TAM_BLOQUE; i++){
		bloque[i] = letra;
	}
}

int main(){
	char bloque[TAM_BLOQUE];
	int fd_disco;
	int num_bloque = 0;
	int iter = 50;
	char letra = 'A';

	printf("[Bloque %d] [Iteraciones %d]\n", num_bloque, iter);
	
	/* abrir fichero origen */
	fd_disco = abrir ("/DISPOSITIVOS/disco",0);
	if (0 > fd_disco) {
		printf("escribir_disco: Error al abrir el disco\n");
		return (-1);
	}

	int i;
	for(i = 0; i < iter; i++) {
		letra++;
		if(letra > 'Z'){
			letra = 'A';
		}
		llenar_bloque(letra, bloque);

		printf("\n\nmostrar_disco: escribiendo bloque %d\n\n\n", num_bloque);

		int ret = escribir(fd_disco, bloque, TAM_BLOQUE);
		if(ret < 0) {
			break;
		}
	}
	
	/* cerrar disco */
	cerrar (fd_disco);

	return (0);
}


