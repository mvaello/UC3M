/**
	SISTEMAS DISTRIBUIDOS :: UC3M
	CADENA DE MONTAJE (EJERCICIO DE COLAS DE MENSAJES)
	@author: Álvaro Fernández Rojas
*/

#include "cadena_montaje.h"

int main(int argc, char** argv) {
	//Comprobación de argumentos de ejecución.
	if(argc != 3) {
		printf("Modo de Empleo: %s <fichero_entrada> <fichero_salida>\n", argv[0]);
		return ERROR_ARGS;
	}

	//Descriptores de fichero.
	int fdin = open(argv[1], O_RDONLY, S_IRWXU + S_IRWXG + S_IRWXO);
	int fdout = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, S_IRWXU + S_IRWXG + S_IRWXO);

	//Comprobar errores al obtener los descriptores de fichero.
	if(!fdin) {
		perror("[ERROR] Error al crear el descriptor del fichero de entrada.\n");
		return ERROR_FDESC_IN;
	}
	if(!fdout) {
		perror("[ERROR] Error al crear el descriptor del fichero de salida.\n");
		return ERROR_FDESC_OUT;
	}

	//Comprobamos si existe el fichero de entrada y si está vacío.
	struct stat st;
	if(fstat(fdin, &st) == -1) {
		perror("[ERROR] No existe el fichero de entrada.\n");
		return ERROR_FDIN_EXISTS;
	}
	long fdin_size = st.st_size;
	if(fdin_size == 0) {
		perror("[ERROR] El fichero de entrada está vacío.\n");
		return ERROR_FDIN_SIZE;
	}

	//Ejecutamos la función del padre de la cadena de montaje.
	return cadena_montaje(fdin, fdout);
}

int cadena_montaje(int fdin, int fdout) {
	//Creamos un pid_t para cada proceso hijo (3).
	//Progresivamente, en las condiciones ejecutadas por el padre iremos creando hijos (3).
	//Cada vez que creamos un hijo, este realiza su función y termina.
	//Una vez creados todos los hijos el padre esperará a que vayan terminando, para después cerrar los ficheros y terminar.

	//Informamos que se ha iniciado correctamente el programa.
	printf("[CADENA_MONTAJE] Iniciado.\n");

	//Padre crear el primer hijo.
	pid_t pid[3];
	pid[0] = fork();
	if(pid[0] == -1) {
		perror("[ERROR] Error en el fork 1.\n");
		return ERROR_FORK;
	}
	else if(pid[0] == 0) {
		//Hijo 1 ejecuta su función.
		printf("[PROCESO1] Creado.\n");

		return funcion_proceso1(fdin);
	}
	else {
		//Padre crea el segundo hijo.
		pid[1] = fork();
		if(pid[1] == -1) {
			perror("[ERROR] Error en el fork 2.\n");
			return ERROR_FORK;
		}
		else if(pid[1] == 0) {
			//Hijo 2 ejecuta su función.
			printf("[PROCESO2] Creado.\n");

			return funcion_proceso2();
		}
		else {
			//Padre crear el tercer hijo.
			pid[2] = fork();
			if(pid[2] == -1) {
				perror("[ERROR] Error en el fork 3.\n");
				return ERROR_FORK;
			}
			else if(pid[2] == 0) {
				//Hijo 3 ejecuta su función.
				printf("[PROCESO3] Creado.\n");

				return funcion_proceso3(fdout);
			}
			else {
				//El padre realiza una espera pasiva de los hijos.
				int i;
				int status[3];
				for(i = 0; i < 3; i++) {
					waitpid(pid[i], &status[i], 0);
				}

				//Mostramos la finalización del programa.
				printf("[CADENA_MONTAJE] Finalizado.\n");

				//Cerramos los descriptores de fichero.
				close(fdin);
				close(fdout);

				return NO_ERROR;
			}
		}
	}
}

