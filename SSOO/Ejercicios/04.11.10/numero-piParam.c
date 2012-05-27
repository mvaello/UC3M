/* programa que crea 10 threads.
 * Cada uno de ellos calcula el valor del número PI y lo almacena  en la posición que le corresponde de en un array.
 * Cuando han terminado todos los threads el programa principal calcula la media de los valores de pi almacenados en el array
 * compilar con gcc -lpthread -lm numero_piParam.c 
 */
//José Manuel Pérez Lobato
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define RADIO 5000
#define PUNTOS 1000000

float valoresPIthreads[10];
void *calcula_pi (void *kk);

main()
{
	pthread_attr_t attr;
	pthread_t thread[10];
	int i;
	float *valorpi=0, suma=0, media=0;
	
	pthread_attr_init(&attr);
	for (i=0;i<10;i++) {
		pthread_create(&thread[i],&attr,calcula_pi,&i);
		sleep(1);
	}
	for (i=0;i<10;i++) {
		pthread_join(thread[i],NULL);
	}
	for (i=0;i<10;i++) {
		printf("Valor del thread %d: %f\n",i,valoresPIthreads[i]);
		suma=suma+valoresPIthreads[i];
	}
	media=suma/10.0;
	printf("El valor medio de Pi obtenido es: %f\n",media);
}

void *calcula_pi (void *idthread)
{
	int j, y=0, x=0, cont=0,*numthread;
	float pi=0, h=0;
	numthread = idthread;
	srandom(pthread_self());
	for (j=0;j<PUNTOS;j++)
	{
		y=(random()%((2*RADIO)+1)-RADIO);
		x=(random()%((2*RADIO)+1)-RADIO);
		h=sqrt((x*x)+(y*y));
		if ( h<=RADIO ) cont++;
	}
	numthread=idthread;
	valoresPIthreads[*numthread]=(cont*4)/(float)PUNTOS;
	pthread_exit(&pi);
}

