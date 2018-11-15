// Use this to see if a number has an integer square root
#define EPS 1.E-7


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

double g_time[2];

int main(int argc, char *argv[])
{
	int num;
	int maxmsg;
	int num_p;
	int num_c;
	int i;
	struct timeval tv;

	sem_t empty_list;
	sem_t full_list;


	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

	sem_init(&empty_list, 0, maxmsg);
	sem_init(&full_list, 0, 0);

	pthread_t producers[num_p];
	pthread_t consumers[num_c];

	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	for (int i = 0; i < num_p; i++) {
		//sem_wait()
		pthread_mutex_lock(&mutex);

		//sem_post()
	}


    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
	exit(0);
}

void* producer (void *arg) {
	int *id = (int *) arg;
	for(int i = 0l i < 10000; ++i) {

	}
	free(arg);
	pthread_exit(NULL);
}

void* consumer(void *arg) {
	int *id = (int *) arg;
	for(int i = 0; i < 10000; ++i) {

	}
	free(arg);
	pthread_exit(NULL);
}
