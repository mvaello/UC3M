#include <stdio.h> /* printf */
#include <stdlib.h> /* fgets */
#include <string.h> /* atoi, strchr */
#include <pthread.h> /* pthread */
#include <unistd.h> /* sleep */
#include <time.h> /* time */

#define TRUE			1
#define FALSE			0

#define MAXSLEEP		10
#define MINSLEEP		1

int parkingInit(int parkingFloors, int parkingPlaces);
int getMaxPlaces(int parkingFloors, int parkingPlaces);
int parkCar();
void removeCar(int floor);

int readNumber();
int generateRandom(int min, int max);

void* car(void *p);
void* parking();
