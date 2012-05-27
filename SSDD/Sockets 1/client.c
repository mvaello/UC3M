#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>

#include <wordexp.h>

#include "common.h"
#include "client.h"

int debug = 0;
int sockfd;

void usage(char *program_name) {
	printf("Usage: %s [-d] -s <server> -p <port>\n", program_name);
}

void f_ping(){
	if (debug)
		printf("PING\n");

	//Calcular tiempo actual.
	struct timeval tv1;
	gettimeofday(&tv1, NULL);

	//Ping server.
	char byte[1];
	byte[0] = CODE_PING;
	ssize_t bytes_written;
	if((bytes_written = write(sockfd, byte, 1)) == -1) {
		perror("c> Error transfiriendo datos al servidor.\n");
		return;
	}

	//Recibir respuesta.
	char bytes[SOCK_BUFFER];
	ssize_t bytes_read;
	if((bytes_read = read(sockfd, bytes, SOCK_BUFFER)) == -1) {
		perror("c> Error recibiendo datos del servidor.\n");
		return;
	}
	else if(bytes_read == 0) {
		printf("c> El servidor ha cerrado la conexión.\n");
		close(sockfd);
	
		exit(EXIT_SUCCESS);
	}

	//Comprobamos que la respuesta del servidor coincide con el ping.
	if(bytes[0] == CODE_PING) {
		//Calcular diferencia de tiempo.
		struct timeval tv2;
		gettimeofday(&tv2, NULL);
		printf("%ld.%06ld\n", (tv2.tv_sec - tv1.tv_sec) / 2, (tv2.tv_usec - tv1.tv_usec) / 2);
	}

	return;
}

void f_swap(char *src, char *dst){
	if (debug)
		printf("SWAP <SRC=%s> <DST=%s>\n", src, dst);
	
	//Descriptores de fichero.
	int fdin = open(src, O_RDONLY, S_IRWXU + S_IRWXG + S_IRWXO);
	int fdout = open(dst, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU + S_IRWXG + S_IRWXO);

	//Comprobar errores al obtener los descriptores de fichero.
	if(!fdin) {
		perror("[ERROR] Error al crear el descriptor del fichero de entrada.\n");
		return;
	}
	if(!fdout) {
		perror("[ERROR] Error al crear el descriptor del fichero de salida.\n");
		return;
	}

	//Comprobamos si existe el fichero de entrada y si está vacío.
	struct stat st;
	if(fstat(fdin, &st) == -1) {
		perror("[ERROR] No existe el fichero de entrada.\n");
		return;
	}
	long fdin_size = st.st_size;
	if(fdin_size == 0) {
		perror("[ERROR] El fichero de entrada está vacío.\n");
		return;
	}

	//Variables para comprobación de Lectura/Escritura.
	ssize_t bytes_read, bytes_written;

	//Calcular el número de iteraciones a realizar.
	unsigned int iter = (fdin_size / SWAP_BUFFER) + 1;

	//Creamos la petición swap.
	struct swap petition;
	petition.code = CODE_SWAP;
	petition.total_iter = htonl(iter);
	petition.total_length = htonl((unsigned int) fdin_size);

	//Mandar la petición de swap al servidor.
	if((bytes_written = write(sockfd, (char*) &petition, sizeof(struct swap))) == -1) {
		perror("[ERROR] Fallo al enviar datos al servidor.\n");
		close(fdin);
		close(fdout);
		return;
	}

	unsigned int swapped_global_calc = 0, swapped_global = 0;
	char bytes[SOCK_BUFFER];
	unsigned int i;
	for(i = 0; i < iter; i++) {
		//Iteración SWAP.
		struct swap_iter swap_out;
		swap_out.code = CODE_SWAP_ITER;
		swap_out.id = htonl(i);

		//Leer del fichero de entrada un bloque de máximo SWAP_BUFFER y escribirlo en la petición de swap.
		bytes_read = read(fdin, &swap_out.text, SWAP_BUFFER);
		if(bytes_read == -1) {
			perror("[ERROR] Fallo al leer datos del fichero de entrada.\n");
			break;
		}
		swap_out.text_length = htonl(bytes_read);

		//Mandar la petición de swap al servidor.
		if((bytes_written = write(sockfd, (char*) &swap_out, sizeof(struct swap_iter))) == -1) {
			perror("[ERROR] Fallo al enviar datos al servidor.\n");
			break;
		}

		//Esperamos la respuesta del servidor.
		if((bytes_read = read(sockfd, bytes, SOCK_BUFFER)) == -1) {
			perror("[ERROR] Fallo al recibir datos del servidor.\n");
			break;
		}
		else if(bytes_read == 0) {
			printf("c> El servidor ha cerrado la conexión.\n");
			close(sockfd);
	
			exit(EXIT_SUCCESS);
		}

		//Comprobamos que el mensaje de respuesta sea del tipo SWAP.
		if(bytes[0] != CODE_SWAP_ITER) {
			printf("[ERROR] Se esperaba una respuesta de tipo SWAP_ITER.\n");
			break;
		}

		//Asignamos la respuesta a la estructura de datos de swap.
		struct swap_iter *swap_in = (struct swap_iter *) &bytes;

		//Comprobar si la id es correcta.
		if(ntohl(swap_in->id) != i) {
			printf("[ERROR] Identificador de swap incorrecto: id[%u] != %u\n", i, ntohl(swap_in->id));
			break;
		}

		//(DEBUG) Información sobre la iteración del swap.
		if(debug) {
			printf("swap iter[%d] %u/%u\n", i, ntohl(swap_in->swapped), ntohl(swap_in->text_length));
		}

		//Actualizamos los caracteres intercambiados.
		swapped_global_calc += ntohl(swap_in->swapped);
		swapped_global = ntohl(swap_in->swapped_global);

		//Escribimos los bytes intercambiados al fichero de salida.
		bytes_written = write(fdout, swap_in->text, ntohl(swap_in->text_length));
		if(bytes_written == -1) {
			perror("[ERROR] Fallo al escribir datos en el fichero de salida.\n");
			break;
		}
	}

	//Mostramos los swaps realizados.
	if(swapped_global_calc == swapped_global) {
		printf("%u\n", swapped_global);
	}
	else {
		printf("client[%u] vs server[%u]\n", swapped_global_calc, swapped_global);
		printf("[ERROR] El swap obtenido del servidor no coincide.\n");
	}

	//Cerramos los ficheros de entrada y salida.
	close(fdin);
	close(fdout);
}

