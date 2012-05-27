#ifndef SERVER_H
#define SERVER_H 1

#include <pthread.h>

#define BACKLOG (100)

void *thread_fun(void *soap);

#endif
