/*-
 * main.c
 *
 *   Grupo 83, Grado en Ingeniería Informática. Universidad Carlos III Madrid (2010).
 *
 * 	·Fernández Rojas, Álvaro <100275557@alumnos.uc3m.es>
 * 	·Fuentes Rodríguez, Pablo <100275836@alumnos.uc3m.es>
 *
 * Minishell C source
 * Show how to use "obtain_order" input interface function
 *
 */

/* Incluimos las librerías necesarias para la ejecución de la MiniShell. */

#include <stddef.h>			/* NULL */
#include <stdio.h>			/* setbuf, printf */
#include <stdlib.h>			/* exit */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define REDIRECT_IN	0
#define REDIRECT_OUT	1
#define REDIRECT_ERR	2

extern int obtain_order();		/* See parser.y for description */
/* Definimos la función executeCommand que ejecuta comandos simples con posibilidad de background.*/
int executeCommand(char*** argvv, int bg, char** filev); 
/* Definimos la función redirectOutput con la que manejamos la redirección de ficheros.*/
int redirectOutput(char** filev); 
/* Definimos la función executeCommandPiped que ejecuta más de un comando (con un máximo de 3) con tuberías*/
int executeCommandsPiped(char*** argvv, int argvc, int bg, char** filev);
/* Definimos la función history que nos muestra por pantalla un listado con los últimos 10 (o menos)comandos usados en nuestra MiniShell.*/ 
void history(char ****argvv, int commandsStored, char *filev_history[10][3], int *bg_history);
/* Definimos la función header que muestra por pantalla una portada de programa al principio de su ejecución.*/
void header();
/* Definimos la función argumentos_copy que copia los datos guardados en la estructura argvv. */
void argumentos_copy(char ***argvv, char ****argvv_copia);
/* Definimos la función ficheros_copy que copia los datos guardados en la estructura filev. */
void ficheros_copy(char *filev[3], char *filev_copia[3]);

