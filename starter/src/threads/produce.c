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
int producerSize;
int bufferSize;
int numberOfItemToProduce;
sem_t spaces;
sem_t items;
pthread_mutex_t mutex;
int pindex = 0;
int cindex = 0;

void *producer(void* arg);
void* consumer(void *arg);

int main(int argc, char *argv[])
{
	int num;
	int maxmsg;
	int num_p;
	int num_c;
	//int i;
	struct timeval tv;

	int i;
	int j;
	int k;

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
	numberOfItemToProduce = num;

	buffer = malloc(maxmsg*sizeof(int));

	sem_init(&spaces, 0, maxmsg);
	sem_init(&items, 0, 0);
	pthread_mutex_init(&mutex, NULL);

	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	pthread_t threads[num_p+num_c];

	for (i = 0; i < num_p; i++) {
		int* id = malloc(sizeof(int));
		*id = i;
		pthread_create(&threads[i], NULL, producer, id);
	}

	for (j = num_p; j < num_p+num_c; j++) {
		int* jd = malloc(sizeof(int));
		*jd = j-num_p;
		pthread_create(&threads[j], NULL, consumer, jd);
	}

	for (k = 0; k < num_p + num_c; k++) {
		pthread_join(threads[k], NULL);
	}

	sem_destroy(&spaces);
	sem_destroy(&items);
	pthread_mutex_destroy(&mutex);

	free(buffer);


    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);

	exit(0);
}


void* producer (void *arg) {
	int *id = (int *) arg;
	int i;

	for (i=0; i < numberOfItemToProduce; i ++) {
		if (i%producerSize == *id) {
			sem_wait(&spaces);
			pthread_mutex_lock(&mutex);
			buffer[pindex%bufferSize] = i;
			pindex ++;
			pthread_mutex_unlock(&mutex);
			sem_post(&items);
		}
	}
	free(arg);
	pthread_exit(NULL);
}

void* consumer(void *arg) {
	int *id = (int *) arg;
	int num;
	double squareRoot;
	while (1) {
		sem_wait(&items);
		pthread_mutex_lock(&mutex);
		num = buffer[cindex%bufferSize];
		cindex ++;

		if (cindex > numberOfItemToProduce-1) {
			break;
		}

		pthread_mutex_unlock(&mutex);
		sem_post(&spaces);
		squareRoot = sqrt(number);
		if (squareRoot == (int) squareRoot) {
			printf(" %d %d %d \n", id, number, (int) squareRoot);
		}
	}

	sem_post(&spaces);
	sem_post(&items);
	pthread_mutex_unlock(&mutex);

	free(arg);
	pthread_exit(NULL);
}

