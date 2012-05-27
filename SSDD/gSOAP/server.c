#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>

#include "soapH.h" /* get the gSOAP-generated definitions */
#include "ns.nsmap" /* get the gSOAP-generated namespace bindings */

#include "common.h"
#include "server.h"

int debug = 0;

//Control de threads.
int busy = TRUE;
pthread_mutex_t mutex;
pthread_cond_t thread_ready;

//Estadísticas.
unsigned int stats[STAT_CODES] = {0,0,0,0,0};
pthread_mutex_t stats_mutex[STAT_CODES] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

void usage(char *program_name) {
	printf("Usage: %s [-d] -p <port>\n", program_name);
}

char* num2ip(unsigned long num) {
	char* ip = (char *) malloc(16);
	sprintf(ip, "%d.%d.%d.%d", (int)((num>>24) & 0xFF), (int)((num >> 16) & 0xFF), (int) ((num >> 8) &0xFF), (int) (num & 0xFF));
	return ip;
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

	//Entorno de ejecución del servidor.
	SOAP_SOCKET m, s; 
	struct soap soap;

	//Inicializar un entorno de ejecución.
	soap_init(&soap);

	//Configurar SOAP.
	soap.send_timeout = 60; // 60 seconds
	soap.recv_timeout = 60; // 60 seconds
	soap.accept_timeout = 3600; // server stops after 1 hour of inactivity
	soap.max_keep_alive = 100; // max keep-alive sequence

	//Creamos un identificador de threads.
	pthread_t client_thread;

	//Creamos e inicializamos los atributos de los threads que los clientes van a usar.
	pthread_attr_t client_attr;
	pthread_attr_init(&client_attr);
	pthread_attr_setdetachstate(&client_attr, PTHREAD_CREATE_DETACHED);

	//Mutex y señal.
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&thread_ready, NULL);

	//Activar un servicio web
	m = soap_bind(&soap, NULL, port, BACKLOG);
	if(!soap_valid_socket(m)) {
		soap_print_fault(&soap, stderr);
		exit(1);
	}

	//Informamos de que se ha creado correctamente el bind del servidor.
	printf("s> init server %s:%d\n","http://0.0.0.0", port);

	//Informamos de que el servidor está esperando.
	printf("s> waiting\n");

	while (1) {
		//Esperamos la conexión de un cliente.
		s = soap_accept(&soap);
		if(!soap_valid_socket(s)) {
			if(soap.errnum) {
				soap_print_fault(&soap, stderr);
				exit(1);
			}

			fprintf(stderr, "El servidor se ha cerrado.\n");
			break;
		}

		//Mostramos la ip y el puerto del cliente conectado.
		printf("s> accept %s:%d\n", num2ip(soap.ip), soap.port);

		//Crear proceso ligero.
		if(pthread_create(&client_thread, &client_attr, thread_fun, (void*) &soap) == -1) {
			perror("Error creando un thread para el cliente.\n");
			continue;
		}

		//Esperar a que el hilo copie el soap.
		pthread_mutex_lock(&mutex);
		while(busy == TRUE) {
			pthread_cond_wait(&thread_ready, &mutex);
		}
		pthread_mutex_unlock(&mutex);
		busy = TRUE;
	}

	soap_done(&soap);
	exit(0);
}

void *thread_fun(void *soap) {
	//Recuperamos los argumentos del proceso principal.
	struct soap *thread_soap;
	pthread_mutex_lock(&mutex);
	thread_soap = soap_copy((struct soap *) soap);
	if(!thread_soap) {
		printf("Error copiando servicio en el thread.\n");
		return NULL; 
	}
	busy = FALSE;
	pthread_cond_signal(&thread_ready);
	pthread_mutex_unlock(&mutex);

	//Servir funciones del soap.
	soap_serve(thread_soap);

	//Destruir servicio.
	soap_destroy(thread_soap);
	soap_end(thread_soap);
	soap_done(thread_soap);
	free(thread_soap);

	return NULL;
}

