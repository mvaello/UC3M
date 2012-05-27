//Estructuras de datos.
struct swap_data {
	unsigned int total_iter;
	unsigned int total_length;
	unsigned int iter;
	unsigned int swapped;
	unsigned int swapped_local;
	unsigned int text_length;
	char text[1024];
};
struct hash_pet {
	unsigned int total_iter;
	unsigned int total_length;
	unsigned int iter;
	unsigned int hash;
	unsigned int text_length;
	char text[1024];
};
struct hash_res {
	unsigned int total_iter;
	unsigned int total_length;
	unsigned int iter;
	unsigned int hash;
	unsigned int hash_local;
};
struct check_pet {
	unsigned int total_iter;
	unsigned int total_length;
	unsigned int iter;
	unsigned int hash;
	unsigned int hash_client;
	unsigned int text_length;
	char text[1024];
};
struct check_res {
	char check;
	unsigned int total_iter;
	unsigned int total_length;
	unsigned int iter;
	unsigned int hash;
	unsigned int hash_local;
	unsigned int hash_client;
};
struct server_stat {
	unsigned int ping;
	unsigned int swap;
	unsigned int hash;
	unsigned int check;
	unsigned int stat;
};

//Funciones del servicio.
int ns__ping(char in, char *out);
int ns__swap(struct swap_data in, struct swap_data *out);
int ns__hash(struct hash_pet in, struct hash_res *out);
int ns__check(struct check_pet in, struct check_res *out);
int ns__stat(char in, struct server_stat *out);
int ns__quit(char in, char *quit);
