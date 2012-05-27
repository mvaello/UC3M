/*
 * usuario/leer_fichero_mul_child.c
 */

/*
 * Programa de usuario que escribe varios ficheros con distintos tamaños, los sincroniza y los lee.
 */

#include <strings.h>
#include "servicios.h"
 
#define TAM_BUFFER 512

int main() {
	char buffer[TAM_BUFFER + 1];

	/*
		LECTURA DISCOFS
	*/
	//Abrir fichero.
	int fd512 = abrir("/FICHEROS/lectura/fichero_mul_child512", T_FICHERO);
	printf("leer_fichero_mul_child(%d) -> fd = %d", obtener_id_proceso(), fd512);
	if(fd512 < 0) {
		printf("leer_fichero_mul_child: Error al abrir fichero origen fichero_mul_child512\n");
		return -1;
	}

	//Mostrar fichero.
	int leidos;
	bzero(buffer, TAM_BUFFER + 1);
	leidos = leer(fd512, buffer, TAM_BUFFER);
	int leidos512 = leidos;
	while(leidos > 0) {
		printf("leer_fichero_mul_child(%d) -> %s", obtener_id_proceso(), buffer);
		bzero(buffer, TAM_BUFFER + 1);
		leidos = leer(fd512, buffer, TAM_BUFFER);
		leidos512 += leidos;
	}
	printf("leer_fichero_mul_child(%d) -> Leidos(512): %d\n", obtener_id_proceso(), leidos512);

	//Cerrar fichero.
	cerrar(fd512);

	return (0);
}
