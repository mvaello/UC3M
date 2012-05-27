/*
 * usuario/mostrar_disco.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que muestra del cdrom por pantalla
 */

#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BLOQUE 1024	

int main(){
	char bloque[TAM_BLOQUE];

	int fd_cdrom;
	int ret;
	int num=0;

	printf("mostrar el cdrom por pantalla");
	
	/* abrir fichero origen */
	fd_cdrom = abrir ("/DISPOSITIVOS/cdrom",0);
	if (0 > fd_cdrom) {
		printf("copiar_disco: Error al abrir el cdrom\n");
		return (-1);
	}

	/* mostra cdrom por pantalla */
	printf("mostrar_disco: leyendo bloque %d\n\n\n",num);
	ret = leer(fd_cdrom,bloque,TAM_BLOQUE);
	while (0 < ret) { 
		printf("%s",bloque);
		num=num+1;
		printf("\n\nmostrar_disco: leyendo bloque %d\n\n\n",num);
		ret = leer(fd_cdrom,bloque,TAM_BLOQUE);
	}
	
	
	/* cerrar cdrom */
	cerrar (fd_cdrom);

	return (0);
}

