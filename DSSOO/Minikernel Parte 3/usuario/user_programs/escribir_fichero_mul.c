/*
 * usuario/escribir_fichero_mul.c
 */

/*
 * Programa de usuario que escribe varios ficheros con distintos tamaños, los sincroniza y los lee.
 */

#include <strings.h>
#include "servicios.h"

int main() {
	/*
		MONTAJE DISCOFS
	*/
	//Montar disco.
	printf("escribir_fichero_mul -> Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
	if(montar("disco", "lectura", "discoFS") < 0) {
		printf("escribir_fichero_mul -> Error al montar particion lectura en disco con discoFS.\n");
	}

	int i = 0;
	for(i = 0; i < 5; i++) {
		if(crear_proceso("escribir_fichero_mul_child") < 0) {
			printf("Error creando escribir_fichero_mul_child.\n");
		}
	}

	return (0);
}
