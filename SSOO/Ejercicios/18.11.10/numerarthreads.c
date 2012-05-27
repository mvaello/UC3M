// fichero threads.c
//THREADS
// Realizar un programa que cree 1 thread que suma los valores que se le pasan como parámetro en un array de 10 enteros y cuando termina devuelve el valor calculado.
// compilar con  gcc -lpthread joinconvalor.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define NUMTHREADS 10

void *hilo(int* num);

pthread_attr_t attr;
pthread_mutex_t mtx;
pthread_mutex_attrt attrmtx;
pthread_t idth[NUMTHREADS];

int main(){
	int i;

	pthread_mutex_init(&mtx, &attrtmtx);
	pthread_attr_init(&attr);

	// Creo el thread
	for(i = 0; i < NUMTHREADS; i++) {
		pthread_mutex_lock(&mtx);
		pthread_create(&idth[i], &attr, hilo, &i);
		pthread_mutex_unlock(&mtx);
	}

	// Espero la finalización del thread
	for(i = 0; i < NUMTHREADS; i++) {
		pthread_join(idth[i], NULL);
	}


	return(0);
}

void *hilo(int* num) {
	int id = 0;

	pthread_mutex_lock(&mtx);

	id = (int)* num;

	pthread_mutex_unlock(&mtx);

	printf("\tThread id: %d\n", id);
	pthread_exit(0);
} 