void f_hash(char *src){
	if (debug)
		printf("HASH <SRC=%s>\n", src);
	
	//Descriptores de fichero.
	int fdin = open(src, O_RDONLY, S_IRWXU + S_IRWXG + S_IRWXO);

	//Comprobar errores al obtener los descriptores de fichero.
	if(!fdin) {
		perror("[ERROR] Error al crear el descriptor del fichero de entrada.\n");
		return;
	}

	//Comprobamos si existe el fichero de entrada y si está vacío.
	struct stat st;
	if(fstat(fdin, &st) == -1) {
		perror("[ERROR] No existe el fichero de entrada.\n");
		return;
	}
	long fdin_size = st.st_size;
	if(fdin_size == 0) {
		perror("[ERROR] El fichero de entrada está vacío.\n");
		return;
	}

	//Variables para comprobación de Lectura/Escritura.
	ssize_t bytes_read, bytes_written;

	//Calcular el número de iteraciones a realizar.
	unsigned int iter = (fdin_size / SWAP_BUFFER) + 1;

	//Creamos la petición hash.
	struct hash petition;
	petition.code = CODE_HASH;
	petition.total_iter = htonl(iter);
	petition.total_length = htonl((unsigned int) fdin_size);

	//Mandar la petición de hash al servidor.
	if((bytes_written = write(sockfd, (char*) &petition, sizeof(struct hash))) == -1) {
		perror("[ERROR] Fallo al enviar datos al servidor.\n");
		close(fdin);
		return;
	}

	unsigned int hash_global = 0, hash_global_calc = 0;
	char bytes[SOCK_BUFFER];
	unsigned int i;
	for(i = 0; i < iter; i++) {
		//Iteración HASH.
		struct hash_iter_pet hash_out;
		hash_out.code = CODE_HASH_ITER_PET;
		hash_out.id = htonl(i);

		//Leer del fichero de entrada un bloque de máximo HASH_BUFFER y escribirlo en la petición de hash.
		if((bytes_read = read(fdin, &hash_out.text, HASH_BUFFER)) == -1) {
			perror("[ERROR] Fallo al leer datos del fichero de entrada.\n");
			break;
		}
		hash_out.text_length = htonl(bytes_read);

		//Mandar la petición de swap al servidor.
		if((bytes_written = write(sockfd, (char*) &hash_out, sizeof(struct hash_iter_pet))) == -1) {
			perror("[ERROR] Fallo al enviar datos al servidor.\n");
			break;
		}

		//Esperamos la respuesta del servidor.
		if((bytes_read = read(sockfd, bytes, SOCK_BUFFER)) == -1) {
			perror("[ERROR] Fallo al recibir datos del servidor.\n");
			break;
		}
		else if(bytes_read == 0) {
			printf("c> El servidor ha cerrado la conexión.\n");
			close(sockfd);
		
			exit(EXIT_SUCCESS);
		}

		//Comprobamos que el mensaje de respuesta sea del tipo CODE_HASH_ITER_RES.
		if(bytes[0] != CODE_HASH_ITER_RES) {
			printf("[ERROR] Se esperaba una respuesta de tipo HASH_ITER_RES.\n");
			break;
		}

		//Asignamos la respuesta a la estructura de datos de hash.
		struct hash_iter_res *hash_in = (struct hash_iter_res *) &bytes;

		//Comprobar si la id es correcta.
		if(ntohl(hash_in->id) != i) {
			printf("[ERROR] Identificador de hash incorrecto: id[%u] != %u\n", i, ntohl(hash_in->id));
			break;
		}

		//(DEBUG) Información sobre la iteración del swap.
		if(debug) {
			printf("hash iter[%d] %u (%u)\n", i, ntohl(hash_in->hash), ntohl(hash_out.text_length));
		}

		//Actualizamos el hash.
		hash_global_calc = (hash_global_calc + ntohl(hash_in->hash)) % HASH_MOD;
		hash_global = ntohl(hash_in->hash_global);
	}

	//Mostramos el hash obtenido.
	if(hash_global_calc == hash_global) {
		printf("%u\n", hash_global);
	}
	else {
		printf("client[%u] vs server[%u]\n", hash_global_calc, hash_global);
		printf("[ERROR] El hash obtenido del servidor no coincide.\n");
	}

	//Cerramos el descriptor de fichero de entrada.
	close(fdin);
}

