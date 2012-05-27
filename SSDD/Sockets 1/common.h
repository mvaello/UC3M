#ifndef COMMON_H
#define COMMON_H 1

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TRUE 1
#define FALSE 0

#define TCP 6

#define SOCK_BUFFER 1100
#define SWAP_BUFFER 1024
#define HASH_BUFFER 1024
#define CHECK_BUFFER 1024

#define CODE_EXIT 1
#define CODE_PING 2
#define CODE_SWAP 3
#define CODE_SWAP_ITER 4
#define CODE_HASH 5
#define CODE_HASH_ITER_PET 6
#define CODE_HASH_ITER_RES 7
#define CODE_CHECK 8
#define CODE_CHECK_ITER_PET 9
#define CODE_CHECK_ITER_RES 10
#define CODE_STAT 11
#define STAT_CODES 5

#define HASH_MOD 1000000

#define CHECK_OK 0
#define CHECK_FAIL 1
#define CHECK_STRING_LENGTH 5
#define CHECK_STRING_OK "OK"
#define CHECK_STRING_FAIL "FAIL"

struct swap {
	char code;
	unsigned int total_iter;
	unsigned int total_length;
};
struct swap_iter {
	char code;
	unsigned int id;
	unsigned int swapped;
	unsigned int swapped_global;
	unsigned int text_length;
	char text[SWAP_BUFFER];
};

struct hash {
	char code;
	unsigned int total_iter;
	unsigned int total_length;
};
struct hash_iter_pet {
	char code;
	unsigned int id;
	unsigned int text_length;
	char text[HASH_BUFFER];
};
struct hash_iter_res {
	char code;
	unsigned int id;
	unsigned int hash;
	unsigned int hash_global;
};

struct check {
	char code;
	unsigned int total_iter;
	unsigned int total_length;
	unsigned int hash;
};
struct check_iter_pet {
	char code;
	unsigned int id;
	unsigned int text_length;
	char text[CHECK_BUFFER];
};
struct check_iter_res {
	char code;
	char check;
	unsigned int id;
	unsigned int hash;
	unsigned int hash_global;
};

struct client_stat {
	char code;
	unsigned int ping;
	unsigned int swap;
	unsigned int hash;
	unsigned int check;
	unsigned int stat;
};

#endif
