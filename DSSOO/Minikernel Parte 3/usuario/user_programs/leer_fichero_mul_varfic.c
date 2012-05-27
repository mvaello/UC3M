/*
 * usuario/leer_fichero_mul_varfic.c
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
	printf("leer_fichero_mul_varfic -> Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
	if(montar("disco", "lectura", "discoFS") < 0) {
		printf("leer_fichero_mul_varfic -> Error al montar particion lectura en disco con discoFS.\n");
	}

	int i = 0;
	for(i = 0; i < 5; i++) {
		if(crear_proceso("leer_fichero_mul_varfic_child") < 0) {
			printf("Error creando leer_fichero_mul_varfic_child.\n");
		}
	}

	return (0);
}