int funcion_proceso1(int fdin) {
	//Abrimos el descriptor de la primera cola, modificando sus atributos a los deseados en la práctica (bloques de 4KB).
	mqd_t mqdout;
	struct mq_attr attr;
	attr.mq_maxmsg = QUEUE_MAX_MSGS;
	attr.mq_msgsize = FILE_BLOCK;
	if((mqdout = mq_open(QUEUE_1, O_CREAT | O_WRONLY, 0777, &attr)) == -1) {
		mq_close(mqdout);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		perror("[ERROR][PROCESO1] Error abriendo el descriptor de la cola 1.\n");
		return ERROR_MQ_DESC;
	}

	//Calculamos el número de bloques de 4KB que hay en el fichero de entrada (NÚMERO DE ITERACIONES),
	struct stat st;
	if(fstat(fdin, &st) == -1) {
		perror("[ERROR][PROCESO1] No existe el fichero de entrada.\n");
		mq_close(mqdout);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		return ERROR_FDIN_EXISTS;
	}
	long fdin_size = st.st_size;
	int iter = (fdin_size/FILE_BLOCK) + 1;
	printf("[PROCESO1] Número de iteraciones: %d.\n", iter);

	//Convertimos el entero que contiene el número de iteraciones a un array de char y lo enviamos por la cola como primer mensaje.
	//De esta forma, el segundo proceso sabrá el número de veces que ha de leer de la cola de mensajes.
	char iter_bytes[ITER_BYTES_SIZE];
	iter_bytes[0] = (iter & 0xFF);
	iter_bytes[1] = ((iter >> 8) & 0xFF);
	iter_bytes[2] = ((iter >> 16) & 0xFF);
	iter_bytes[3] = ((iter >> 24) & 0xFF);
	if(mq_send(mqdout, iter_bytes, ITER_BYTES_SIZE, 0) == -1) {
		perror("[ERROR][PROCESO1] Error al escribir en la cola 1.\n");
		mq_close(mqdout);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		return ERROR_MQ_SEND;
	}

	//Reservamos un array de bytes con el tamaño máximo de bloque que queremos leer del fichero.
	//Para el número de iteraciones ya calculado, vamos leyendo bloques de un máximo de 4KB del fichero de entrada y escribiéndolos en la cola de mensajes.
	char bytes[FILE_BLOCK];
	int i;
	for(i = 0; i < iter; i++) {
		//Leemos del fichero de entrada un bloque de máximo 4KB, guardando el número de bytes realmente leídos (el fichero puede no tener bloques exactos de 4KB).
		ssize_t bytes_read = read(fdin, &bytes, FILE_BLOCK);

		printf("[PROCESO1] Leídos %d bytes del fichero de origen. [i=%d]\n", bytes_read, i);

		//Mandamos los bytes leídos a la primera cola de mensajes.
		if(mq_send(mqdout, bytes, bytes_read, 0) == -1) {
			perror("[ERROR][PROCESO1] Error al escribir en la cola 1.\n");
			mq_close(mqdout);
			mq_unlink(QUEUE_1);
			mq_unlink(QUEUE_2);
			return ERROR_MQ_SEND;
		}

		printf("[PROCESO1] Escritos %d bytes en la cola 1 [i=%d].\n", bytes_read, i);
	}

	//Cerramos el descriptor de fichero de la primera cola.
	mq_close(mqdout);

	//Finalización del primer proceso.
	printf("[PROCESO1] Finalizado.\n");

	return NO_ERROR;
}

