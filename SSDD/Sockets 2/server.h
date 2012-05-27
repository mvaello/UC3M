#ifndef SERVER_H
#define SERVER_H 1

#include <signal.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_QUEUE_CONN 5

void* client_func(void* p);

#endif
