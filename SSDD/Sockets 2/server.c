#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>

#include "common.h"
#include "server.h"

int debug = 0;
int busy = TRUE;

pthread_mutex_t mutex;
pthread_cond_t thread_ready;

void usage(char *program_name) {
	printf("Usage: %s [-d] -p <port>\n", program_name);
}

int main(int argc, char *argv[]) {
	char *program_name = argv[0];
	int opt, port=0;
	char *port_s;
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "dp:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 'p':
				port_s = optarg;
				port = strtol(optarg, NULL, 10);
				break;
			case '?':
				if (optopt == 'p')
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
		fprintf(stderr, "DEBUG ON\n");

	//Creamos el descriptor del socket para el servidor.
	int server_sd;
	struct sockaddr_in server_addr;
	if((server_sd = socket(AF_INET, SOCK_STREAM, TCP)) == -1) {
		perror("s> Error al crear el socket.\n");
		exit(1);
	}

	//Ponemos al servidor a escuchar en cualquier dirección de la máquina y en el puerto pasado como argumento.
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	if(bind(server_sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
		perror("s> Error en el bind del servidor.\n");
		close(server_sd);
		return -1;
	}

	//Informamos de que se ha creado correctamente el bind del socket.
	printf("s> init server %s:%d\n", inet_ntoa(server_addr.sin_addr), port);

	//Ponemos el servidor a escuchar pasivamente.
	if(listen(server_sd, MAX_QUEUE_CONN) == -1) {
		perror("s> Error en la escucha pasiva del servidor.\n");
		close(server_sd);
		return -1;
	}

	//Informamos de que el servidor está esperando.
	printf("s> waiting\n");

	//Creamos un array de threads con tamaño el máximo número de clientes conectados que el servidor soportará.
	pthread_t client_thread;

	//Creamos los atributos de los threads que los clientes van a usar.
	pthread_attr_t client_attr;
	pthread_attr_init(&client_attr);
	pthread_attr_setdetachstate(&client_attr, PTHREAD_CREATE_DETACHED);

	//Mutex y señal.
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&thread_ready, NULL);

	//Descriptor y dirección del cliente.
	int client_sd;
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	//Bucle para aceptar conexiones de clients.
	while(TRUE) {
		//Esperamos la conexión de un cliente.
		if((client_sd = accept(server_sd, (struct sockaddr *)  &client_addr, &addr_len)) == -1) {
			perror("s> Error aceptando la conexión de un cliente.\n");
			continue;
		}

		//Array de argumentos por si en un futuro son necesarios varios parámetros. Creamos el hilo para el cliente.
		int* arguments = malloc(sizeof(int) * 1);
		arguments[0] = client_sd;
		if(pthread_create(&client_thread, &client_attr, client_func, (void*) arguments) == -1) {
			perror("s> Error creando un thread para el cliente.\n");
			continue;
		}

		pthread_mutex_lock(&mutex);
		while(busy == TRUE) {
			pthread_cond_wait(&thread_ready, &mutex);
		}
		pthread_mutex_unlock(&mutex);

		busy = TRUE;
	}

	//Cerramos el descpritor del socket principal.
	close(server_sd);

	return 0;
}

