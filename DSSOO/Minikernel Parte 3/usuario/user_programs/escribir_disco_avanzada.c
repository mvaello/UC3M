#include "servicios.h"

int main(){

	printf("escribir_disco_avanzada: comienza\n");

	if (crear_proceso("escribir_disco")<0)
		printf("Error creando escribir_disco baja\n");

	if (crear_proceso("escribir_disco")<0)
		printf("Error creando escribir_disco baja\n");

	if (crear_proceso("escribir_disco")<0)
		printf("Error creando escribir_disco baja\n");

	printf("escribir_disco_avanzada: termina\n");

	return 0; 
}
