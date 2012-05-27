/*
 * usuario/test_bloque_cdrom.c
 *
 *  Programa para probar la escritura de los bloques de una imagen.
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

#include "servicios.h"

#define MAX_STRING 256	
#define TAM_BLOQUE 1024	

char * comparador = "cdcdcdcdcd";

int comprobar_valores(char * buffer, int num_bloque){
	int error_encontrado = 0;
	int index = 0;
	for(index = 0; (index < TAM_BLOQUE) && !error_encontrado; index++){
		if(buffer[index] != comparador[num_bloque]){
			printf("Num bloque: %d, index: %d, buffer: %c, comparador: %c\n", index, buffer[index], comparador[num_bloque]);
			error_encontrado = 1;
		}
	}
	printf("comprobar_valores :%d\n", error_encontrado);
	return error_encontrado;
}

void rellenarBuffer(char * buffer, char c){
	int index = 0;
	for(index = 0; index < TAM_BLOQUE; index++){
		buffer[index]=c;
	}
}

int main(){
	char bloque[TAM_BLOQUE];
	int fd_escritura;
	int ret;
	int num_bloque=0;
	int id_actual = obtener_id_proceso();

	printf("Comprobando escritura por bloques (%d)\n", id_actual);
	
	/* abrir fichero origen para escribir */
	fd_escritura = abrir ("/DISPOSITIVOS/disco",0);
	if (0 > fd_escritura) {
		printf("test_bloque_disco(%d): Error al abrir el disco para escritura\n", id_actual);
		printf("### Result: Fail\n");	
		return (-1);
	}

	/* inicializar el buffer */
	rellenarBuffer(bloque, comparador[num_bloque]);

	printf("test_bloque_disco (%d) escribir bloque %d\n", id_actual, num_bloque);

	ret = escribir(fd_escritura, bloque, TAM_BLOQUE);
	while (ret > 0){
		printf("resultado escribirBloque: %d\n", ret);
		num_bloque++;
		rellenarBuffer(bloque, comparador[num_bloque]);
		ret = escribir(fd_escritura, bloque, TAM_BLOQUE);
	}

	cerrar(fd_escritura);

	if(num_bloque != 10){
		printf("### Result: Fail\n");
	}else{
		printf("### Result: OK\n");
	}

	return (0);
}

