#include "servicios.h"

int main(){

	printf("leer_disco_avanzada: comienza\n");

	if (crear_proceso("leer_disco")<0)
		printf("Error creando leer_disco baja\n");

	if (crear_proceso("leer_disco")<0)
		printf("Error creando leer_disco baja\n");

	if (crear_proceso("leer_disco")<0)
		printf("Error creando leer_disco baja\n");

	printf("leer_disco_avanzada: termina\n");

	return 0; 
}
