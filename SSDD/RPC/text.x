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

program SSDD_TEXT {
	version FINAL {
		char ping_rpc(char byte) = 1;
		struct swap_data swap_rpc(struct swap_data data) = 2;
		struct hash_res hash_rpc(struct hash_pet data) = 3;
		struct check_res check_rpc(struct check_pet data) = 4;
		struct server_stat stat_rpc(char byte) = 5;
		char quit_rpc(char byte) = 6;
	} = 1;
} = 0x20000001;