void f_check(char *src, int hash){
	if (debug)
		printf("CHECK <SRC=%s> <HASH=%d>\n", src, hash);
	
		if (debug)
		printf("HASH <SRC=%s>\n", src);
	
	//Descriptores de fichero.
	int fdin = open(src, O_RDONLY, S_IRWXU + S_IRWXG + S_IRWXO);

	//Comprobar errores al obtener los descriptores de fichero.
	if(!fdin) {
		perror("[ERROR] Error al crear el descriptor del fichero de entrada.\n");
		return;
	}

	//Comprobamos si existe el fichero de entrada y si está vacío.
	struct stat st;
	if(fstat(fdin, &st) == -1) {
		perror("[ERROR] No existe el fichero de entrada.\n");
		return;
	}
	long fdin_size = st.st_size;
	if(fdin_size == 0) {
		perror("[ERROR] El fichero de entrada está vacío.\n");
		return;
	}

	//Variables para comprobación de Lectura/Escritura.
	ssize_t bytes_read, bytes_written;

	//Calcular el número de iteraciones a realizar.
	unsigned int iter = (fdin_size / SWAP_BUFFER) + 1;

	//Creamos la petición check.
	struct check petition;
	petition.code = CODE_CHECK;
	petition.total_iter = htonl(iter);
	petition.total_length = htonl((unsigned int) fdin_size);
	petition.hash = htonl(hash);

	//Mandar la petición de check al servidor.
	if((bytes_written = write(sockfd, (char*) &petition, sizeof(struct check))) == -1) {
		perror("[ERROR] Fallo al enviar datos al servidor.\n");
		close(fdin);
		return;
	}

	char check = 0;

	unsigned int hash_global = 0, hash_global_calc = 0;
	char bytes[SOCK_BUFFER];
	unsigned int i;
	for(i = 0; i < iter; i++) {
		//Iteración CHECK.
		struct check_iter_pet check_out;
		check_out.code = CODE_CHECK_ITER_PET;
		check_out.id = htonl(i);

		//Leer del fichero de entrada un bloque de máximo CHECK_BUFFER y escribirlo en la petición de check.
		if((bytes_read = read(fdin, &check_out.text, CHECK_BUFFER)) == -1) {
			perror("[ERROR] Fallo al leer datos del fichero de entrada.\n");
			break;
		}
		check_out.text_length = htonl(bytes_read);

		//Mandar la petición de swap al servidor.
		if((bytes_written = write(sockfd, (char*) &check_out, sizeof(struct check_iter_pet))) == -1) {
			perror("[ERROR] Fallo al enviar datos al servidor.\n");
			break;
		}

		//Esperamos la respuesta del servidor.
		if((bytes_read = read(sockfd, bytes, SOCK_BUFFER)) == -1) {
			perror("[ERROR] Fallo al recibir datos del servidor.\n");
			break;
		}
		else if(bytes_read == 0) {
			printf("c> El servidor ha cerrado la conexión.\n");
			close(sockfd);
		
			exit(EXIT_SUCCESS);
		}

		//Comprobamos que el mensaje de respuesta sea del tipo CODE_HASH_ITER_RES.
		if(bytes[0] != CODE_CHECK_ITER_RES) {
			printf("[ERROR] Se esperaba una respuesta de tipo HASH_ITER_RES.\n");
			break;
		}

		//Asignamos la respuesta a la estructura de datos de check.
		struct check_iter_res *check_in = (struct check_iter_res *) &bytes;

		//Comprobar si la id es correcta.
		if(ntohl(check_in->id) != i) {
			printf("[ERROR] Identificador de check incorrecto: id[%u] != %u\n", i, ntohl(check_in->id));
			break;
		}

		//(DEBUG) Información sobre la iteración del check.
		if(debug) {
			printf("check iter[%d] %u (%u)\n", i, ntohl(check_in->hash), ntohl(check_out.text_length));
		}

		//Actualizamos el hash.
		hash_global_calc = (hash_global_calc + ntohl(check_in->hash)) % HASH_MOD;
		hash_global = ntohl(check_in->hash_global);

		//Guardamos el check del servidor.
		check = check_in->check;
	}

	//Comprobamos si ha habido errores en el hash.
	if(hash_global_calc != hash_global) {
		printf("client[%u] vs server[%u]\n", hash_global_calc, hash_global);
		printf("[ERROR] El hash obtenido del servidor no coincide.\n");
	}

	//Imprimimos la comprobación del servidor.
	char *check_string;
	if(check == CHECK_OK) {
		check_string = CHECK_STRING_OK;
	}
	else if(check == CHECK_FAIL) {
		check_string = CHECK_STRING_FAIL;
	}
	printf("%s\n", check_string);

	//Comprobamos que el check es correcto.
	if(hash_global != hash_global_calc) {
		printf("[ERROR] El check obtenido del servidor no coincide.\n");
	}

	//Cerramos el descriptor de fichero de entrada.
	close(fdin);
}

