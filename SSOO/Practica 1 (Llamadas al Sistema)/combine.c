#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct estructura {
	char nombre[52];
	double peso;
	int referencia;
	int cantidad;
};

/* Declaramos el método combinar. */
int combinar(char* stringBinario1, char* stringBinario2, char* stringBinarioSalida);

int main(int argc, char *argv[]){
	/* Comprobamos que se han pasado cuatro argumentos, si no es así finalizamos el programa e informamos del uso de éste. */
	if(argc != 4) {
		printf("Modo de Empleo: combine <fichero_binario_1> <fichero_binario_2> <fichero_binario_salida>\n");
		return 1;
	}

	/* Llamamos al método que combina los dos fichero binarios. */
	return combinar(argv[1], argv[2], argv[3]);
}

int combinar(char* stringBinario1, char* stringBinario2, char* stringBinarioSalida) {
	/* Declaramos la estructura. */
	struct estructura entrada;

	/* Comprobamos que se pueden abrir y crear los ficheros binarios. */
	int fdBinario1 = open(stringBinario1,O_RDONLY);
	if(!fdBinario1) {
		printf("El fichero binario %s no puede abrirse.\n", stringBinario1);
		return 1;
	}
	int fdBinario2 = open(stringBinario2,O_RDONLY);
	if(!fdBinario2) {
		printf("El fichero binario %s no puede abrirse.\n", stringBinario2);
		return 1;
	}
	int fdBinarioSalida = open(stringBinarioSalida, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU + S_IRWXG + S_IRWXO);
	if(!fdBinarioSalida) {
		printf("El fichero binario %s no puede ser creado.\n", stringBinarioSalida);
		return 1;
	}

	/* Calculamos el tamaño de los binarios de entrada. */
	struct stat st;
	stat(stringBinario1, &st);
	long binarioSize1 = st.st_size;
	stat(stringBinario2, &st);
	long binarioSize2 = st.st_size;

	/* Calculamos el número de entradas que los ficheros binarios contienen. */
	long fileEntries1 = binarioSize1 / (sizeof(entrada.nombre) + sizeof(entrada.peso) + sizeof(entrada.referencia) + sizeof(entrada.cantidad));
	long fileEntries2 = binarioSize2 / (sizeof(entrada.nombre) + sizeof(entrada.peso) + sizeof(entrada.referencia) + sizeof(entrada.cantidad));

	/* Nos colocamos al principio de los ficheros. */
	lseek(fdBinario1, 0, SEEK_SET);
	lseek(fdBinario2, 0, SEEK_SET);

	/* Leemos las entradas del primer binario de entrada y las agregamos al binario de salida. */
	int i;
	for(i = 0; i < fileEntries1; i++) {
		/* Leemos las líneas del fichero con el formato "String(52) + Double(8) + Int(4) + Int(4)". */
		read(fdBinario1, &entrada.nombre, sizeof(entrada.nombre));
		read(fdBinario1, &entrada.peso, sizeof(entrada.peso));
		read(fdBinario1, &entrada.referencia, sizeof(entrada.referencia));
		read(fdBinario1, &entrada.cantidad, sizeof(entrada.cantidad));

		/* Añadimos al fichero binario la estructura leída. */
		write(fdBinarioSalida, &entrada.nombre, sizeof(entrada.nombre));
		write(fdBinarioSalida, &entrada.peso, sizeof(entrada.peso));
		write(fdBinarioSalida, &entrada.referencia, sizeof(entrada.referencia));
		write(fdBinarioSalida, &entrada.cantidad, sizeof(entrada.cantidad));
	}

	/* Leemos las entradas del segundo binario de entrada y las agregamos al binario de salida. */
	for(i = 0; i < fileEntries2; i++) {
		/* Leemos las líneas del fichero con el formato "String(52) + Double(8) + Int(4) + Int(4)". */
		read(fdBinario2, &entrada.nombre, sizeof(entrada.nombre));
		read(fdBinario2, &entrada.peso, sizeof(entrada.peso));
		read(fdBinario2, &entrada.referencia, sizeof(entrada.referencia));
		read(fdBinario2, &entrada.cantidad, sizeof(entrada.cantidad));

		/* Añadimos al fichero binario la estructura leída. */
		write(fdBinarioSalida, &entrada.nombre, sizeof(entrada.nombre));
		write(fdBinarioSalida, &entrada.peso, sizeof(entrada.peso));
		write(fdBinarioSalida, &entrada.referencia, sizeof(entrada.referencia));
		write(fdBinarioSalida, &entrada.cantidad, sizeof(entrada.cantidad));
	}

	/* Cerramos los archivos */
	close(fdBinario1);
	close(fdBinario2);
	close(fdBinarioSalida);

	return 0;
}