int ns__ping(struct soap *soap, char in, char *out) {
	//Contador de estadísticas.
	pthread_mutex_lock(&stats_mutex[0]);
	stats[0]++;
	pthread_mutex_unlock(&stats_mutex[0]);

	//Petición PING.
	printf("s> %s:%d ping\n", num2ip(soap->ip), soap->port);
	if(in == CODE_PING) {
		*out = CODE_PING;
		return SOAP_OK;
	}
	else {
		*out = CODE_PING;
		return -1;
	}
}
int ns__swap(struct soap *soap, struct swap_data in, struct swap_data *out) {
	//Nueva petición de intercambio de letras.
	if(ntohl(in.iter) == 0) {
		//Contador de estadísticas.
		pthread_mutex_lock(&stats_mutex[1]);
		stats[1]++;
		pthread_mutex_unlock(&stats_mutex[1]);

		printf("s> %s:%d init swap %u\n", num2ip(soap->ip), soap->port, ntohl(in.total_length));
	}

	//Copiamos información de la petición a la respuesta.
	out->iter = in.iter;
	out->total_iter = in.total_iter;
	out->total_length = in.total_length;
	out->text_length = in.text_length;

	//Swap chars.
	unsigned int i, swapped_local = 0;
	for(i = 0; i < ntohl(in.text_length); i++) {
		if(in.text[i] != tolower(in.text[i])) {
			out->text[i] = tolower(in.text[i]);
			swapped_local++;
		}
		else if(in.text[i] != toupper(in.text[i])) {
			out->text[i] = toupper(in.text[i]);
			swapped_local++;
		}
		else {
			out->text[i] = in.text[i];
		}
	}

	//Actualizamos el número de swaps.
	out->swapped_local = htonl(swapped_local);
	out->swapped = htonl(swapped_local + ntohl(in.swapped));

	//(DEBUG) Información sobre la iteración del swap.
	if(debug) {
		printf("s> %s:%d swap iter[%d] %u/%u\n", num2ip(soap->ip), soap->port, ntohl(in.iter), swapped_local, ntohl(in.text_length));
	}

	//Resultado de swap.
	if(ntohl(out->iter) == ntohl(out->total_iter) - 1) {
		printf("s> %s:%d swap = %u\n", num2ip(soap->ip), soap->port, ntohl(out->swapped));
	}

	return SOAP_OK;
}
int ns__hash(struct soap *soap, struct hash_pet in, struct hash_res *out) {
	//Nueva petición de hash.
	if(ntohl(in.iter) == 0) {
		//Contador de estadísticas.
		pthread_mutex_lock(&stats_mutex[2]);
		stats[2]++;
		pthread_mutex_unlock(&stats_mutex[2]);

		printf("s> %s:%d init hash %u\n", num2ip(soap->ip), soap->port, ntohl(in.total_length));
	}

	//Copiamos información de la petición a la respuesta.
	out->iter = in.iter;
	out->total_iter = in.total_iter;
	out->total_length = in.total_length;

	//Hash chars.
	unsigned int i, hash_local = 0;
	for(i = 0; i < ntohl(in.text_length); i++) {
		hash_local = hash_local + in.text[i] % HASH_MOD;
	}

	//Actualizamos el hash.
	out->hash_local = htonl(hash_local);
	out->hash = htonl((hash_local + ntohl(in.hash)) % HASH_MOD);

	//(DEBUG) Información sobre la iteración del hash.
	if(debug) {
		printf("s> %s:%d hash iter[%d] %u (%u)\n", num2ip(soap->ip), soap->port, ntohl(in.iter), hash_local, ntohl(in.text_length));
	}

	//Resultado de hash.
	if(ntohl(out->iter) == ntohl(out->total_iter) - 1) {
		printf("s> %s:%d hash = %u\n", num2ip(soap->ip), soap->port, ntohl(out->hash));
	}

	return SOAP_OK;
}
int ns__check(struct soap *soap, struct check_pet in, struct check_res *out) {
	//Nueva petición de check.
	if(ntohl(in.iter) == 0) {
		//Contador de estadísticas.
		pthread_mutex_lock(&stats_mutex[3]);
		stats[3]++;
		pthread_mutex_unlock(&stats_mutex[3]);

		printf("s> %s:%d init check %u\n", num2ip(soap->ip), soap->port, ntohl(in.total_length));
	}

	//Copiamos información de la petición a la respuesta.
	out->iter = in.iter;
	out->total_iter = in.total_iter;
	out->total_length = in.total_length;
	out->hash_client = in.hash_client;

	//Hash chars.
	unsigned int i, hash_local = 0;
	for(i = 0; i < ntohl(in.text_length); i++) {
		hash_local = hash_local + in.text[i] % HASH_MOD;
	}

	//Actualizamos el hash.
	out->hash_local = htonl(hash_local);
	out->hash = htonl((hash_local + ntohl(in.hash)) % HASH_MOD);

	//(DEBUG) Información sobre la iteración del check.
	if(debug) {
		printf("s> %s:%d check iter[%d] %u (%u)\n", num2ip(soap->ip), soap->port, ntohl(in.iter), hash_local, ntohl(in.text_length));
	}

	//Resultado de check.
	if(ntohl(out->iter) == ntohl(out->total_iter) - 1) {
		if(out->hash == out->hash_client) {
			out->check = CHECK_OK;
		}
		else {
			out->check = CHECK_FAIL;
		}

		char *check_string;
		if(out->check == CHECK_OK) {
			check_string = CHECK_STRING_OK;
		}
		else if(out->check == CHECK_FAIL) {
			check_string = CHECK_STRING_FAIL;
		}

		printf("s> %s:%d check = %s\n", num2ip(soap->ip), soap->port, check_string);
	}

	return SOAP_OK;
}
int ns__stat(struct soap *soap, char in, struct server_stat *out) {
	//Petición de estadísticas.
	printf("s> %s:%d init stat\n", num2ip(soap->ip), soap->port);

	//Copiar estadísticas.
	pthread_mutex_lock(&stats_mutex[0]);
	out->ping = htonl(stats[0]);
	pthread_mutex_unlock(&stats_mutex[0]);
	pthread_mutex_lock(&stats_mutex[1]);
	out->swap = htonl(stats[1]);
	pthread_mutex_unlock(&stats_mutex[1]);
	pthread_mutex_lock(&stats_mutex[2]);
	out->hash = htonl(stats[2]);
	pthread_mutex_unlock(&stats_mutex[2]);
	pthread_mutex_lock(&stats_mutex[3]);
	out->check = htonl(stats[3]);
	pthread_mutex_unlock(&stats_mutex[3]);
	pthread_mutex_lock(&stats_mutex[4]);
	out->stat = htonl(stats[4]);
	pthread_mutex_unlock(&stats_mutex[4]);

	//Resultado de stat.
	printf("s> %s:%d stat = %u %u %u %u %u\n", num2ip(soap->ip), soap->port, ntohl(out->ping), ntohl(out->swap), ntohl(out->hash), ntohl(out->check), ntohl(out->stat));

	//Contador de estadísticas.
	pthread_mutex_lock(&stats_mutex[4]);
	stats[4]++;
	pthread_mutex_unlock(&stats_mutex[4]);

	return SOAP_OK;
}
int ns__quit(struct soap *soap, char in, char *out) {
	//Petición quit.
	printf("s> %s:%d quit\n", num2ip(soap->ip), soap->port);

	*out = CODE_QUIT;

	return SOAP_OK;
}
