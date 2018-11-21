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
#include <semaphore.h>
#include <pthread.h>

double g_time[2];


int *buffer;
int *lastProduced;
int producerSize;
int bufferSize;
sem_t spaces;
sem_t items;
pthread_mutex_t mutex;
int pindex = 0;
int cindex = 0;

int main(int argc, char *argv[])
{
	int num;
	int maxmsg;
	int num_p;
	int num_c;
	int i;
	struct timeval tv;

	sem_t spaces;
	sem_t items;


	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}


	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

	producerSize = num_p;
	bufferSize = maxmsg;

	buffer = malloc(maxmsg*sizeof(int));
	lastProduced = malloc(num_p*sizeof(int));

	for (int i = 0; i < producerSize; i ++) {
		lastProducer[i] = 0;
	}

	sem_init(&spaces, 0, maxmsg);
	sem_init(&items, 0, 0);
	pthread_mutex_init(&mutex, NULL);

	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	pthread_t threads[num_p+num_c];

	for (int i = 0; i < num_p; i++) {
		int* id = malloc(sizeof(int));
		*id = i;
		pthread_create(&threads[i], NULL, producer, id);
	}

	for (int j = num_p; j < num_p+num_c; j++) {
		int* jd = malloc(sizeof(int));
		*jd = j-num_p;
		pthread_create(&threads[j], NULL, consumer, jd);
	}

	for (int k = 0; k < num_p + num_c; k++) {
		pthread_join(threads[k], NULL);
	}

	sem_destroy(&spaces);
	sem_destroy(&items);
	pthread_mutex_destory(&mutex);


    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
	pthread_exit(0);
	exit(0);
}

void* producer (void *arg) {
	int *id = (int *) arg;
	while (1) {
		int num = produce(*id);
		sem_wait(&spaces);
		pthread_mutex_lock(&mutex);
		*buffer[pindex] = num;
		pindex = (pindex + 1) % bufferSize;
		pthread_mutex_unlock(&mutex);
		sem_post(&items);
	}
	free(arg);
	pthread_exit(NULL);
}

void* consumer(void *arg) {
	int *id = (int *) arg;
	while (1) {
		sem_wait(&items);
		pthread_mutex_lock(&mutex);
		int num = buffer[cindex];
		*buffer[cindex] = -1;
		cindex = (cindex + 1) % bufferSize;
		pthread_mutex_unlock(&mutex);
		sem_post(&spaces);
		consume(*id, num);
	}
	free(arg);
	pthread_exit(NULL);
}

int produce(int id) {
	int num = lastProduced[id];
	if (num == 0) {
		lastProduced[id] = id;
	} else {
		lastProduced[id] = lastProduced[id] + producerSize;
	}
	return lastProduced[id];
}

int consume(int id, int number) {
	double squareRoot = sqrt(number);
	if (squareRoot == (int) squareRoot) {
		printf(" %d %d %d", id, number, squareRoot)
	}
}