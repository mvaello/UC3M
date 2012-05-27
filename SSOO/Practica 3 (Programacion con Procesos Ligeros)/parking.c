#include "parking.h"

/* Creamos los mutex y condiciones para los threads del parking y de los coches. */
pthread_mutex_t mutex;
pthread_cond_t carOrder;
pthread_cond_t parkingSleeping;

/* variables globales. */
FILE* textFile; /* Fichero en el que se escriben los datos del programa. */
int occupation = 0; /* Variable que almacena el número de coches estacionados en el parking. */
int* parkingStatus; /* Variable que almacena el estado de cada planta del parking. */
int waitingCars; /* Variable que alamacena el número de coches esperando para entrar en el parking. */

/* Terminado. */
int main(int argc, char *argv[]){
	/* Comprobamos que se han pasado tres argumentos, si no es así finalizamos el programa e informamos del uso de éste. */
	if(argc != 4) {
		printf("Modo de Empleo: %s <num_plantas> <num_plazas_planta> <fichero_salida>\n", argv[0]);
		return -1;
	}

	/* Comprobamos si se puede crear un descriptor de fichero para el nombre pasado como argumento. */
	textFile = fopen(argv[3], "w");
	if(!textFile) {
		printf("Error al abrir el fichero de texto: %s.\n", argv[3]);
		return -1;
	}

	/* Convertimos los argumentos a enteros. */
	int parkingFloors = atoi(argv[1]);
	int parkingPlaces = atoi(argv[2]);

	/* Comprobación para que los usuarios no introduzcan capacidades del parking incorrectas. */
	if(parkingFloors <= 0) {
		printf("<num_plantas> tiene que ser mayor que 0.");
		return -1;
	}
	if(parkingPlaces <= 0) {
		printf("<num_plazas_planta> tiene que ser mayor que 0.");
		return -1;
	}

	/* Llamamos al método principal del parking. */
	return parkingInit(parkingFloors, parkingPlaces);
}

/* NO terminado. */
int parkingInit(int parkingFloors, int parkingPlaces) {
	/* Llamamos a la función srand() para generar número aleatorios. */
	srand(time(NULL));

	/* Calculamos el número máximo de coches. */
	int maxCars = getMaxPlaces(parkingFloors, parkingPlaces);

	/*
		Creamos un array de enteros para el estado del parking.
		Cada posición del array equivale a cada una de las plantas.
		Cada una de estas posiciones almacenará un valor con el número de plazas libres en esa planta.
	*/
	/*
	int aux[parkingFloors];
	parkingStatus = aux;
	*/
	parkingStatus = malloc(sizeof(int*) * parkingFloors);

	/* Llenamos las plazas libres en cada planta. */
	int i;
	for(i = 0; i < sizeof(parkingStatus); i++) {
		parkingStatus[i] = parkingPlaces;
	}

	/* Variables para los threads. */
	pthread_t carThread;
	pthread_t parkingThread;

	/* Crear el thread de los coches como detached. */
	pthread_attr_t attrCar;
	pthread_attr_init(&attrCar);
	pthread_attr_setdetachstate(&attrCar, PTHREAD_CREATE_DETACHED);

	/* Condiciones y mutex. */
	pthread_mutex_init(&mutex, NULL); /* Mutex para controlar los accesos a los contadores. */
	pthread_cond_init(&parkingSleeping, NULL); /* Condición que hace que el parking espere la entrada/salida de más coches. */
	pthread_cond_init(&carOrder, NULL); /* Condición que indica al coche que debe aparcar. */

	/* Creamos el thread que gestiona el parking. */
	pthread_create(&parkingThread, NULL, parking, NULL);

	int counter = 0; /* Cuenta el número de coches introducidos. */
	int* arguments; /* Array usado para pasar parámetros a la función del thread coche. */
	/* Bucle principal. */
	while(TRUE) {
		printf("parking> "); /* PROMPT */

		int cars = readNumber(); /* Leer un entero. */

		if(cars == 0) { /* Esperar a que los vehículos terminen de entrar y salir. */
			while(TRUE) {
				/* Bloqueamos el acceso al contador. */
				pthread_mutex_lock(&mutex);

				/* Si no hay coches dentro del parking y tampoco hay coches esperando fuera salimos del bucle de espera. */
				if(occupation == 0 && waitingCars == 0) {
					break;
				}

				/* Bloqueamos el acceso al contador. */
				pthread_mutex_unlock(&mutex);
			}

			break; /* Salimos del bucle principal. */
		}
		else if(cars > 0) {
			/* Creamos un thread para cada nuevo coche introducido. */
			for(i = 0; i < cars; i++) {
				carThread = (pthread_t) malloc(sizeof(pthread_t));

				/* En el array de argumentos reservamos espacio para pasar el valor de dos enteros: el contador y el número máximo de coches. */
				arguments = malloc(sizeof(int) * 2);
				arguments[0] = counter;
				arguments[1] = maxCars;

				/* Si ha habido un error al crear el proceso ligero lo notificamos y no incrementamos el contador de coches introducidos. */
				if (pthread_create(&carThread, &attrCar, car, (void*) arguments) == -1) {
					printf("Error al crear proceso ligero\n");
				}
				else {
					counter++;
				}
			}
		}
		else { /* Se ha leído un número menor que 0. Formato inválido -> Volvemos a pedir un nuevo número. */
			printf("Número introducido no válido (%d).\n", cars);
		}
	}

	/* Cerramos el descriptor del fichero de texto. */
	fclose(textFile);

	/* Destruímos el mutex. */
	pthread_mutex_destroy(&mutex);

	return 0;
}

