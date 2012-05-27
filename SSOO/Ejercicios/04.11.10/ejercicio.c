#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int valoresSumaThreads[10];
void *calcula_suma(void *idthread, int fd);
int fd;

int main(int argc, char *argv[]) {
	/* Comprobamos que se puede abrir el fichero binario. */
	fd = open(argv[1],O_RDONLY);
	if(!fdBinario) {
		printf("El fichero binario %s no puede ser creado.\n", argv[1]);
		return -1;
	}

	int suma = 0;

	pthread_attr_t attr;
	pthread_t thread[10];

	int i;
	
	pthread_attr_init(&attr);

	for (i = 0; i < 10; i++) {
		pthread_create(&thread[i], &attr, calcula_suma, &i, &fdBinario);
		sleep(1);
	}
	for (i = 0; i < 10; i++) {
		pthread_join(thread[i], NULL);
	}
	for (i = 0; i < 10; i++) {
		printf("Valor del thread %d: %d\n", i, valoresSumaThreads[i]);
		suma = suma + valoresSumaThreads[i];
	}

	printf("La suma de todos los números enteros es: %d\n", suma);
}

void *calcula_suma (void *idThread) {

	int i, aux;

	/* Nos colocamos en la posición adecuada del fichero. */
	lseek(fd, sizeof(aux) * (i + (idThread * 100)), SEEK_SET);

	for(i = 0; i < 100; i++) {
		/* Leemos un número entero */
		read(fd, &aux, sizeof(aux));

		/* Sumamos el int leído */
		valoresSumaThreads[idThread] = valoresSumaThreads[idThread] + aux;
	}

}
