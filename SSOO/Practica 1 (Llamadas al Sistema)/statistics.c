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

/* Declaramos el método calcularStats. */
int calcularStats(char* stringBinario);

int main(int argc, char *argv[]){
	/* Comprobamos que se han pasado tres argumentos, si no es así finalizamos el programa e informamos del uso de éste. */
	if(argc != 2) {
		printf("Modo de Empleo: statistics <fichero_binario>\n");
		return 1;
	}

	/* Llamamos al método que calcula las estadísticas. */
	return calcularStats(argv[1]);
}

int calcularStats(char* stringBinario) {
	/* Declaramos la estructura. */
	struct estructura entrada;

	/* Comprobamos que se puede abrir el fichero binario. */
	int fdBinario = open(stringBinario,O_RDONLY);
	if(!fdBinario) {
		printf("El fichero binario %s no puede ser creado.\n", stringBinario);
		return 1;
	}

	/* Calculamos el tamaño del binario de entrada. */
	struct stat st;
	stat(stringBinario, &st);
	long binarioSize = st.st_size;

	/* Calculamos el número de entradas que el fichero binario contiene. */
	long fileEntries = binarioSize / (sizeof(entrada.nombre) + sizeof(entrada.peso) + sizeof(entrada.referencia) + sizeof(entrada.cantidad));

	/* Declaramos las variables que almacen las piezas con respecto a su cantidad. */
	int cantidadAlta = 0, cantidadMedia = 0, cantidadBaja = 0;

	/* Nos colocamos al principio del fichero. */
	lseek(fdBinario, 0, SEEK_SET);

	/* Leemos las entradas del binario de entrada y procesamos del tipo al que pertenecen. */
	int i;
	for(i = 0; i < fileEntries; i++) {
		/* Leemos las líneas del fichero con el formato "String(52) + Double(8) + Int(4) + Int(4)". */
		read(fdBinario, &entrada.nombre, sizeof(entrada.nombre));
		read(fdBinario, &entrada.peso, sizeof(entrada.peso));
		read(fdBinario, &entrada.referencia, sizeof(entrada.referencia));
		read(fdBinario, &entrada.cantidad, sizeof(entrada.cantidad));

		/* Sumamos la pieza leída a la sección correspondiente según la cantidad que tengamos. */
		if(entrada.cantidad >= 0 && entrada.cantidad < 100) {
			cantidadBaja++;
		}
		else if(entrada.cantidad >= 100 && entrada.cantidad < 500) {
			cantidadMedia++;
		}
		else if(entrada.cantidad >= 500 && entrada.cantidad <= 1000) {
			cantidadAlta++;
		}
	}

	/* Calculamos los porcentajes de las piezas. */
	int cantidadTotal;
	cantidadTotal = cantidadAlta + cantidadMedia + cantidadBaja;

	/* Mostramos los resultados tras procesar el fichero binario. */
	printf("B: %d%%\n", (cantidadBaja * 100 / cantidadTotal));
	printf("M: %d%%\n", (cantidadMedia * 100 / cantidadTotal));
	printf("A: %d%%\n", (cantidadAlta * 100 / cantidadTotal));

	/* Cerramos el archivo. */
	close(fdBinario);

	return 0;
}