/* Lee un número por teclado. */
int readNumber() {
	/* Declaramos la variable number para almacenar el número leído por teclado. */
	int number;

	/* Leemos un número por teclado. */
	scanf("%d", &number);

	/* Devolvemos la conversión a entero de la cadena leída. */
	return number;
}

/* Genera un número aleatorio entre los números pasados como parámetro, ambos incluídos. */
int generateRandom(int min, int max) {
	return rand() % (max - min + 1) + min;
}

/* Calculamos el número máximo de plazas del parking multiplicando las plantas por el número de plazas de cada planta. */
int getMaxPlaces(int parkingFloors, int parkingPlaces) {
	return parkingPlaces * parkingFloors;
}

/* Método que busca un hueco en una de las plantas sucesivamente y devuelve el número de la planta en la que se ha encontrado el hueco. */
int parkCar() {
	int i;
	for(i = 0; i < sizeof(parkingStatus); i++) {
		if(parkingStatus[i] > 0) {
			parkingStatus[i]--;
			break;
		}
	}

	return i + 1;
}

/* Método que saca un coche de una planta del parking. */
void removeCar(int floor) {
	parkingStatus[floor - 1]++;
}

/* NO terminado. */
void* parking() {
	while(TRUE) {
		pthread_mutex_lock(&mutex); /* Bloqueamos el contador para que nadie pueda acceder a él. */

		while (occupation == 0) {
			pthread_cond_wait(&parkingSleeping, &mutex); /* se bloquea */
		}

		pthread_cond_signal(&carOrder); /* Le decimos a uno de los coches nuevos que aparquen. */
		pthread_mutex_unlock(&mutex); /* Desbloqueamos el contador. */
	}

	/* Salimos del thread del parking. */
	pthread_exit(0);
}

/* NO terminado. */
void* car(void* p) {
	/* Converitmos el puntero pasado a un puntero de tipo entero. Aisgnamos cada posición del array a su variable correspondiente. */
	int* arguments = (int*) p;
	int carNumber = arguments[0];
	int maxCars = arguments[1];

	/* El coche pasa a estar en espera a entrar en el parking. */
	waitingCars++;

	while(TRUE) {
		/* Bloqueamos el acceso al contador. */
		pthread_mutex_lock(&mutex);
		if(occupation < maxCars) {
			/* El coche deja de estar en espera y entra en el parking. */
			waitingCars--;
			occupation++;
			fprintf(textFile, "[%d] entra en parking, %d plazas libres\n", carNumber, maxCars - occupation);

			/* Desbloqueamos el acceso al contador. */
			pthread_mutex_unlock(&mutex);


			/* El coche duerme un tiempo aleatorio entre 1 y 10 segundos.*/
			sleep(generateRandom(MINSLEEP, MAXSLEEP));


			/* Bloqueamos el acceso al contador. */
			pthread_mutex_lock(&mutex);

			/* Esperamos a que el parking le dé al coche la señal de aparcar. */
			pthread_cond_signal(&parkingSleeping);
			pthread_cond_wait(&carOrder, &mutex);

			/* El coche se aparca en un hueco de la planta más baja disponible. */
			int floor = parkCar();
			fprintf(textFile, "[%d] accede a planta %d, %d plazas libres en planta\n", carNumber, floor, parkingStatus[floor - 1]);

			/* Desbloqueamos el acceso al contador. */
			pthread_mutex_unlock(&mutex);


			/* El coche duerme un tiempo aleatorio entre 1 y 10 segundos.*/
			sleep(generateRandom(MINSLEEP, MAXSLEEP));


			/* Bloqueamos el acceso al contador. */
			pthread_mutex_lock(&mutex);

			/* El coche sale de su estacionamiento y acto seguido del parking. */
			removeCar(floor);
			fprintf(textFile, "[%d] sale de planta %d, %d plazas libres en planta\n", carNumber, floor, parkingStatus[floor - 1]);
			occupation--;
			fprintf(textFile, "[%d] sale del parking, %d plazas libres\n", carNumber, maxCars - occupation);

			/* Desbloqueamos el acceso al contador. */
			pthread_mutex_unlock(&mutex);

			/* Salimos del bucle para terminar el thread del coche. */
			break;
		}
		/* Desbloqueamos el acceso al contador. */
		pthread_mutex_unlock(&mutex);
	}

	/* Salimos del thread del coche. */
	pthread_exit(0);
}
