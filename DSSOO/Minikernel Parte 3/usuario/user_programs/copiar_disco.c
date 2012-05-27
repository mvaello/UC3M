/*
 * usuario/copiar_disco.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que copia el cdrom en el disco
 */

#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BLOQUE 1024	

int main(){
	char bloque[TAM_BLOQUE];

	int fd_cdrom, fd_disco;
	int ret,num=0;

	printf("Copiar el cdrom en el disco\n");
	
	/* abrir fichero origen */
	fd_cdrom = abrir ("/DISPOSITIVOS/cdrom",0);
	if (0 > fd_cdrom) {
		printf("copiar_disco: Error al abrir el cdrom\n");
		return (-1);
	}
	
	/* abrir fichero destino */
	fd_disco = abrir ("/DISPOSITIVOS/disco",0);
	if (0 > fd_disco) {
		printf("copiar_disco: Error al abrir el disco\n");
		return (-1);
	}


	/* copiar origen en destino */
	printf("copiar_disco: leyendo bloque %d\n",num);
	ret = leer(fd_cdrom,bloque,TAM_BLOQUE);
	while (0 < ret) { 
		printf("copiar_disco: escribiendo bloque %d\n",num);
		ret = escribir(fd_disco,bloque,TAM_BLOQUE);
		if (0 > ret) {
			printf("copiar_disco: Error al escribir bloque %d\n",num);
			return (-1);
		}
		num=num+1;
		printf("\n\nmostrar_disco: leyendo bloque %d\n\n\n",num);
		ret = leer(fd_cdrom,bloque,TAM_BLOQUE);
	}
	
	
	/* cerrar cdrom */
	cerrar (fd_cdrom);

	/* cerrar disco */
	cerrar (fd_disco);
	
	return (0);
}

