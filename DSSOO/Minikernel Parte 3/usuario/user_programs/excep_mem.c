/*
 * usuario/excep_mem.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que fuerza una excepción en acceso a memoria.
 * Además crea un proceso hijo (simplon).
 */

#include "servicios.h"

#define TOT_ITER 200	/* Número total de iteraciones */
#define ITER_EXCEP 20	/* iteración en la que causa excepción */

int main(){
	int i;
	int *p=0;

	if (crear_proceso("simplon")<0)
		printf("Error creando simplon\n");

	for (i=0; i<TOT_ITER; i++){
		printf("excep_mem: i %d\n", i);
		if ((i+1)%ITER_EXCEP==0)
			*p=5;
	}
	printf("excep_mem: termina\n");
	terminar_proceso();
	return 0; /* No se debería llegar a este punto */
}