void f_stat(){
	if (debug)
		printf("STAT\n");

	//Solicitar estadísticas al servidor.
	char byte[1];
	byte[0] = CODE_STAT;
	ssize_t bytes_written;
	if((bytes_written = write(sockfd, byte, 1)) == -1) {
		perror("[ERROR] Fallo al enviar datos al servidor.\n");
		return;
	}

	//Recibir estadísticas del servidor.
	char bytes[SOCK_BUFFER];
	ssize_t bytes_read;
	if((bytes_read = read(sockfd, bytes, SOCK_BUFFER)) == -1) {
		perror("[ERROR] Fallo al recibir datos del servidor.\n");
		return;
	}
	else if(bytes_read == 0) {
		printf("c> El servidor ha cerrado la conexión.\n");
		close(sockfd);
	
		exit(EXIT_SUCCESS);
	}

	//Comprobamos que el mensaje de respuesta sea del tipo STAT.
	if(bytes[0] != CODE_STAT) {
		printf("[ERROR] Se esperaba una respuesta de tipo STAT.\n");
		return;
	}

	//Asignamos la respuesta a la estructura de datos de cstat.
	struct client_stat *cstat = (struct client_stat *) &bytes;

	//Mostramos las estadísticas.
	printf("ping %u\n", ntohl(cstat->ping));
	printf("swap %u\n", ntohl(cstat->swap));
	printf("hash %u\n", ntohl(cstat->hash));
	printf("check %u\n", ntohl(cstat->check));
	printf("stat %u\n", ntohl(cstat->stat));
}