void* client_func(void* p) {
	//Recuperamos los argumentos del proceso principal.
	pthread_mutex_lock(&mutex);
	int client_sd = *((int*) p);
	busy = FALSE;
	pthread_cond_signal(&thread_ready);
	pthread_mutex_unlock(&mutex);

	//Recuperamos la configuración del socket.
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	getpeername(client_sd, (struct sockaddr*) &client_addr, &addr_len);

	//Mostramos la ip y el puerto del cliente conectado.
	printf("s> accept %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	//Estadísticas.
	unsigned int stats[STAT_CODES] = {0,0,0,0,0};

	//Creamos un array de bytes para almacenar la información recibida/transferida al cliente.
	char bytes[SOCK_BUFFER];
	ssize_t bytes_read, bytes_written;
	while(TRUE) {
		//Esperamos una petición del cliente.
		if((bytes_read = read(client_sd, bytes, 1)) == -1) {
			perror("[ERROR] Fallo al recibir datos del cliente.\n");
			break;
		}
		else if(bytes_read == 0) {
			printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			close(client_sd);
		
			pthread_exit(EXIT_SUCCESS);
		}

		//Manejamos la petición del cliente.
		if(bytes[0] == CODE_EXIT) {
			//Desconexión del cliente.
			printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			break;
		}
		else if(bytes[0] == CODE_PING) {
			//Contador de estadísticas.
			stats[0]++;

			//Petición PING.
			printf("s> %s:%d ping\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			//Respondemos a la petición de PING.
			char byte[1];
			byte[0] = CODE_PING;
			if((bytes_written = write(client_sd, byte, 1)) == -1) {
				perror("[ERROR] Fallo al enviar datos al cliente.\n");
				break;
			}
		}
		else if(bytes[0] == CODE_SWAP) {
			//Contador de estadísticas.
			stats[1]++;

			if((bytes_read = read(client_sd, bytes + 1, sizeof(struct swap) - 1)) == -1) {
				perror("[ERROR] Fallo al recibir datos del cliente.\n");
				break;
			}
			else if(bytes_read == 0) {
				printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				close(client_sd);
			
				pthread_exit(EXIT_SUCCESS);
			}

			struct swap *petition = (struct swap *) &bytes;

			//Petición de intercambio de letras.
			printf("s> %s:%d init swap %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), ntohl(petition->total_length));

			//Esperamos "iter" paquetes del cliente.
			unsigned int iter = ntohl(petition->total_iter);
			unsigned int i, swapped_global = 0;
			for(i = 0; i < iter; i++) {
				//Esperamos una petición del cliente.
				if((bytes_read = read(client_sd, bytes, sizeof(struct swap_iter))) == -1) {
					perror("[ERROR] Fallo al recibir datos del cliente.\n");
					break;
				}
				else if(bytes_read == 0) {
					printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					close(client_sd);

					pthread_exit(EXIT_SUCCESS);
				}
				struct swap_iter *swap = (struct swap_iter *) &bytes;

				//Comprobar si la id es correcta.
				if(ntohl(swap->id) != i) {
					printf("s> %s:%d swap error id[%u] != %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), i, ntohl(swap->id));
				}

				//Swap chars.
				unsigned int j, swapped_local = 0;
				for(j = 0; j < ntohl(swap->text_length); j++) {
					if(swap->text[j] != tolower(swap->text[j])) {
						swap->text[j] = tolower(swap->text[j]);
						swapped_local++;
					}
					else if(swap->text[j] != toupper(swap->text[j])) {
						swap->text[j] = toupper(swap->text[j]);
						swapped_local++;
					}
				}
				swapped_global += swapped_local;

				//Actualizamos el número de swaps.
				swap->swapped = htonl(swapped_local);
				swap->swapped_global = htonl(swapped_global);

				//(DEBUG) Información sobre la iteración del swap.
				if(debug) {
					printf("s> %s:%d swap iter[%d] %u/%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), i, swapped_local, ntohl(swap->text_length));
				}

				//Mandar la respuesta de swap al cliente.
				if((bytes_written = write(client_sd, (char*) swap, sizeof(struct swap_iter))) == -1) {
					perror("[ERROR] Fallo al enviar datos al cliente.\n");
					break;
				}
			}

			//Respuesta de intercambio de letras.
			printf("s> %s:%d swap = %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), swapped_global);
		}
		else if(bytes[0] == CODE_HASH) {
			//Contador de estadísticas.
			stats[2]++;

			if((bytes_read = read(client_sd, bytes + 1, sizeof(struct hash) - 1)) == -1) {
				perror("[ERROR] Fallo al recibir datos del cliente.\n");
				break;
			}
			else if(bytes_read == 0) {
				printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				close(client_sd);
			
				pthread_exit(EXIT_SUCCESS);
			}

			struct hash *petition = (struct hash *) &bytes;

			//Petición de intercambio de letras.
			printf("s> %s:%d init hash %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), ntohl(petition->total_length));

			//Crear estructura de respuesta.
			struct hash_iter_res response;
			response.code = CODE_HASH_ITER_RES;

			//Esperamos "iter" paquetes del cliente.
			unsigned int iter = ntohl(petition->total_iter);
			unsigned int i, hash_global = 0;
			for(i = 0; i < iter; i++) {
				//Esperamos una petición del cliente.
				if((bytes_read = read(client_sd, bytes, sizeof(struct hash_iter_pet))) == -1) {
					perror("[ERROR] Fallo al recibir datos del cliente.\n");
					break;
				}
				else if(bytes_read == 0) {
					printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					close(client_sd);

					pthread_exit(EXIT_SUCCESS);
				}
				struct hash_iter_pet *hash = (struct hash_iter_pet *) &bytes;

				//Comprobar si la id es correcta.
				if(ntohl(hash->id) != i) {
					printf("s> %s:%d hash error id[%u] != %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), i, ntohl(hash->id));
				}

				//Swap chars.
				unsigned int j, hash_local = 0;
				for(j = 0; j < ntohl(hash->text_length); j++) {
					hash_local = hash_local + hash->text[j] % HASH_MOD;
				}
				hash_global = (hash_global + hash_local) % HASH_MOD;

				//(DEBUG) Información sobre la iteración del swap.
				if(debug) {
					printf("s> %s:%d hash iter[%d] %u (%u)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), i, hash_local, ntohl(hash->text_length));
				}

				//Answer client.
				response.id = htonl(i);
				response.hash = htonl(hash_local);
				response.hash_global = htonl(hash_global);

				//Mandar la respuesta de swap al cliente.
				if((bytes_written = write(client_sd, (char*) &response, sizeof(struct hash_iter_res))) == -1) {
					perror("[ERROR] Fallo al enviar datos al cliente.\n");
					break;
				}
			}

			//Respuesta de intercambio de letras.
			printf("s> %s:%d hash = %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), hash_global);
		}
		else if(bytes[0] == CODE_CHECK) {
			//Contador de estadísticas.
			stats[3]++;

			if((bytes_read = read(client_sd, bytes + 1, sizeof(struct check) - 1)) == -1) {
				perror("[ERROR] Fallo al recibir datos del cliente.\n");
				break;
			}
			else if(bytes_read == 0) {
				printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				close(client_sd);
			
				pthread_exit(EXIT_SUCCESS);
			}

			struct check *petition = (struct check *) &bytes;
			unsigned int client_hash = ntohl(petition->hash);

			//Petición de intercambio de letras.
			printf("s> %s:%d init check %u %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), ntohl(petition->total_length), ntohl(petition->hash));

			//Crear estructura de respuesta.
			struct check_iter_res response;
			response.code = CODE_CHECK_ITER_RES;
			response.check = CHECK_FAIL;

			//Esperamos "iter" paquetes del cliente.
			unsigned int iter = ntohl(petition->total_iter);
			unsigned int i, hash_global = 0;
			for(i = 0; i < iter; i++) {
				//Esperamos una petición del cliente.
				if((bytes_read = read(client_sd, bytes, sizeof(struct check_iter_pet))) == -1) {
					perror("[ERROR] Fallo al recibir datos del cliente.\n");
					break;
				}
				else if(bytes_read == 0) {
					printf("s> %s:%d quit\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					close(client_sd);

					pthread_exit(EXIT_SUCCESS);
				}
				struct check_iter_pet *check = (struct check_iter_pet *) &bytes;

				//Comprobar si la id es correcta.
				if(ntohl(check->id) != i) {
					printf("s> %s:%d check error id[%u] != %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), i, ntohl(check->id));
				}

				//Swap chars.
				unsigned int j, hash_local = 0;
				for(j = 0; j < ntohl(check->text_length); j++) {
					hash_local = hash_local + check->text[j] % HASH_MOD;
				}
				hash_global = (hash_global + hash_local) % HASH_MOD;

				//(DEBUG) Información sobre la iteración del swap.
				if(debug) {
					printf("s> %s:%d check iter[%d] %u (%u)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), i, hash_local, ntohl(check->text_length));
				}

				//Answer client.
				response.id = htonl(i);
				response.hash = htonl(hash_local);
				response.hash_global = htonl(hash_global);

				if(i == iter - 1 && hash_global == client_hash) {
					response.check = CHECK_OK;
				}

				//Mandar la respuesta de swap al cliente.
				if((bytes_written = write(client_sd, (char*) &response, sizeof(struct check_iter_res))) == -1) {
					perror("[ERROR] Fallo al enviar datos al cliente.\n");
					break;
				}
			}

			char *check_string;
			if(response.check == CHECK_OK) {
				check_string = CHECK_STRING_OK;
			}
			else if(response.check == CHECK_FAIL) {
				check_string = CHECK_STRING_FAIL;
			}

			//Respuesta de intercambio de letras.
			printf("s> %s:%d check = %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), check_string);
		}
		else if(bytes[0] == CODE_STAT) {
			//Iniciamos la operación de estadísticas.
			printf("s> %s:%d init stat\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			//Creamos la estructura de las estadísticas.
			struct client_stat cstat;
			cstat.code = CODE_STAT;
			cstat.ping = htonl(stats[0]);
			cstat.swap = htonl(stats[1]);
			cstat.hash = htonl(stats[2]);
			cstat.check = htonl(stats[3]);
			cstat.stat = htonl(stats[4]);

			//Mandar la respuesta de stat al cliente.
			ssize_t socket_bytes_written;
			if((socket_bytes_written = write(client_sd, (char*) &cstat, sizeof(struct client_stat))) == -1) {
				perror("Error transfiriendo datos al cliente.\n");
				break;
			}

			//Finalizamos la operación de estadísticas.
			printf("s> %s:%d stat = %u %u %u %u %u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), stats[0], stats[1], stats[2], stats[3], stats[4]);

			//Contador de estadísticas.
			stats[4]++;
		}

		memset(bytes, 0, sizeof(bytes));
	}

	close(client_sd);

	pthread_exit(0);
}
