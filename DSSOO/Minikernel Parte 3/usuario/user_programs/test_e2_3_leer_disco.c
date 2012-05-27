/*
 * usuario/test_e2_3_leer_disco.c
 *
 *  Programa para probar la lectura de los bloques de una imagen. La imagen debe contener
 *  10 bloques conteniendo el bloque 0, todo a, el bloque 1 todo b, el bloque 2 todo a, hasta el bloque 9 que
 *  contendr√°todo b.
 *
 *  Minikernel. VersiÛn 1.0
 *
 *  Fernando PÈrez Costoya
 *
 */

#include "servicios.h"

char * comparador = "ababababab";

int comprobar_valores(char * buffer, int num_bloque){
	int error_encontrado = 0;
	int index = 0;
	for(index = 0; (index < 1024) && !error_encontrado; index++){
		if(buffer[index] != comparador[num_bloque]){
			printf("Num bloque: %d, index: %d, buffer: %c, comparador: %c\n", index, buffer[index], comparador[num_bloque]);
			error_encontrado = 1;
		}
	}
	printf("comprobar_valores :%d\n", error_encontrado);
	return error_encontrado;
}

int main(){
	char bloque[1025];

	int fd_disco;
	int ret;
	int num_bloque=0;
	int error_encontrado = 0;

	int id_actual = obtener_id_proceso();

	printf("Comprobando lectura por bloques (%d)\n", id_actual);
	
	/* limpiar el bloque */
	for(ret = 0; ret < 1024; ret ++){
		bloque[ret] = 'n';
	}
	bloque[1024] = '\0';


	/* abrir fichero origen */
	fd_disco = abrir ("/DISPOSITIVOS/disco",0);
	if (0 > fd_disco) {
		printf("test_bloque_cdrom(%d): Error al abrir el cdrom\n", id_actual);
		printf("### Result: Fail\n");	
		return (-1);
	}

	/* mostra cdrom por pantalla */
	printf("test_disco_cdrom (%d) leyendo bloque %d\n", id_actual, num_bloque);
	ret = leer(fd_disco, bloque, 1024);

	error_encontrado = comprobar_valores(bloque, num_bloque);

	while ((0 < ret) && !error_encontrado) { 
        	fprintf(stdout, "{%s}FIN\n", bloque);
		num_bloque++;
		printf("test_disco_cdrom (%d) leyendo bloque %d\n", id_actual, num_bloque);
		ret = leer(fd_disco, bloque, 1024);
		if(ret > 0){
			error_encontrado = comprobar_valores(bloque, num_bloque);
		}
	}
	
	printf("Bloque final: %d, error_encontrado: %d\n", num_bloque, error_encontrado);
	/* El √ltimo bloque leido no deber√a sobrepasar la imagen. */
	if(num_bloque != 10){
		printf("### Result: Fail\n");
	}else{
		if(!error_encontrado){
			printf("### Result: OK\n");
		}else{
			printf("### Result: Fail\n");
		}
	}
	
	/* cerrar cdrom */
	cerrar (fd_disco);

	return (0);
}