int funcion_proceso2() {
	//Abrimos el descriptor de ambas colas, siendo la primera en modo lectura y la segunda en modo escritura.
	mqd_t mqdin, mqdout;
	struct mq_attr attrin;
	attrin.mq_maxmsg = QUEUE_MAX_MSGS;
	attrin.mq_msgsize = FILE_BLOCK;
	if((mqdin = mq_open(QUEUE_1, O_CREAT | O_RDONLY, 0777, &attrin)) == -1) {
		perror("[ERROR][PROCESO2] Error abriendo el descriptor de la cola 1.\n");
		return ERROR_MQ_DESC;
	}
	struct mq_attr attrout;
	attrout.mq_maxmsg = QUEUE_MAX_MSGS;
	attrout.mq_msgsize = FILE_BLOCK;
	if((mqdout = mq_open(QUEUE_2, O_CREAT | O_WRONLY, 0777, &attrout)) == -1) {
		perror("[ERROR][PROCESO2] Error abriendo el descriptor de la cola 2.\n");
		return ERROR_MQ_DESC;
	}

	//Lo primero que leemos de la cola será el número de bloques de 4KB que debemos leer de la primera cola, transformar y escribir en la segunda.
	char iter_bytes_read[ITER_BYTES_SIZE];
	if(mq_receive(mqdin, iter_bytes_read, attrin.mq_msgsize, 0) == -1) {
		perror("[ERROR][PROCESO2] Error recibiendo datos de la cola 1.\n");
		mq_close(mqdin);
		mq_close(mqdout);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		return ERROR_MQ_RECEIVE;
	}
	//Convertimos el número de iteraciones de un array de char a un entero.
	int iter = ((iter_bytes_read[0] & 0xFF) | ((iter_bytes_read[1] << 8) & 0xFF00) | ((iter_bytes_read[2] << 16) & 0xFF0000) | ((iter_bytes_read[3] << 8) & 0xFF000000));
	printf("[PROCESO2] Número de iteraciones: %d.\n", iter);

	//Mandamos el número de iteraciones a la segunda cola de mensajes cómo primer mensajes, para que el tercer proceso sepa el número de bloques de 4KB que debe leer de la cola 2.
	//Convertimos el número de iteraciones de un entero a un array de caracteres (bytes).
	char iter_bytes_write[ITER_BYTES_SIZE];
	iter_bytes_write[0] = (iter & 0xFF);
	iter_bytes_write[1] = ((iter >> 8) & 0xFF);
	iter_bytes_write[2] = ((iter >> 16) & 0xFF);
	iter_bytes_write[3] = ((iter >> 24) & 0xFF);
	if(mq_send(mqdout, iter_bytes_write, ITER_BYTES_SIZE, 0) == -1) {
		perror("[ERROR][PROCESO2] Error al escribir en la cola 2.\n");
		mq_close(mqdin);
		mq_close(mqdout);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		return ERROR_MQ_SEND;
	}

	//Reservamos un array de bytes con el tamaño máximo de bloque que queremos leer de la primera cola de mensajes.
	//Para el número de iteraciones ya calculado, vamos leyendo bloques de un máximo de 4KB de la cola 1 y escribiéndolos en la cola 2.
	char bytes[FILE_BLOCK];
	ssize_t bytes_read;
	int i;
	for(i = 0; i < iter; i++) {
		//Leemos de la primera cola un bloque de máximo 4KB, guardando el número de bytes realmente leídos (la cola puede no tener bloques exactos de 4KB).
		if((bytes_read = mq_receive(mqdin, bytes, attrin.mq_msgsize, 0)) == -1) {
			perror("[ERROR][PROCESO2] Error recibiendo datos de la cola 1.\n");
			mq_close(mqdin);
			mq_close(mqdout);
			mq_unlink(QUEUE_1);
			mq_unlink(QUEUE_2);
			return ERROR_MQ_RECEIVE;
		}

		printf("[PROCESO2] Leídos %d bytes de la cola 1 [i=%d].\n", bytes_read, i);

		//Convertimos las mayúsculas a minúsculas con la función tolower().
		int j;
		for(j = 0; j < FILE_BLOCK; j++) {
			bytes[j] = tolower(bytes[j]);
		}

		//Mandamos los bytes leídos a la segunda cola de mensajes.
		if(mq_send(mqdout, bytes, bytes_read, 0) == -1) {
			perror("[ERROR][PROCESO2] Error al escribir en la cola 2.\n");
			mq_close(mqdin);
			mq_close(mqdout);
			mq_unlink(QUEUE_1);
			mq_unlink(QUEUE_2);
			return ERROR_MQ_SEND;
		}

		printf("[PROCESO2] Escritos %d bytes en la cola 2 [i=%d].\n", bytes_read, i);
	}

	//Cerramos el descriptor de fichero de la primera cola.
	mq_close(mqdin);
	mq_close(mqdout);
	mq_unlink(QUEUE_1);

	//Finalización del segundo proceso.
	printf("[PROCESO2] Finalizado.\n");

	return NO_ERROR;
}

