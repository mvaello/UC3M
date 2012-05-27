/**
	SISTEMAS DISTRIBUIDOS :: UC3M
	CADENA DE MONTAJE (EJERCICIO DE COLAS DE MENSAJES)
	@author: Álvaro Fernández Rojas
*/

#ifndef CADENA_MONTAJE_H
#define CADENA_MONTAJE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <ctype.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0

#define NO_ERROR 0

#define ERROR_ARGS -1
#define ERROR_FDESC_IN -2
#define ERROR_FDESC_OUT -3
#define ERROR_FDIN_EXISTS -4
#define ERROR_FDIN_SIZE -5
#define ERROR_FORK -6
#define ERROR_MQ_DESC -7
#define ERROR_MQ_SEND -8
#define ERROR_MQ_RECEIVE -9
#define ERROR_FILE_WRITE -10

#define QUEUE_1 "/cola1"
#define QUEUE_2 "/cola2"

#define ITER_BYTES_SIZE sizeof(int)

#define FILE_BLOCK 4096
#define QUEUE_MAX_MSGS 10

int cadena_montaje(int fdin, int fdout);

int funcion_proceso1(int fdin);
int funcion_proceso2();
int funcion_proceso3(int fdout);

#endif
