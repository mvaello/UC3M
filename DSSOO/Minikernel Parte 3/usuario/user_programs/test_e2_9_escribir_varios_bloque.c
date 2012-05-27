/*
 * usuario/test_e2_9_escribir_varios_bloque.c
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
		if (crear_proceso("test_e2_4_escribir_bloque")<0){
			printf("test_e2_9_escribir_varios_bloque: Error al crear el proceso test_e2_2_leer_bloque\n");
		}
	}

	printf("Procesos lanzados.\n");
	return 0;
}
