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
int convertir(char* stringTexto, char* stringBinario);

int main(int argc, char *argv[]){
	/* Comprobamos que se han pasado tres argumentos, si no es así finalizamos el programa e informamos del uso de éste. */
	if(argc != 3) {
		printf("Modo de Empleo: text2bin <fichero_texto> <fichero_binario>\n");
		return 1;
	}

	/* Llamamos al método que convierte el fichero de texto a binario. */
	return convertir(argv[1], argv[2]);
}

int convertir(char* stringTexto, char* stringBinario) {
	/* Declaramos la estructura. */
	struct estructura entrada;

	/* Comprobamos que se puede abrir el fichero de texto. */
	FILE* ficheroTexto = fopen(stringTexto,"r");
	if(!ficheroTexto) {
		printf("Error al abrir el fichero de texto: %s.\n", stringTexto);
		return 1;
	}

	/* Comprobamos que se puede abrir el fichero binario. */
	int fdBinario = open(stringBinario, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU + S_IRWXG + S_IRWXO);
	if(!fdBinario) {
		printf("El fichero binario %s no puede ser creado.\n", stringBinario);
		return 1;
	}

	/* Leemos una línea del fichero en el formato "String(52) + Double(8) + Int(4) + Int(4)". */
	while(fscanf(ficheroTexto, "%s\t%lG\t%d\t%d\n", entrada.nombre, &entrada.peso, &entrada.referencia, &entrada.cantidad) != -1) {
		/* Limpiamos la cadena de caracteres nombre. */
		int i;
		for(i = 0; i < 52; i++) {
			entrada.nombre[i] = 0;
		}

		/* Escribimos en el fichero binario la entrada leída. */
		write(fdBinario, &entrada.nombre, sizeof(entrada.nombre));
		write(fdBinario, &entrada.peso, sizeof(entrada.peso));
		write(fdBinario, &entrada.referencia, sizeof(entrada.referencia));
		write(fdBinario, &entrada.cantidad, sizeof(entrada.cantidad));

		/* Imprimimos por pantalla la estructura leída. */
		printf("%s\t%g\t%d\t%d\n", entrada.nombre, entrada.peso, entrada.referencia, entrada.cantidad);
	}

	/* Cerramos los archivos */
	fclose(ficheroTexto);
	close(fdBinario);

	return 0;
}
