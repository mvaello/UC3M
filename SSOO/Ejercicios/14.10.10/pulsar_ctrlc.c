#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int counter = 1;

void pressedCTRLC (int s) {
	printf("Ha pulsado %d veces CTRL+C", counter);
	counter++;
}

int main() {
	signal(SIGINT, pressedCTRLC);

	while(counter < 10) {
		sleep(10);
		printf("Fin del Sleep\n");
	}

	return 0;
}