/* Definición del main, ejecución del programa.*/
int main(void)
{
	/*Definimos las variables necesarias para el desarrollo de la MiniShell*/

	int maxCommands = 10; /* Variable para el control del nº máximo de comandos mostrados con "history" */
	int commandsStored = 0; /* Variable para el control del nº máximo de comandos mostrados con "history". Almacena el nº de comandos almacenados.*/

	char ***argvv;
	char ***argvv_history[maxCommands];
	int argvc;
	char **argv;
	char *filev[3];
	char *filev_history[maxCommands][3];
	int bg;
	int bg_history[10];
	int ret;

	setbuf(stdout, NULL);			/* Unbuffered */
	setbuf(stdin, NULL);

	header(); /* Muestra la portada del programa por pantalla */

	while (1) 
	{
		fprintf(stderr, "%s", "msh> ");	/* Prompt */
		ret = obtain_order(&argvv, filev, &bg);
		if (ret == 0) break;		/* EOF */
		if (ret == -1) continue;	/* Syntax error */
		argvc = ret - 1;		/* Line */
		if (argvc == 0) continue;	/* Empty line */

		/* Condición que comprueba si lo introducido por pantalla coincide con "history" o "!1", "!2", "!3", etc. Si introducimos "history" mostrará los últimos 10 comandos usados. Sin embargo si usamos "!n" Se ejecutará el comando enésimo de la lista del "history", compromabando que "n" no sea mayor que 10 ya que "history" solo almacena 10 comandos, o que no se hayan introducido 10 comandos aún.*/
		if(strcmp(argvv[0][0], "history") != 0 && (argvv[0][0][0] != '!')) {
			/* Si el número de comandos almacenados no ha llegado al máximo permitido, seguimos almacenando en las posiciones sucesivas. */
			if(commandsStored < maxCommands) {
				commandsStored++;
				argumentos_copy(argvv, &argvv_history[commandsStored - 1]);
				bg_history[commandsStored - 1] = bg;
				ficheros_copy(filev, filev_history[commandsStored - 1]);
			}
			/* Si hemos llegado al número máximo de comandos almacenados, seguimos la estructura FIFO, borrando el comando de la posición 1, desplazando el resto de comandos una posición hacia arriba e insertando el nuevo comando en la última posición. */
			else {
				int i = 0;
				for(i = 1; i < maxCommands; i++) {
					argumentos_copy(argvv_history[i], &argvv_history[i - 1]);
					bg_history[commandsStored - 1] = bg_history[commandsStored];
					ficheros_copy(filev_history[i], filev_history[i - 1]);
				}

				argumentos_copy(argvv, &argvv_history[maxCommands - 1]);
				bg_history[maxCommands - 1] = bg;
				ficheros_copy(filev, filev_history[maxCommands - 1]);
			}
		}

		/* Condición para detectar cuando introducimos por teclado un único comando. */
		if(argvc == 1) {	
			/* Detectamos si el comando introducido es "history", y en caso afirmativo lo ejecutamos. */
			if(strcmp(argvv[0][0], "history") == 0) {
				history(argvv_history, commandsStored, filev_history, bg_history);
			}
			/* Detectamos si el comando introducido es de tipo "!n", y en caso afirmativo ejecutamos el comando correspodiente del "history". */
			else if(argvv[0][0][0] == '!') {
				int number = atoi(argvv[0][0] + 1);
				/*Si el numero de comando introducido es superior a los comandos que "history tiene guardados, lógicamente no podemos ejecutar el comando correspondiente a ese número porque no existe. Por ello mostramos un mensaje por pantalla para avisa al usuario de que el comando no se ha encontrado. Al igual, hacemos esto si se introduce un número de comando superior a 10, ya que, como sabemos, "history" almacena como mucho 10 comandos.*/
				if(number <= 0 || number > maxCommands || number > commandsStored) {
					printf("!%d Comando no encontrado.\n", number);
				}
				/* Si el comando se ha encontrado, lo ejecutamos. */
				else {
					/* Averiguamos el valor de argvc, que nos servirá para saber si tiene o no tuberías. */
					for (argvc = 0; (argv = argvv_history[number - 1][argvc]); argvc++);

					/* Distinguimos si es un comando simple o si tiene tuberías */
					if(argvc == 1) {
						executeCommand(argvv_history[number - 1], bg, filev_history[number - 1]);
					}
					else {
						executeCommandsPiped(argvv_history[number - 1], argvc, bg, filev_history[number - 1]);
					}
				}
			}
			/* Si el comando introducido no es "history", ni es de tipo "!n", realizamos la ejecución de comandos simples por defecto. */
			else {
				executeCommand(argvv, bg, filev);
			}
		}
		/* Condición para detectar más de un comando. */
		else {
			executeCommandsPiped(argvv, argvc, bg, filev);
		}
	} //fin while 
	
	exit(0);

	return 0;

} //fin main


/*Ejecución de comandos simples. */
int executeCommand(char*** argvv, int bg, char** filev) {
	int pid = fork(); /* Creamos un hijo */

	switch(pid) {
		case -1:	/* error */
			perror("Error en el fork."); /* Impresión de mensaje de error por pantalla. */
			return -1;
		case 0:		/* hijo */
			redirectOutput(filev);
			if (execvp(argvv[0][0], argvv[0]) < 0) {
				perror("Error en el exec."); /* Impresión de mensaje de error por pantalla. */
				return -1;
			}
			return 0;
		default:	/* padre */
			/* Si no ejecutamos en background, el padre espera a que el hijo termine.*/
			if(!bg) {
				int status;
				while (wait(&status) != pid); /* Padre espera a que el hijo termine. */
			}

			return 0;
	}
}

