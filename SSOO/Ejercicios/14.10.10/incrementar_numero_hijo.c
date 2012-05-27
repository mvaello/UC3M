/*
	Este programa incrementa el número pasado por teclado, de manera ininterrumpida hasta que se interrumpe el programa manualmente.
*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int end = 0;

void timeEnded(int s) {
	end = 1;
}
void childEnd(int s) {
	printf("FIN POR ORDEN.");
}

int main() {
	/* Creamos la variable que almacena el número pasado por teclado y el número del pid. */
	int number, pid = 0;

	/* El programa debe finalizar a los 60 segundos de haberse iniciado. */
	signal(SIGALARM, timeEnded);
	alarm(60);

	while(!end) {
		/* Leemos un número por teclado. */
		printf("Escriba un número: ");
		scanf("%d", &number);

		/* En la primera iteración del bucle no queremos matar al hijo, en el resto sí. */
		if(pid != 0) {
			kill(pid, SIGUSR1);
			while(wait(NULL) != pid); /* Esperamos a que el hijo se muera y liberamos recursos. */
		}

		/* Creamos un hijo con el bucle que incrementa infinitas veces el número pasado por teclado. */
		if((pid = fork()) == 0) {
			signal(SIGUSR1, childEnd);
			while(1) {
				printf("%d\n", number);
				number++;
			}
		}
	}

	while(wait(NULL) != pid); /* Esperamos a que el hijo se muera y liberamos recursos. */

	return 0;
}
