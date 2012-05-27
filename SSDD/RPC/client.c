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

CLIENT *clnt;

void usage(char *program_name) {
	printf("Usage: %s [-d] -s <server>\n", program_name);
}


void f_ping(){
	if (debug)
		printf("PING\n");

	//Variables de petición y respuesta.
	char byte = CODE_PING;
	char res;

	//Calcular tiempo actual.
	struct timeval tv1;
	gettimeofday(&tv1, NULL);

	//Ping al servidor.
	enum clnt_stat retval = ping_rpc_1(&byte, &res, clnt);

	//Calcular tiempo actual.
	struct timeval tv2;
	gettimeofday(&tv2, NULL);

	//Comprobamos que el procedimiento se ha realizado correctamente.
	if(retval == RPC_SUCCESS && res == CODE_PING) {
		//Calcular diferencia de tiempo.
		printf("%ld.%06ld\n", (tv2.tv_sec - tv1.tv_sec) / 2, (tv2.tv_usec - tv1.tv_usec) / 2);
	}
	else {
		clnt_perror(clnt, "[ERROR] La llamada al procedimiento remoto ha fallado.\n");
		clnt_destroy(clnt);
		exit(1);
	}
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
	unsigned int iter = (fdin_size / TEXT_BUFFER) + 1;

	//Creamos la petición swap.
	struct swap_data pet, res;
	pet.total_iter = htonl(iter);
	pet.total_length = htonl((unsigned int) fdin_size);
	pet.swapped = 0;
	pet.swapped_local = 0;

	//Iteramos recorriendo cada bloque del fichero.
	unsigned int swapped_global_calc = 0, swapped_global = 0;
	unsigned int i;
	for(i = 0; i < iter; i++) {
		pet.iter = htonl(i);

		//Leer del fichero de entrada un bloque de máximo HASH_BUFFER y escribirlo en la petición de swap.
		bytes_read = read(fdin, &pet.text, TEXT_BUFFER);
		if(bytes_read == -1) {
			perror("[ERROR] Fallo al leer datos del fichero de entrada.\n");
			break;
		}
		pet.text_length = htonl(bytes_read);

		//Swap al servidor.
		enum clnt_stat retval = swap_rpc_1(&pet, &res, clnt);
		if(retval != RPC_SUCCESS) {
			clnt_perror(clnt, "[ERROR] La llamada al procedimiento remoto ha fallado.\n");
			clnt_destroy(clnt);
			exit(1);
		}

		//(DEBUG) Información sobre la iteración del swap.
		if(DEBUG) {
			printf("swap iter[%d] %u/%u\n", i, ntohl(res.swapped_local), ntohl(res.text_length));
		}

		//Actualizamos los caracteres intercambiados.
		pet.swapped = res.swapped;
		swapped_global_calc += ntohl(res.swapped_local);
		swapped_global = ntohl(res.swapped);

		//Escribimos los bytes intercambiados al fichero de salida.
		bytes_written = write(fdout, res.text, ntohl(res.text_length));
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

void f_hash(char *src) {
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
	unsigned int iter = (fdin_size / TEXT_BUFFER) + 1;


	//Creamos la petición hash.
	struct hash_pet pet;
	pet.total_iter = htonl(iter);
	pet.total_length = htonl((unsigned int) fdin_size);
	pet.hash = 0;
	struct hash_res res;

	//Iteramos recorriendo cada bloque del fichero.
	unsigned int hash_global_calc = 0;
	unsigned int i;
	for(i = 0; i < iter; i++) {
		pet.iter = htonl(i);

		//Leer del fichero de entrada un bloque de máximo TEXT_BUFFER y escribirlo en la petición de hash.
		bytes_read = read(fdin, &pet.text, TEXT_BUFFER);
		if(bytes_read == -1) {
			perror("[ERROR] Fallo al leer datos del fichero de entrada.\n");
			break;
		}
		pet.text_length = htonl(bytes_read);

		//Hash al servidor.
		enum clnt_stat retval = hash_rpc_1(&pet, &res, clnt);
		if(retval != RPC_SUCCESS) {
			clnt_perror(clnt, "[ERROR] La llamada al procedimiento remoto ha fallado.\n");
			clnt_destroy(clnt);
			exit(1);
		}

		//(DEBUG) Información sobre la iteración del hash.
		if(DEBUG) {
			printf("hash iter[%d] %u (%u)\n", i, ntohl(res.hash_local), bytes_read);
		}

		//Actualizamos el hash.
		pet.hash = res.hash;
		hash_global_calc = (hash_global_calc + htonl(res.hash_local)) % HASH_MOD;
	}

	//Mostramos el hash obtenido..
	if(hash_global_calc == ntohl(res.hash)) {
		printf("%u\n", ntohl(res.hash));
	}
	else {
		printf("client[%u] vs server[%u]\n", hash_global_calc, ntohl(res.hash));
		printf("[ERROR] El hash obtenido del servidor no coincide.\n");
	}

	//Cerramos los ficheros de entrada y salida.
	close(fdin);
}

void f_check(char *src, int hash){
	if (debug)
		printf("CHECK <SRC=%s> <HASH=%d>\n", src, hash);
	
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
	unsigned int iter = (fdin_size / TEXT_BUFFER) + 1;

	//Creamos la petición check.
	struct check_pet pet;
	pet.total_iter = htonl(iter);
	pet.total_length = htonl((unsigned int) fdin_size);
	pet.hash = 0;
	pet.hash_client = htonl(hash);
	struct check_res res;

	//Iteramos recorriendo cada bloque del fichero.
	char check = 0;
	unsigned int hash_global_calc = 0;
	unsigned int i;
	for(i = 0; i < iter; i++) {
		pet.iter = htonl(i);

		//Leer del fichero de entrada un bloque de máximo TEXT_BUFFER y escribirlo en la petición de hash.
		bytes_read = read(fdin, &pet.text, TEXT_BUFFER);
		if(bytes_read == -1) {
			perror("[ERROR] Fallo al leer datos del fichero de entrada.\n");
			break;
		}
		pet.text_length = htonl(bytes_read);

		//Hash al servidor.
		enum clnt_stat retval = check_rpc_1(&pet, &res, clnt);
		if(retval != RPC_SUCCESS) {
			clnt_perror(clnt, "[ERROR] La llamada al procedimiento remoto ha fallado.\n");
			clnt_destroy(clnt);
			exit(1);
		}

		//(DEBUG) Información sobre la iteración del hash.
		if(DEBUG) {
			printf("check iter[%d] %u (%u)\n", i, ntohl(res.hash_local), bytes_read);
		}

		//Actualizamos el hash.
		pet.hash = res.hash;
		hash_global_calc = (hash_global_calc + htonl(res.hash_local)) % HASH_MOD;
		check = res.check;
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
	if(hash_global_calc != ntohl(res.hash)) {
		printf("[ERROR] El check obtenido del servidor no coincide.\n");
	}

	//Cerramos los ficheros de entrada y salida.
	close(fdin);
}

void f_stat(){
	if (debug)
		printf("STAT\n");
	
	//Variables de petición y respuesta.
	char byte = CODE_STAT;
	struct server_stat res;

	//Informar al servidor de la salida.
	enum clnt_stat retval = stat_rpc_1(&byte, &res, clnt);

	//Comprobamos que el procedimiento se ha realizado correctamente.
	if(retval == RPC_SUCCESS) {
		//Mostramos las estadísticas.
		printf("ping %u\n", ntohl(res.ping));
		printf("swap %u\n", ntohl(res.swap));
		printf("hash %u\n", ntohl(res.hash));
		printf("check %u\n", ntohl(res.check));
		printf("stat %u\n", ntohl(res.stat));
	}
	else {
		clnt_perror(clnt, "[ERROR] La llamada al procedimiento remoto ha fallado.\n");
		clnt_destroy(clnt);
		exit(1);
	}
}

void f_quit(){
	if (debug)
		printf("QUIT\n");
	
	//Variables de petición y respuesta.
	char byte = CODE_QUIT;
	char res;

	//Informar al servidor de la salida.
	enum clnt_stat retval = quit_rpc_1(&byte, &res, clnt);
	if(retval != RPC_SUCCESS) {
		clnt_perror(clnt, "[ERROR] La llamada al procedimiento remoto ha fallado.\n");
		clnt_destroy(clnt);
		exit(1);
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
					if (p.we_wordc == 1) {
						f_quit();
						exit = 1;
					} else {
						printf("Syntax error. Use: quit\n");
					}
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
	int opt;
	char *server;

	setbuf(stdout, NULL);
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "ds:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 's':
				server = optarg;
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
	
	if (debug)
		printf("SERVER: %s\n", server);

	clnt = clnt_create(server, SSDD_TEXT, FINAL, "tcp");
	if(clnt == NULL) {
		printf("c> Error en la conexión con el servidor %s\n", server);
		return -1;
	}
	
	shell();
	
	clnt_destroy(clnt);
	
	exit(EXIT_SUCCESS);
}

