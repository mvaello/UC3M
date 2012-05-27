/*
 * usuario/abrir_fichero_exista_noexista.c
 */

/*
 * Programa de usuario que abre un fichero exista o no exista.
 */

#include <strings.h>
#include "servicios.h"

#define TAM_BUFFER 1024 

int main() {
	char* fichero_existente = "/FICHEROS/lectura/leer_escritura_varios_tamanyos256";
	char* fichero_no_existente = "/FICHEROS/lectura/leer_escritura_varios_tamanyos64";

	//Montar disco.
	printf("abrir_fichero_exista_noexista -> Montando el disco en la particion /FICHEROS/lectura con discoFS\n");
	if(montar("disco", "lectura", "discoFS") < 0) {
		printf("abrir_fichero_exista_noexista -> Error al montar particion lectura en disco con discoFS.\n");
	}

	//Abrir ficheros
	int fd64 = abrir(fichero_no_existente, T_FICHERO);
	if(fd64 < 0) {
		printf("abrir_fichero_exista_noexista: Error al abrir fichero origen %s\n", fichero_no_existente);
		return -1;
	}
	int fd256 = abrir(fichero_existente, T_FICHERO);
	if(fd256 < 0) {
		printf("abrir_fichero_exista_noexista: Error al abrir fichero origen %s\n", fichero_existente);
		return -1;
	}

	printf("abrir_fichero_exista_noexista -> %s -> fd = %d", fichero_no_existente, fd64);
	printf("abrir_fichero_exista_noexista -> %s -> fd = %d", fichero_existente, fd256);

	//Mostrar ficheros.
	char buffer[TAM_BUFFER + 1];
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
	printf("Leidos(%s): %d\n", fichero_existente, leidos256);

	bzero(buffer, TAM_BUFFER + 1);
	leidos = leer(fd64, buffer, TAM_BUFFER);
	int leidos64 = leidos;
	while(leidos > 0) {
		printf("%s",buffer);
		bzero(buffer, TAM_BUFFER + 1);
		leidos = leer(fd64, buffer, TAM_BUFFER);
		leidos64 += leidos;
	}
	printf("Leidos(%s): %d\n", fichero_no_existente, leidos64);

	//Cerrar ficheros
	cerrar(fd64);
	cerrar(fd256);

	return 0;
}