void quit() {
	//Quit server.
	char byte[1];
	byte[0] = CODE_EXIT;
	ssize_t bytes_written;
	if((bytes_written = write(sockfd, byte, 1)) == -1) {
		perror("c> Error transfiriendo datos al servidor.\n");
		return;
	}
	else if(bytes_written == 0) {
		printf("c> El servidor ha cerrado la conexión.\n");
		close(sockfd);
	
		exit(EXIT_SUCCESS);
	}
}

void shell() {
	char line[1024];
	char *pch;
	int exit = 0;
	
	wordexp_t p;
	char **w;
	int ret;
	
	memset(&p, 0, sizeof(wordexp));
	
	do {
		fprintf(stdout, "c> ");
		memset(line, 0, 1024);
		pch = fgets(line, 1024, stdin);
		
		if ( (strlen(line)>1) && ((line[strlen(line)-1]=='\n') || (line[strlen(line)-1]=='\r')) )
			line[strlen(line)-1]='\0';
		
		ret=wordexp((const char *)line, &p, 0);
		if (ret == 0) {
			w = p.we_wordv;
		
			if ( (w != NULL) && (p.we_wordc > 0) ) {
				if (strcmp(w[0],"ping")==0) {
					if (p.we_wordc == 1)
						f_ping();
					else
						printf("Syntax error. Use: ping\n");
				} else if (strcmp(w[0],"swap")==0) {
					if (p.we_wordc == 3)
						f_swap(w[1],w[2]);
					else
						printf("Syntax error. Use: swap <source_file> <destination_file>\n");
				} else if (strcmp(w[0],"hash")==0) {
					if (p.we_wordc == 2)
						f_hash(w[1]);
					else
						printf("Syntax error. Use: hash <source_file>\n");
				} else if (strcmp(w[0],"check")==0) {
					if (p.we_wordc == 3)
						f_check(w[1], atoi(w[2]));
					else
						printf("Syntax error. Use: check <source_file> <hash>\n");
				} else if (strcmp(w[0],"stat")==0) {
					if (p.we_wordc == 1)
						f_stat();
					else
						printf("Syntax error. Use: stat\n");
				} else if (strcmp(w[0],"quit")==0) {
					quit();
					exit = 1;
				} else {
					fprintf(stderr, "Error: command '%s' not valid.\n", w[0]);
				}
			}
			
			wordfree(&p);
		}
	} while ((pch != NULL) && (!exit));
}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
	int opt, port=0;
	char *server, *port_s;
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "ds:p:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 's':
				server = optarg;
				break;
			case 'p':
				port_s = optarg;
				port = strtol(optarg, NULL, 10);
				break;
			case '?':
				if ((optopt == 's') || (optopt == 'p'))
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				usage(program_name);
				exit(EX_USAGE);
		}
	}
	
	if ((port < 1024) || (port > 65535)) {
		fprintf (stderr, "Error: Port must be in the range 1024 <= port <= 65535\n");
		usage(program_name);
		exit(EX_USAGE);
	}
	
	if (debug)
		printf("SERVER: %s PORT: %d\n", server, port);

	//Comprobamos si la dirección de entrada es una IP o un dominio, y configuramos el puerto.
	struct sockaddr_in address;
	struct hostent *hp;
	hp = gethostbyname(server);
	if(hp == NULL) {
		address.sin_addr.s_addr = inet_addr(server);
	}
	else {
		memcpy(&address.sin_addr.s_addr,*(hp->h_addr_list),sizeof(address.sin_addr.s_addr));
	}
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	//Abrimos el descriptor del socket.
	if((sockfd = socket(address.sin_family, SOCK_STREAM, TCP)) == -1) {
		printf("c> Error en la conexión con el servidor %s:%d.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		return -1;
	}

	//Intentamos realizar la conexión con el servidor.
	if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
		printf("c> Error en la conexión con el servidor %s:%d.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		return -1;
	}

	shell();

	close(sockfd);
	
	exit(EXIT_SUCCESS);
}

