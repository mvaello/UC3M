/*
 * usuario/test_e2_2_leer_bloque.c
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

void mostrarBuffer(char * buffer){
        char buffer_mostrar[1025];
        bzero(buffer_mostrar, 1025);
        memcpy(buffer_mostrar, buffer, 1025);
        fprintf(stdout, "Bloque:\n{%*s}FIN\n", 1024, buffer_mostrar);

}

int main(){

	int fd_disco = -1;
	int ret = -1;
	int comparador;

	char bufferL[1025];
	char bufferE[1025];

	memset(bufferE, 'a', 1024);
	bufferE[1024] = '\0';
	bufferL[1024] = '\0';

	/* abrir fichero disco */
	fd_disco = abrir ("/DISPOSITIVOS/disco", 0);
	if (0 > fd_disco) {
		printf("test_e2_2_leer_bloque: Error al abrir el disco \n");
		printf("### Result: Fail\n");
		return (-1);
	}

	ret = leer(fd_disco, bufferL, 1024);
	if ( 0 > ret){
		printf("test_e2_2_leer_bloque: Error al leer del disco \n");
		printf("### Result: Fail\n");
		return (-1);
	}

	mostrarBuffer(bufferE);
	mostrarBuffer(bufferL);	

	/* comparamos datos extraidos */
	comparador = strcmp(bufferE, bufferL);

	if(comparador != 0)
	{
		printf("test_e2_2_leer_bloque: Datos en el disco no encontrados \n");
		printf("### Result: Fail\n");
		return -1;
	}
	
	printf("### Result: OK\n");
	return 0;
}
