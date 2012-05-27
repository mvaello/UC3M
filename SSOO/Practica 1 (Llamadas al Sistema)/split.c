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
int dividir(char* stringBinarioEntrada, char* stringBinario1, char* stringBinario2);

int main(int argc, char *argv[]){
	/* Comprobamos que se han pasado cuatro argumentos, si no es así finalizamos el programa e informamos del uso de éste. */
	if(argc != 4) {
		printf("Modo de Empleo: split <fichero_binario_entrada> <fichero_binario_1> <fichero_binario_2>\n");
		return 1;
	}

	/* Llamamos al método que combina los dos fichero binarios. */
	return dividir(argv[1], argv[2], argv[3]);
}

int dividir(char* stringBinarioEntrada, char* stringBinario1, char* stringBinario2) {
	/* Declaramos la estructura. */
	struct estructura entrada;

	/* Comprobamos que se pueden abrir y crear los ficheros binarios. */
	int fdBinarioEntrada = open(stringBinarioEntrada,O_RDONLY);
	if(!fdBinarioEntrada) {
		printf("El fichero binario %s no puede abrirse.\n", stringBinarioEntrada);
		return 1;
	}
	int fdBinario1 = open(stringBinario1, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU + S_IRWXG + S_IRWXO);
	if(!fdBinario1) {
		printf("El fichero binario %s no puede ser creado.\n", stringBinario1);
		return 1;
	}
	int fdBinario2 = open(stringBinario2, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU + S_IRWXG + S_IRWXO);
	if(!fdBinario2) {
		printf("El fichero binario %s no puede ser creado.\n", stringBinario2);
		return 1;
	}

	/* Calculamos el tamaño del binario de entrada. */
	struct stat st;
	stat(stringBinarioEntrada, &st);
	long binarioSize = st.st_size;

	/* Calculamos el número de entradas que el fichero binario contiene. */
	long fileEntries = binarioSize / (sizeof(entrada.nombre) + sizeof(entrada.peso) + sizeof(entrada.referencia) + sizeof(entrada.cantidad));

	/*
		A B C D E F G	-> volante | asiento | rueda | llanta | puerta | radiador | bateria
		G F E		-> bateria | radiador | puerta
		D C B A		-> llanta | rueda | asiento | volante
	*/
	/* Leemos las entradas del binario de entrada y las agregamos a los binarios de salida. */
	int i;
	for(i = fileEntries - 1; i >= 0; i--) {
		/* Nos colocamos en la posición adecuada del fichero. */
		lseek(fdBinarioEntrada, (sizeof(entrada.nombre) + sizeof(entrada.peso) + sizeof(entrada.referencia) + sizeof(entrada.cantidad)) * i, SEEK_SET);

		/* Leemos las líneas del fichero con el formato "String(52) + Double(8) + Int(4) + Int(4)". */
		read(fdBinarioEntrada, &entrada.nombre, sizeof(entrada.nombre));
		read(fdBinarioEntrada, &entrada.peso, sizeof(entrada.peso));
		read(fdBinarioEntrada, &entrada.referencia, sizeof(entrada.referencia));
		read(fdBinarioEntrada, &entrada.cantidad, sizeof(entrada.cantidad));

		/* Imprimimos por pantalla la estructura leída. */
		printf("%s\t%g\t%d\t%d\n", entrada.nombre, entrada.peso, entrada.referencia, entrada.cantidad);

		/* Añadimos empezando por la última entrada y retrocediendo al primer fichero, hasta llegar a la mitad, y añadimos al primer fichero el resto, hasta llegar 			a la primera entrada. */
		if(i > ((fileEntries / 2))) {
			/* Añadimos al fichero binario la estructura leída. */
			write(fdBinario1, &entrada.nombre, sizeof(entrada.nombre));
			write(fdBinario1, &entrada.peso, sizeof(entrada.peso));
			write(fdBinario1, &entrada.referencia, sizeof(entrada.referencia));
			write(fdBinario1, &entrada.cantidad, sizeof(entrada.cantidad));
		}
		else {
			/* Añadimos al fichero binario la estructura leída. */
			write(fdBinario2, &entrada.nombre, sizeof(entrada.nombre));
			write(fdBinario2, &entrada.peso, sizeof(entrada.peso));
			write(fdBinario2, &entrada.referencia, sizeof(entrada.referencia));
			write(fdBinario2, &entrada.cantidad, sizeof(entrada.cantidad));
		}
	}


	/* Cerramos los archivos */
	close(fdBinarioEntrada);
	close(fdBinario1);
	close(fdBinario2);

	return 0;
}
