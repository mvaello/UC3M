#include "common.h"
#include "server.h"

//Estadísticas.
unsigned int stats[STAT_CODES] = {0,0,0,0,0};
pthread_mutex_t stats_mutex[STAT_CODES] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

//Servicio de PING.
bool_t ping_rpc_1_svc(char* byte, char *result, struct svc_req *rqstp) {
	//Contador de estadísticas.
	pthread_mutex_lock(&stats_mutex[0]);
	stats[0]++;
	pthread_mutex_unlock(&stats_mutex[0]);

	//Petición de ping.
	struct sockaddr_in *client_addr = svc_getcaller(rqstp->rq_xprt);
	printf("s> %s:%d ping\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));

	if(*byte == CODE_PING) {
		*result = CODE_PING;
		return TRUE;
	}
	else {
		*result = CODE_ERROR;
		return FALSE;
	}
}

//Servicio de SWAP.
bool_t swap_rpc_1_svc(struct swap_data* data_in, struct swap_data *data_out, struct svc_req *rqstp) {
	//Petición de intercambio de letras.
	struct sockaddr_in *client_addr = svc_getcaller(rqstp->rq_xprt);
	if(ntohl(data_in->iter) == 0) {
		//Contador de estadísticas.
		pthread_mutex_lock(&stats_mutex[1]);
		stats[1]++;
		pthread_mutex_unlock(&stats_mutex[1]);

		printf("s> %s:%d init swap %u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_in->total_length));
	}

	//Copiamos información de la petición a la respuesta.
	data_out->iter = data_in->iter;
	data_out->total_iter = data_in->total_iter;
	data_out->total_length = data_in->total_length;
	data_out->text_length = data_in->text_length;

	//Swap chars.
	unsigned int i, swapped_local = 0;
	for(i = 0; i < ntohl(data_in->text_length); i++) {
		if(data_in->text[i] != tolower(data_in->text[i])) {
			data_out->text[i] = tolower(data_in->text[i]);
			swapped_local++;
		}
		else if(data_in->text[i] != toupper(data_in->text[i])) {
			data_out->text[i] = toupper(data_in->text[i]);
			swapped_local++;
		}
		else {
			data_out->text[i] = data_in->text[i];
		}
	}

	//Actualizamos el número de swaps.
	data_out->swapped_local = htonl(swapped_local);
	data_out->swapped = htonl(swapped_local + ntohl(data_in->swapped));

	//(DEBUG) Información sobre la iteración del swap.
	if(DEBUG) {
		printf("s> %s:%d swap iter[%d] %u/%u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_in->iter), swapped_local, ntohl(data_in->text_length));
	}

	//Resultado de swap.
	if(ntohl(data_out->iter) == ntohl(data_out->total_iter) - 1) {
		printf("s> %s:%d swap = %u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_out->swapped));
	}

	return TRUE;
}

//Servicio de HASH.
bool_t hash_rpc_1_svc(struct hash_pet *data_in, struct hash_res *data_out, struct svc_req *rqstp) {
	//Petición de hash.
	struct sockaddr_in *client_addr = svc_getcaller(rqstp->rq_xprt);
	if(ntohl(data_in->iter) == 0) {
		//Contador de estadísticas.
		pthread_mutex_lock(&stats_mutex[2]);
		stats[2]++;
		pthread_mutex_unlock(&stats_mutex[2]);

		printf("s> %s:%d init hash %u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_in->total_length));
	}

	//Copiamos información de la petición a la respuesta.
	data_out->iter = data_in->iter;
	data_out->total_iter = data_in->total_iter;
	data_out->total_length = data_in->total_length;

	//Hash chars.
	unsigned int i, hash_local = 0;
	for(i = 0; i < ntohl(data_in->text_length); i++) {
		hash_local = hash_local + data_in->text[i] % HASH_MOD;
	}

	//Actualizamos el hash.
	data_out->hash_local = htonl(hash_local);
	data_out->hash = htonl((hash_local + ntohl(data_in->hash)) % HASH_MOD);

	//(DEBUG) Información sobre la iteración del hash.
	if(DEBUG) {
		printf("s> %s:%d hash iter[%d] %u (%u)\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_in->iter), hash_local, ntohl(data_in->text_length));
	}

	//Resultado de hash.
	if(ntohl(data_out->iter) == ntohl(data_out->total_iter) - 1) {
		printf("s> %s:%d hash = %u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_out->hash));
	}

	return TRUE;
}

//Servicio de CHECK.
bool_t check_rpc_1_svc(struct check_pet *data_in, struct check_res *data_out, struct svc_req *rqstp) {
	//Petición check.
	struct sockaddr_in *client_addr = svc_getcaller(rqstp->rq_xprt);
	if(ntohl(data_in->iter) == 0) {
		//Contador de estadísticas.
		pthread_mutex_lock(&stats_mutex[3]);
		stats[3]++;
		pthread_mutex_unlock(&stats_mutex[3]);

		printf("s> %s:%d init check %u %u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_in->total_length), ntohl(data_in->hash_client));
	}

	//Copiamos información de la petición a la respuesta.
	data_out->iter = data_in->iter;
	data_out->total_iter = data_in->total_iter;
	data_out->total_length = data_in->total_length;
	data_out->hash_client = data_in->hash_client;

	//Hash chars.
	unsigned int i, hash_local = 0;
	for(i = 0; i < ntohl(data_in->text_length); i++) {
		hash_local = hash_local + data_in->text[i] % HASH_MOD;
	}

	//Actualizamos el hash.
	data_out->hash_local = htonl(hash_local);
	data_out->hash = htonl((hash_local + ntohl(data_in->hash)) % HASH_MOD);

	//(DEBUG) Información sobre la iteración del check.
	if(DEBUG) {
		printf("s> %s:%d check iter[%d] %u (%u)\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_in->iter), hash_local, ntohl(data_in->text_length));
	}

	//Resultado de check.
	if(ntohl(data_out->iter) == ntohl(data_out->total_iter) - 1) {
		if(data_out->hash == data_out->hash_client) {
			data_out->check = CHECK_OK;
		}
		else {
			data_out->check = CHECK_FAIL;
		}

		char *check_string;
		if(data_out->check == CHECK_OK) {
			check_string = CHECK_STRING_OK;
		}
		else if(data_out->check == CHECK_FAIL) {
			check_string = CHECK_STRING_FAIL;
		}

		printf("s> %s:%d check = %s\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), check_string);
	}

	return TRUE;
}

//Servicio de STAT.
bool_t stat_rpc_1_svc(char* byte, struct server_stat *data_out, struct svc_req *rqstp) {
	//Petición de estadísticas.
	struct sockaddr_in *client_addr = svc_getcaller(rqstp->rq_xprt);
	printf("s> %s:%d init stat\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));

	//Respuesta.
	pthread_mutex_lock(&stats_mutex[0]);
	data_out->ping = htonl(stats[0]);
	pthread_mutex_unlock(&stats_mutex[0]);
	pthread_mutex_lock(&stats_mutex[1]);
	data_out->swap = htonl(stats[1]);
	pthread_mutex_unlock(&stats_mutex[1]);
	pthread_mutex_lock(&stats_mutex[2]);
	data_out->hash = htonl(stats[2]);
	pthread_mutex_unlock(&stats_mutex[2]);
	pthread_mutex_lock(&stats_mutex[3]);
	data_out->check = htonl(stats[3]);
	pthread_mutex_unlock(&stats_mutex[3]);
	pthread_mutex_lock(&stats_mutex[4]);
	data_out->stat = htonl(stats[4]);
	pthread_mutex_unlock(&stats_mutex[4]);

	//Resultado de stat.
	printf("s> %s:%d stat = %u %u %u %u %u\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(data_out->ping), ntohl(data_out->swap), ntohl(data_out->hash), ntohl(data_out->check), ntohl(data_out->stat));

	//Contador de estadísticas.
	pthread_mutex_lock(&stats_mutex[4]);
	stats[4]++;
	pthread_mutex_unlock(&stats_mutex[4]);

	return TRUE;
}

//Servicio de QUIT.
bool_t quit_rpc_1_svc(char* byte, char *result, struct svc_req *rqstp) {
	//Petición de salida.
	struct sockaddr_in *client_addr = svc_getcaller(rqstp->rq_xprt);
	printf("s> %s:%d quit\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));

	*result = CODE_QUIT;

	return TRUE;
}

//Liberar memoria de los resultados.
int ssdd_text_1_freeresult(SVCXPRT *svcx_prt, xdrproc_t xdr_proc, caddr_t c_addr) {
	xdr_free(xdr_proc, c_addr);
	return 1;
}

