/*
 * usuario/excep_arit.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 * Programa de usuario que fuerza una excepción aritmética
 */

#include "servicios.h"

#define TOT_ITER 200	/* número total de iteraciones */
#define ITER_EXCEP 22	/* iteración en la que causa excepción */

int tot=0;

int main(){
	int i;
	int tot=0;

	for (i=0; i<TOT_ITER; i++){
		printf("excep_arit: i %d \n", i);

		/* Forzando una excepción */
		if ((i+1)%ITER_EXCEP==0)
			i/=tot;
	}
	/* No debería llegar ya que ha generado una excepción */
	printf("excep_arit: termina\n");
	return 0;
}
