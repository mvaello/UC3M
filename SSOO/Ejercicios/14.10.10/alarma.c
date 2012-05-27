#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void alarmArrived (int s) {
	printf("Ha llegado la señal de alarma: %d\n", s);
}

main() {
	int i, rest;

	struct sigaction sa;
	sa.sa_handler = alarmArrived;
	sa.sa_flags = 0;

	sigaction(SIGALARM, &sa, NULL);

	alarm(3);

	for(i = 1; i <= 10; i++) {
		sleep(1);
		printf("Han pasado %d seg.\n", i);
		if(i % 4 == 0) {
			alarm(3);
		}
	}

	rest = alarm(0);

	printf("Faltaban %d seg para la siguiente alarma.\n", rest);
}
