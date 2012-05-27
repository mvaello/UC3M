#include <stdio.h>
#include <pthread.h>
#define TRUE 1
#define FALSE 0

pthread_attr_t attr;
int ocupado;

void *f( void *arg){
int *v;

//Forma complicada de acceder a los argumentos
int i,j,k;
i=*((int *)(arg));
j=*((int *)(arg)+1);
k=*((int *)(arg)+2);
	printf ("ejecuto el thread\n");
printf ("TH:arg con i,j,k:%d, %d, %d:\n", i,j,k);	

//Forma simple de acceder a los argumentos
v=(int *)arg;
printf ("TH:arg con v:%d, %d, %d:\n", v[0],v[1],v[2]);	
	printf ("fin ejecucion el thread\n");
}
main (){
	int argu[4];
	pthread_t thid;
	argu[0]=99;
	argu[1]=11;
	argu[2]=22;
printf ("M:argu:%d, %d, %d:\n", argu[0],argu[1], argu[2]);	

//f(argu);

        pthread_attr_init (&attr);
	//pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&thid, &attr, f, (void *)argu);
	pthread_join(thid,NULL);
//sleep(2);
}