/* Redirección de ficheros. */
int redirectOutput(char** filev) {
	int result = 0;

	if(filev[0]) {
		int fd = open(filev[0], O_RDONLY, 0600);
		dup2(fd, REDIRECT_IN); /* Redirección con entrada. Duplicamos el descriptor de ficheros y lo cerramos. */
		close(fd); /* Cerramos el descriptor de ficheros. */

		result = 1;
	}
	if(filev[1]) {
		int fd = open(filev[1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
		dup2(fd, REDIRECT_OUT); /* Redirección con salida. Duplicamos el descriptor de ficheros y lo cerramos. */
		close(fd); /* Cerramos el descriptor de ficheros. */

		result = 1;
	}
	if(filev[2]) {
		int fd = open(filev[2], O_CREAT | O_TRUNC | O_WRONLY, 0600);
		dup2(fd, REDIRECT_ERR); /* Redirección con errores. Duplicamos el descriptor de ficheros y lo cerramos. */
		close(fd); /* Cerramos el descriptor de ficheros. */

		result = 1;
	}

	return result;
}

/* Ejecución de comandos con tuberías. */
int executeCommandsPiped(char*** argvv, int argvc, int bg, char** filev) {
	/* Controlamos que el número de comandos para tuberías sean 2 o 3, ya que se pide un máximo de 3 comandos para esta función. */
	if(argvc == 2 || argvc == 3) {
		int fd1[2]; /* Descriptor de ficheros 1. */
		int fd2[2]; /* Descriptor de ficheros 2. */
		int i = 0;
		for(i = 0; i < argvc; i++) {
			/* Si es la primera iteración, creamos una tubería con "fd1"*/
			if(i == 0) {
				pipe(fd1);
			}
			/* Si estamos en la segunda iteración del "for" y si, y solo si, nos encontramos en el caso de tener 3 comandos, creamos otra tubería, en este caso con "fd2". */
			if(i == 1 && argvc == 3) {
				pipe(fd2);
			}

			int pid = fork(); /* Creamos un hijo. */

			switch(pid) {
				case -1: /* Error */
					perror("Error en el fork."); /* Impresión de mensaje de error por pantalla. */
					return -1;
				case 0: /* Hijos */
					if(i == 0) { /* Hijo 1*/
						close(1);

						dup(fd1[1]); /* Duplicamos el descriptor de ficheros. */

						close(fd1[0]); /* Cerramos el descriptor de ficheros. */
						close(fd1[1]); /* Cerramos el descriptor de ficheros. */
					}
					else if(i == 1) { /*Hijo 2*/
						close(0);

						dup(fd1[0]);  /* Duplicamos el descriptor de ficheros. */

						close(fd1[0]); /* Cerramos el descriptor de ficheros. */
						close(fd1[1]); /* Cerramos el descriptor de ficheros. */

						/* Si tenemos 3 argumentos continuamos con los descriptores. */
						if(argvc == 3) {
							close(1);

							dup(fd2[1]); /* Duplicamos el descriptor de ficheros. */

							close(fd2[0]); /* Cerramos el descriptor de ficheros. */
							close(fd2[1]); /* Cerramos el descriptor de ficheros. */
						}

						/*Si tenemos 2 argumentos redirigimos con la función "redirectOutput". */
						if(argvc == 2) {
							redirectOutput(filev);
						}
					}
					else if(i == 2 && argvc == 3) { /*Hijo 3. Si, y solo si hay 3 argumentos.*/
						close(0);

						dup(fd2[0]); /* Duplicamos el descriptor de ficheros. */

						close(fd2[0]); /* Cerramos el descriptor de ficheros. */
						close(fd2[1]); /* Cerramos el descriptor de ficheros. */

						redirectOutput(filev); /* Redirigimos con la función "redirectOutput". */
					}

					if (execvp(argvv[i][0], argvv[i]) < 0) {
						perror("Error en el exec."); /* Impresión de mensaje de error por pantalla. */
						return -1;
					}
				default: /* Padre */
					if(i == 1) {
						close(fd1[0]); /* Cerramos el descriptor de ficheros. */
						close(fd1[1]); /* Cerramos el descriptor de ficheros. */
					}
					else if(i == 2 && argvc == 3) {
						close(fd2[0]); /* Cerramos el descriptor de ficheros. */
						close(fd2[1]); /* Cerramos el descriptor de ficheros. */
					}

					/*Si no estamos en background, el padre espera a que terminen los hijos.*/
					if(!bg) {
						int status;
						while(wait (&status) != pid); /* Padre espera la ejecución de los hijos. */
					}
			}
		}
	}

	/* Si hay más de 3 comandos lanzamos un mensaje de error por pantalla, ya que solo se piden ejecución de comandos con tuberías para un máximo de 3 comandos, por tanto, para 2 o 3.*/
	else {
		perror("No se soportan las tuberías para más de 3 comandos"); /* Impresión de mensaje de error por pantalla. */
		return -1;
	}

	return 0;
}

/* Muestra últimos 10 comandos ejecutados por pantalla*/
void history(char ****argvv, int commandsStored, char *filev_history[10][3], int *bg_history) {
	int i;
	int argc;
	int argvc;
	char **argv;

	for(i = 0; i < commandsStored; i++) {
		printf("%d\t", i + 1);
		for (argvc = 0; (argv = argvv[i][argvc]); argvc++) {
			if(argvc!=0){
				printf("| "); /* Imprimimos "| " antes de cada comando excepto para el primero. En caso de haber solo 1 comando, no se imprime.*/	
			}
			/* Impresión de comandos. */
			for (argc = 0; argv[argc]; argc++) {
				printf("%s ", argv[argc]);
			}
		}
		/* Si el comando tiene redirección lo mostramos por pantalla. */
		if(filev_history[i][0] != NULL) {
			printf("< %s", filev_history[i][0]);
		}
		if(filev_history[i][1] != NULL) {
			printf("> %s", filev_history[i][1]);
		}
		if(filev_history[i][2] != NULL) {
			printf(">& %s", filev_history[i][2]);
		}
		/* Si el comando está en background lo señalamos con &. */
		if(bg_history[i]) {
			printf("&");
		}
		printf("\n");
	}
}

/* Copia los datos guardados en la estructura argvv. */
void argumentos_copy(char ***argvv, char ****argvv_copia){
 char ***aux;
 char **argv;
 int argc, argvc;

for (argvc = 0; (argv = argvv[argvc]); argvc++);
 if (argvc == 0)
 return;
 aux = (char ***)malloc(argvc*sizeof(char**)+1);
 for (argvc = 0; (argv = argvv[argvc]); argvc++) {
 for (argc = 0; argv[argc]; argc++);
 aux[argvc] = (char **)malloc(argc*sizeof(char*)+1);
 }
 for (argvc = 0; (argv = argvv[argvc]); argvc++) {
 for (argc = 0; argv[argc]; argc++) {
 aux[argvc][argc] = NULL;
 if (argv[argc]){
 aux[argvc][argc] = (char *)malloc(strlen(argv[argc])+1);
 strcpy(aux[argvc][argc],argv[argc]);
 }
 }
 aux[argvc][argc] = NULL;
 }
 aux[argvc] = NULL;
 *argvv_copia = aux;
 /*
 for (argvc = 0; (argv = aux[argvc]); argvc++) {
 for (argc = 0; argv[argc]; argc++) {
 printf("copia: [%d][%d] -> %s
 (%p)\n",argvc,argc,argv[argc],argv[argc]);
 }
 printf("argvv[%d][%d] -> NULL\n",argvc,argc);
 printf("\n");
 }
 printf("argvv[%d] -> NULL\n",argvc);
 */
}

/* Copia los datos guardados en la estructura filev. */
void ficheros_copy(char *filev[3], char *filev_copia[3]){
	int i;
	for (i = 0; i < 3; i++) {
		filev_copia[i] = NULL;
		if(filev[i]) {
			filev_copia[i] = (char *)malloc(strlen(filev[i])+1);
			strcpy(filev_copia[i], filev[i]);
		}
	}
}

/* Portada de la Minishell con el nombre de los Autores. */
void header(){
	printf("\n");
	printf(" __________________________________________________________\n");
	printf("|                                                          |\n");
	printf("|   **              **                                     |\n");
	printf("|    **            **                                      |\n");
	printf("|     **    **    **    ***  *    ***  ***  ** **  ***     |\n");
	printf("|      ** **  ** **     **   *    *    * *  * * *  **      |\n");
	printf("|       **      **      ***  ***  ***  ***  *   *  ***     |\n");
	printf("|                                                          |\n");
	printf("|                                                          |\n");
	printf("|                          ··· ···                         |\n");
	printf("|                           ·  · ·                         |\n");
	printf("|                           ·  ···                         |\n");
	printf("|                                                          |\n");
	printf("|                      UC3M's miniSHELL                    |\n");
	printf("|                                                          |\n");
	printf("|                                                          |\n");
	printf("| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |\n");
	printf("| Authors:                                                 |\n");
	printf("|    · Fernández Rojas, Álvaro                             |\n");
	printf("|    · Fuentes Rodríguez, Pablo                            |\n");
	printf("|__________________________________________________________|\n");
	printf("\n");
}
