/*
 * usuario/leer_fichero_mul_varfic_child.c
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
	char file_name[257];
	char letra = (char) ('A' + obtener_id_proceso());
	snprintf(file_name, 257, "/FICHEROS/lectura/fichero_mul_varfic_child%c", letra);
	int fd512 = abrir(file_name, T_FICHERO);
	printf("leer_fichero_mul_varfic_child(%d) -> fd = %d (Nombre: %s)", obtener_id_proceso(), fd512, file_name);
	if(fd512 < 0) {
		printf("leer_fichero_mul_varfic_child: Error al abrir fichero origen %s\n", file_name);
		return -1;
	}

	//Mostrar fichero.
	int leidos;
	bzero(buffer, TAM_BUFFER + 1);
	leidos = leer(fd512, buffer, TAM_BUFFER);
	int leidos512 = leidos;
	while(leidos > 0) {
		printf("leer_fichero_mul_varfic_child(%d) -> %s", obtener_id_proceso(), buffer);
		bzero(buffer, TAM_BUFFER + 1);
		leidos = leer(fd512, buffer, TAM_BUFFER);
		leidos512 += leidos;
	}
	printf("leer_fichero_mul_varfic_child(%d) -> Leidos(512): %d\n", obtener_id_proceso(), leidos512);

	//Cerrar fichero.
	cerrar(fd512);

	return (0);
}
