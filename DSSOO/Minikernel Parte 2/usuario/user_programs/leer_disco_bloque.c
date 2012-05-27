/*
 * usuario/leer_disco.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que muestra el contenido del disco por pantalla
 */

#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BLOQUE 1024	

int main(){
	char bloque[TAM_BLOQUE];

	int fd_disco;
	int num_bloque = 1;
	int iter = 50;

	printf("[Bloque %d] [Iteraciones %d]\n", num_bloque, iter);

	printf("mostrar el disco por pantalla");
	
	/* abrir fichero origen */
	fd_disco = abrir ("/DISPOSITIVOS/disco",0);
	if (0 > fd_disco) {
		printf("leer_disco: Error al abrir el disco\n");
		return (-1);
	}

	int i;
	for(i = 0; i < iter; i++) {
		printf("\n\nleer_disco: leyendo bloque %d\n\n\n", num_bloque);

		printf("%s",bloque);

		int ret = leer(fd_disco, bloque, TAM_BLOQUE);
		if(ret < 0) {
			break;
		}
	}
	
	/* cerrar disco  */
	cerrar (fd_disco);

	return (0);
}

