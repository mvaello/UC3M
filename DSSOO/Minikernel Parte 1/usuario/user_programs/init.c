/*
 * usuario/init.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/* Programa inicial que arranca el S.O. Sólo se ejecutarán los programas
   que se incluyan aquí, que, evidentemente, pueden ejecutar otros
   programas...
*/

/*
 *
 * Contiene las mismas pruebas que se realizan para evaluar la práctica.
 * Para llevar a cabo cada prueba, comente y descomente
 * las líneas correspondientes. En la versión inicial, la parte descomentada
 * se corresponde con funcionalidad ya implementada en el material de apoyo.
 *
 */

#include "servicios.h"

int main(){
int num;
int i;


    printf("init: comienza\n");

	printf("Menu de pruebas\n\n");
	
	printf("1.- Prueba simple\n");
	printf("2.- Excepcion aritmetica\n");
	printf("3.- Excepcion memoria\n");
	printf("4.- Nuevas llamadas\n");
	printf("5.- Dormir procesos\n");
	printf("6.- Planificacion Round-Robin\n");
	printf("7.- Mostrar disco\n");
	printf("8.- Mostrar fichero\n");
	printf("9.- Copiar disco\n");
	printf("10.-Copiar fichero\n");
	printf("11.-Dormilon\n");
	printf("12.-Alta prioridad\n");
	printf("13.-Prueba avanzada dormir\n");
	printf("\nIntroduzca su opcion:");
	scanf("%d",&num);


	switch (num) {
	case 1:
		/* PRUEBA CREAR PROCESO QUE NO EXISTE */
		if (crear_proceso("noexiste")<0)
			printf("Error creando noexiste\n");

		/* PRUEBA CREAR PROCESO SIMPLE */
		if (crear_proceso("simplon")<0)
			printf("Error creando simplon\n");
		break;
	case 2:
		/* PRUEBA PROVOCAR EXCEPCION ARITMETICA */
		if (crear_proceso("excep_arit")<0)
			printf("Error creando excep_arit\n");
		break;
	case 3:
		/* PRUEBA PROVOCAR EXCEPCION MEMORIA */
		if (crear_proceso("excep_mem")<0)
			printf("Error creando excep_mem\n");
		break;
	case 4:
		/* PRUEBA DE LAS NUEVAS LLAMADAS */
		for (i=1; i<=2; i++) {
			if (crear_proceso("yosoy")<0)
				printf("Error creando yosoy\n");
		}
		break;
	case 5:
		/* PRUEBA DE LA LLAMADA DORMIR */
		if (crear_proceso("prueba_dormir")<0)
			printf("Error creando prueba_dormir\n");
		break;
	case 6:
		/* PRIMERA PRUEBA DE ROUND-ROBIN */
		if (crear_proceso("prueba_RR")<0)
			printf("Error creando prueba_RR\n");
		break;
	case 7:
		/* PRUEBA DEL DRIVER DE CDROM */
		if (crear_proceso("mostrar_disco")<0)
			printf("Error creando mostrar_disco\n");
		break;
	case 8:
		/* PRUEBA DEL SISTEMA DE FICHEROS SOLO LECTURA */	
		if (crear_proceso("mostrar_fichero")<0)
			printf("Error creando mostrar_fichero\n");
		break;
	case 9:
		/* PRUEBA DEL DRIVER DE DISCO */
		if (crear_proceso("copiar_disco")<0)
			printf("Error creando copiar_disco\n");
		break;
	case 10:
		/* PRUEBA DEL SISTEMA DE FICHEROS LECTURA/ESCRITURA */	
		if (crear_proceso("copiar_fichero")<0)
			printf("Error creando copiar_fichero\n");
		break;
	case 11:
		if(crear_proceso("dormilon") < 0) {
			printf("Error creando dormilon\n");
		}
		break;
	case 12:
		if(crear_proceso("prueba_alta_prioridad")<0) {
			printf("Error creando prueba alta prioridad\n");
		}
		break;
	case 13:
		if(crear_proceso("prueba_avanzada_dormir") < 0) {
			printf("Error creando prueba avanzada dormir\n");
		}
		break;
	}

    printf("init: fin\n");

	return (0);
}
