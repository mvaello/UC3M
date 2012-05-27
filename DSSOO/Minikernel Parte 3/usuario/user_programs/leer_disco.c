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
	int ret;
	int num=0;

	printf("mostrar el cdrom por pantalla");
	
	/* abrir fichero origen */
	fd_disco = abrir ("/DISPOSITIVOS/disco",0);
	if (0 > fd_disco) {
		printf("leer_disco: Error al abrir el disco\n");
		return (-1);
	}

	/* mostra disco por pantalla */
	printf("leer_disco: leyendo bloque %d\n\n\n",num);
	ret = leer(fd_disco,bloque,TAM_BLOQUE);
	while (0 < ret) { 
		printf("%s",bloque);
		num=num+1;
		printf("\n\nleer_disco: leyendo bloque %d\n\n\n",num);
		ret = leer(fd_disco,bloque,TAM_BLOQUE);
	}
	
	
	/* cerrar disco  */
	cerrar (fd_disco);

	return (0);
}

