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
	
printf("\
1.- Prueba simple\n\
2.- Excepcion aritmetica\n\
3.- Excepcion memoria\n\
4.- Nuevas llamadas\n\
5.- Dormir procesos\n\
6.- Planificacion Round-Robin\n\
7.- Mostrar disco\n\
8.- Mostrar fichero\n\
9.- Copiar disco\n\
10.-Copiar fichero\n\
11.-Dormilon\n\
12.-Alta prioridad\n\
13.-Prueba avanzada dormir\n");

/*
	DISCO
*/
printf("\
*************\n\
*** DISCO ***\n\
*************\n\
14.-Leer disco\n\
15.-Escribir disco\n\
16.-Leer disco avanzada\n\
17.-Escribir disco avanzada\n\
18.-Leer bloque disco avanzada\n\
19.-Escribir bloque disco avanzada\n\
*** Correcion ***\n\
20.-test_e2_1_abrir_discoo\n\
21.-test_e2_2_leer_bloque\n\
22.-test_e2_3_leer_disco\n\
23.-test_e2_4_escribir_bloque\n\
24.-test_e2_5_escribir_disco\n\
25.-test_e2_6_leer_varios_disco\n\
26.-test_e2_7_leer_varios_bloque\n\
27.-test_e2_8_escribir_varios_disco\n\
28.-test_e2_9_escribir_varios_bloque\n");

/*
	DISCO_FS
*/
printf("\
*************\n\
**** F.S ****\n\
*************\n\
29.-montar_vol\n\
30.-abrir_fichero\n\
31.-leer_fichero\n\
32.-escribir_fichero\n\
33.-sincronizar_vol\n\
34.-lectura_escritura_varios_tamanyos\n\
35.-escribir_fichero_mul\n\
36.-leer_fichero_mul\n\
37.-escribir_fichero_mul_varfic\n\
38.-leer_fichero_mul_varfic\n\
39.-abrir_fichero_exista_noexista\n");

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

	/*
		DISCO
	*/
	case 14:
		if(crear_proceso("leer_disco") < 0) {
			printf("Error creando prueba leer disco\n");
		}
		break;
	case 15:
		if(crear_proceso("escribir_disco") < 0) {
			printf("Error creando prueba escribir disco\n");
		}
		break;
	case 16:
		for(i = 0; i < 9; i++) {
			if(crear_proceso("leer_disco") < 0) {
				printf("Error creando prueba escribir disco\n");
			}
		}
		break;
	case 17:
		for(i = 0; i < 9; i++) {
			if(crear_proceso("escribir_disco") < 0) {
				printf("Error creando prueba escribir disco\n");
			}
		}
		break;
	case 18:
		for(i = 0; i < 9; i++) {
			if(crear_proceso("leer_disco_bloque") < 0) {
				printf("Error creando prueba escribir disco\n");
			}
		}
		break;
	case 19:
		for(i = 0; i < 9; i++) {
			if(crear_proceso("escribir_disco_bloque") < 0) {
				printf("Error creando prueba escribir disco\n");
			}
		}
		break;
	//Correcion disco.
	case 20:
		if(crear_proceso("test_e2_1_abrir_disco") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 21:
		if(crear_proceso("test_e2_2_leer_bloque") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 22:
		if(crear_proceso("test_e2_3_leer_disco") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 23:
		if(crear_proceso("test_e2_4_escribir_bloque") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 24:
		if(crear_proceso("test_e2_5_escribir_disco") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 25:
		if(crear_proceso("test_e2_6_leer_varios_disco") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 26:
		if(crear_proceso("test_e2_7_leer_varios_bloque") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 27:
		if(crear_proceso("test_e2_8_escribir_varios_disco") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 28:
		if(crear_proceso("test_e2_9_escribir_varios_bloque") < 0) {
			printf("Error creando prueba\n");
		}
		break;

	/*
		DISCO_FS
	*/
	case 29:
		if(crear_proceso("montar_vol") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 30:
		if(crear_proceso("abrir_fichero") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 31:
		if(crear_proceso("leer_fichero") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 32:
		if(crear_proceso("escribir_fichero") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 33:
		if(crear_proceso("sincronizar_vol") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 34:
		if(crear_proceso("lectura_escritura_varios_tamanyos") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 35:
		if(crear_proceso("escribir_fichero_mul") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 36:
		if(crear_proceso("leer_fichero_mul") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 37:
		if(crear_proceso("escribir_fichero_mul_varfic") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 38:
		if(crear_proceso("leer_fichero_mul_varfic") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	case 39:
		if(crear_proceso("abrir_fichero_exista_noexista") < 0) {
			printf("Error creando prueba\n");
		}
		break;
	}

    printf("init: fin\n");

	return (0);
}
