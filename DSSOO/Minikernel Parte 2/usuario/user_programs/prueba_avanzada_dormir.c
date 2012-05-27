#include "servicios.h"

int main(){

	printf("prueba_avanzada_dormir: comienza\n");

	if (crear_proceso("yosoy_duerme")<0)
		printf("Error creando yosoy baja duerme\n");

	if (crear_proceso("yosoy")<0)
		printf("Error creando yosoy baja\n");

        if (crear_proceso_alta_prioridad("yosoy_duerme")<0)
                printf("Error creando yosoy alta duerme\n");

        if (crear_proceso_alta_prioridad("yosoy")<0)
                printf("Error creando yosoy alta\n");


	printf("prueba_avanzada_dormir: termina\n");

	return 0; 
}