int funcion_proceso3(int fdout) {
	//Abrimos el descriptor de la segundo cola como lectura.
	mqd_t mqdin;
	struct mq_attr attrin;
	attrin.mq_maxmsg = QUEUE_MAX_MSGS;
	attrin.mq_msgsize = FILE_BLOCK;
	if((mqdin = mq_open(QUEUE_2, O_CREAT | O_RDONLY, 0777, &attrin)) == -1) {
		perror("[ERROR][PROCESO3] Error abriendo el descriptor de la cola 2.\n");
		mq_close(mqdin);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		return ERROR_MQ_DESC;
	}

	//Lo primero que leemos de la cola será el número de bloques de 4KB que debemos leer de la segunda cola y escribir en el fichero de salida.
	char iter_bytes[ITER_BYTES_SIZE];
	if(mq_receive(mqdin, iter_bytes, attrin.mq_msgsize, 0) == -1) {
		perror("[ERROR][PROCESO3] Error recibiendo datos de la cola 2.\n");
		mq_close(mqdin);
		mq_unlink(QUEUE_1);
		mq_unlink(QUEUE_2);
		return ERROR_MQ_RECEIVE;
	}

	//Convertimos el número de iteraciones de un array de char a un entero.
	int iter = ((iter_bytes[0] & 0xFF) | ((iter_bytes[1] << 8) & 0xFF00) | ((iter_bytes[2] << 16) & 0xFF0000) | ((iter_bytes[3] << 8) & 0xFF000000));
	printf("[PROCESO3] Número de iteraciones: %d.\n", iter);

	//Reservamos un array de bytes con el tamaño máximo de bloque que queremos leer de la segunda cola de mensajes.
	//Para el número de iteraciones ya calculado, vamos leyendo bloques de un máximo de 4KB de la cola 2 y escribiéndolos en el fichero de salida.
	char bytes[FILE_BLOCK];
	ssize_t bytes_read, bytes_written;
	int i;
	for(i = 0; i < iter; i++) {
		//Leemos de la segunda cola un bloque de máximo 4KB, guardando el número de bytes realmente leídos (la cola puede no tener bloques exactos de 4KB).
		if((bytes_read = mq_receive(mqdin, bytes, attrin.mq_msgsize, 0)) == -1) {
			perror("[ERROR][PROCESO3] Error recibiendo datos de la cola 2.\n");
			mq_close(mqdin);
			mq_unlink(QUEUE_1);
			mq_unlink(QUEUE_2);
			return ERROR_MQ_RECEIVE;
		}

		printf("[PROCESO3] Leídos %d bytes de la cola 2 [i=%d].\n", bytes_read, i);

		//Mandamos los bytes leídos al fichero de salida.
		bytes_written = write(fdout, bytes, bytes_read);
		if(bytes_written == -1) {
			perror("[ERROR][PROCESO3] Error escribiendo datos en el fichero de salida.\n");
			mq_close(mqdin);
			mq_unlink(QUEUE_1);
			mq_unlink(QUEUE_2);
			return ERROR_FILE_WRITE;
		}

		printf("[PROCESO3] Escritos %d bytes en el fichero de salida [i=%d].\n", bytes_written, i);
	}

	//Finalización del segundo proceso.
	mq_close(mqdin);
	mq_unlink(QUEUE_2);

	//Finalización del tercer proceso.
	printf("[PROCESO3] Finalizado.\n");

	return NO_ERROR;
}

