/*
 * usuario/leer_escritura_varios_tamanyos.c
 */

/*
 * Programa de usuario que escribe varios ficheros con distintos tamaños, los sincroniza y los lee.
 */

#include <strings.h>
#include "servicios.h"
 
#define TAM_BUFFER 1024

void llenar_bloque(char letra, char* bloque, int tamanyo) {
	int i = 0;
	for (i = 0; i < tamanyo; i++) {
		bloque[i] = letra;
	}
}

int main() {
	char buffer[TAM_BUFFER + 1];

	/*
		MONTAJE DISCOFS
	*/
	//Montar disco.
	printf("Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
	if(montar("disco", "lectura", "discoFS") < 0) {
		printf("leer_escritura_varios_tamanyos: Error al montar particion lectura en disco con discoFS.\n");
	}

	/*
		ESCRITURA FICHEROS
	*/
	//Abrir ficheros
	int fd256 = abrir("/FICHEROS/lectura/leer_escritura_varios_tamanyos256", T_FICHERO);
	if(fd256 < 0) {
		printf("leer_escritura_varios_tamanyos: Error al abrir fichero origen leer_escritura_varios_tamanyos256\n");
		return -1;
	}
	int fd1024 = abrir("/FICHEROS/lectura/leer_escritura_varios_tamanyos1024", T_FICHERO);
	if(fd1024 < 0) {
		printf("leer_escritura_varios_tamanyos: Error al abrir fichero origen leer_escritura_varios_tamanyos1024\n");
		return -1;
	}
	int fd2048 = abrir("/FICHEROS/lectura/leer_escritura_varios_tamanyos2048", T_FICHERO);
	if(fd2048 < 0) {
		printf("leer_escritura_varios_tamanyos: Error al abrir fichero origen leer_escritura_varios_tamanyos2048\n");
		return -1;
	}

	//Llenar bloques
	char buffer256[256];
	char buffer1024[1024];
	char buffer2048[2048];
	llenar_bloque('E', buffer256, 256);
	llenar_bloque('F', buffer1024, 1024);
	llenar_bloque('G', buffer2048, 2048);

	//Escribir bloques.
	int escritos256 = escribir(fd256, buffer256, 256);
	int escritos1024 = escribir(fd1024, buffer1024, 1024);
	int escritos2048 = escribir(fd2048, buffer2048, 2048);
	printf("Escritos(256): %d\n", escritos256);
	printf("Escritos(1024): %d\n", escritos1024);
	printf("Escritos(2048): %d\n", escritos2048);

	//Cerrar ficheros
	cerrar(fd256);
	cerrar(fd1024);
	cerrar(fd2048);

	/*
		SINCRONIZADO DISCOFS
	*/
	printf("Sincronizando el volumen montado en /FICHEROS/lectura\n");
	if(sincronizar("lectura") < 0) {
		printf("sincronizar_vol: Error al sincronizar el volumen\n");
	}

	/*
		LECTURA DISCOFS
	*/
	//Abrir ficheros
	fd256 = abrir("/FICHEROS/lectura/leer_escritura_varios_tamanyos256", T_FICHERO);
	if(fd256 < 0) {
		printf("leer_escritura_varios_tamanyos: Error al abrir fichero origen leer_escritura_varios_tamanyos256\n");
		return -1;
	}
	fd1024 = abrir("/FICHEROS/lectura/leer_escritura_varios_tamanyos1024", T_FICHERO);
	if(fd1024 < 0) {
		printf("leer_escritura_varios_tamanyos: Error al abrir fichero origen leer_escritura_varios_tamanyos256\n");
		return -1;
	}
	fd2048 = abrir("/FICHEROS/lectura/leer_escritura_varios_tamanyos2048", T_FICHERO);
	if(fd2048 < 0) {
		printf("leer_escritura_varios_tamanyos: Error al abrir fichero origen leer_escritura_varios_tamanyos256\n");
		return -1;
	}

	//Mostrar ficheros.
	int leidos;
	bzero(buffer, TAM_BUFFER + 1);
	leidos = leer(fd256, buffer, TAM_BUFFER);
	int leidos256 = leidos;
	while(leidos > 0) {
		printf("%s",buffer);
		bzero(buffer, TAM_BUFFER + 1);
		leidos = leer(fd256, buffer, TAM_BUFFER);
		leidos256 += leidos;
	}
	printf("Leidos(256): %d\n", leidos256);
	bzero(buffer, TAM_BUFFER + 1);
	leidos = leer(fd1024, buffer, TAM_BUFFER);
	int leidos1024 = leidos;
	while(leidos > 0) {
		printf("%s",buffer);
		bzero(buffer, TAM_BUFFER + 1);
		leidos = leer(fd1024, buffer, TAM_BUFFER);
		leidos1024 += leidos;
	}
	printf("Leidos(1024): %d\n", leidos1024);
	bzero(buffer, TAM_BUFFER + 1);
	leidos = leer(fd2048, buffer, TAM_BUFFER);
	int leidos2048 = leidos;
	while(leidos > 0) {
		printf("%s",buffer);
		bzero(buffer, TAM_BUFFER + 1);
		leidos = leer(fd2048, buffer, TAM_BUFFER);
		leidos2048 += leidos;
	}
	printf("Leidos(2048): %d\n", leidos2048);

	//Cerrar ficheros
	cerrar(fd256);
	cerrar(fd1024);
	cerrar(fd2048);

	return 0;
}
