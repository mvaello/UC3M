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

/* Declaramos el método convertir. */
int convertir(char* stringBinario, char* stringTexto);

int main(int argc, char *argv[]){
	/* Comprobamos que se han pasado tres argumentos, si no es así finalizamos el programa e informamos del uso de éste. */
	if(argc != 3) {
		printf("Modo de Empleo: bin2text <fichero_binario> <fichero_texto>\n");
		return 1;
	}

	/* Llamamos al método que convierte el fichero de texto a binario. */
	return convertir(argv[1], argv[2]);
}

int convertir(char* stringBinario, char* stringTexto) {
	/* Declaramos la estructura. */
	struct estructura entrada;

	/* Comprobamos que se puede abrir el fichero binario. */
	int fdBinario = open(stringBinario,O_RDONLY);
	if(!fdBinario) {
		printf("El fichero binario %s no puede ser creado.\n", stringBinario);
		return 1;
	}

	/* Comprobamos que se puede crear el fichero de texto. */
	FILE* ficheroTexto = fopen(stringTexto,"w");
	if(!ficheroTexto) {
		printf("Error al abrir el fichero de texto: %s.\n", stringTexto);
		return 1;
	}

	/* Calculamos el tamaño del binario de entrada. */
	struct stat st;
	stat(stringBinario, &st);
	long binarioSize = st.st_size;

	/* Calculamos el número de entradas que el fichero binario contiene. */
	long fileEntries = binarioSize / (sizeof(entrada.nombre) + sizeof(entrada.peso) + sizeof(entrada.referencia) + sizeof(entrada.cantidad));

	/* Nos colocamos al principio del fichero. */
	lseek(fdBinario, 0, SEEK_SET);

	/* Leemos las entradas del binario de entrada y las escribimos al fichero de texto. */
	int i;
	for(i = 0; i < fileEntries; i++) {
		/* Leemos las líneas del fichero con el formato "String(52) + Double(8) + Int(4) + Int(4)". */
		read(fdBinario, &entrada.nombre, sizeof(entrada.nombre));
		read(fdBinario, &entrada.peso, sizeof(entrada.peso));
		read(fdBinario, &entrada.referencia, sizeof(entrada.referencia));
		read(fdBinario, &entrada.cantidad, sizeof(entrada.cantidad));

		/* Imprimimos al fichero la estructura leída. */
		fprintf(ficheroTexto, "%s\t%g\t%d\t%d\n", entrada.nombre, entrada.peso, entrada.referencia, entrada.cantidad);

		/* Imprimimos por pantalla la estructura leída. */
		printf("%s\t%g\t%d\t%d\n", entrada.nombre, entrada.peso, entrada.referencia, entrada.cantidad);
	}

	/* Cerramos los archivos */
	close(fdBinario);
	fclose(ficheroTexto);

	return 0;
}
