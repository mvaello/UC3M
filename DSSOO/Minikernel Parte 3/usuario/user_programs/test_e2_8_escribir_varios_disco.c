/*
 * usuario/test_e2_8_escribir_varios_disco.c
 *
 *
 *  Minikernel. Versión 1.0
 *
 * @author Alberto Garcia
 *
 */

/*
 */

#include "servicios.h"

int main(){
	int i;
	
	printf("Creando procesos . . . \n");

	for(i=0; i<8; i++)
	{
		if (crear_proceso("test_e2_5_escribir_disco")<0){
			printf("test_e2_8_escribir_varios_disco: Error al crear el proceso test_e2_5_escribir_disco\n");
		}
	}

	printf("Procesos lanzados.\n");
	return 0;
}
