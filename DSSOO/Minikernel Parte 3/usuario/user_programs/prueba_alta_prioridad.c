#include "servicios.h"

int main(){

	printf("prueba_alta_prioridad: comienza\n");

	if (crear_proceso("yosoy")<0)
		printf("Error creando yosoy baja\n");

	if (crear_proceso("yosoy")<0)
		printf("Error creando yosoy baja\n");

        if (crear_proceso_alta_prioridad("yosoy")<0)
                printf("Error creando yosoy alta\n");

        if (crear_proceso_alta_prioridad("yosoy")<0)
                printf("Error creando yosoy alta\n");


	printf("prueba_alta_prioridad: termina\n");

	return 0; 
}
