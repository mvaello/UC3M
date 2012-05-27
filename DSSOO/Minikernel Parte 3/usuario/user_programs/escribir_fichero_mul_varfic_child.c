/*
 * usuario/escribir_fichero_mul_varfic_child.c
 */

/*
 * Programa de usuario que escribe varios ficheros con distintos tamaños, los sincroniza y los lee.
 */

#include <strings.h>
#include "servicios.h"

void llenar_bloque(char letra, char* bloque, int tamanyo) {
	int i = 0;
	for (i = 0; i < tamanyo; i++) {
		bloque[i] = letra;
	}
}

int main() {
	/*
		ESCRITURA FICHEROS
	*/
	//Abrir fichero.
	char file_name[257];
	char letra = (char) ('A' + obtener_id_proceso());
	snprintf(file_name, 257, "/FICHEROS/lectura/fichero_mul_varfic_child%c", letra);
	int fd512 = abrir(file_name, T_FICHERO);
	printf("escribir_fichero_mul_varfic_child(%d) -> fd = %d (Nombre: %s)", obtener_id_proceso(), fd512, file_name);
	if(fd512 < 0) {
		printf("escribir_fichero_mul_varfic_child(%d) ->  Error al abrir fichero origen %s\n", obtener_id_proceso(), file_name);
		return -1;
	}

	//Llenar bloque.
	char buffer512[512];
	
	llenar_bloque(letra, buffer512, 512);

	//Escribir bloque.
	int escritos512 = escribir(fd512, buffer512, 512);
	printf("escribir_fichero_mul_varfic_child(%d) -> Escritos(512%c): %d\n", obtener_id_proceso(), letra, escritos512);

	//Cerrar fichero.
	cerrar(fd512);

	/*
		SINCRONIZADO DISCOFS
	*/
	printf("escribir_fichero_mul_varfic_child(%d) -> Sincronizando el volumen montado en /FICHEROS/lectura\n", obtener_id_proceso());
	if(sincronizar("lectura") < 0) {
		printf("escribir_fichero_mul_varfic_child(%d) -> Error al sincronizar el volumen\n", obtener_id_proceso());
	}

	return 0;
}
