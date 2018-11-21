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
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

double g_time[2];

void producer( int id );
void consumer( int id );

const char *MSG_QUEUE_NAME = "/int_buffer";

int num;
int maxmsg;
int num_p;
int num_c;

int main(int argc, char *argv[])
{
	int i;
	struct timeval tv;

	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	/* 
		Initialize The MSG Queue

		In this block we set our attributes, initialize and open the message queue.
		If the open call fails, we exit the program. 
	*/
    mqd_t qdes;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = maxmsg;
    attr.mq_msgsize = sizeof(int);

	// Open the msg queue
	qdes = mq_open(MSG_QUEUE_NAME, (O_CREAT | O_RDWR), (S_IRWXU | S_IRWXG | S_IRWXO), &attr);

	 // If queue creation failed
    if (qdes == -1) {
        printf("failed to open msg queue");
        return 1;
    }

	/* 
		Create Consumers & Producers

		In this block we want to create consumers and producers, consistant with the 
		input number of each. We call fork for each process and keep track of the 
		process id assigned to each. 
	*/

	// We want an array to store ids for producers and one for consumers. 
	pid_t producers[num_p];
	pid_t consumers[num_c];	

	// Create Producers
	for (i = 0; i < num_p; i++) {
		producers[i] = fork();
		if (producers[i] < 0) {
			printf("fork failed");
		} else if (producers[i] > 0) {
			// printf("Hello from parrent %ld \n", producers[i]);
		} else if (producers[i] == 0) {
			producer(i);
		}
	}

	// Create Consumers
	for (i = 0; i < num_c; i++) {
		consumers[i] = fork();
		if (consumers[i] < 0) {
			printf("fork failed");
		} else if (consumers[i] > 0) {
			// printf("Hello from parrent %ld \n", consumers[i]);
		} else if (consumers[i] == 0) {
			consumer(i);
		}
	}	

	// Wait for all producers to finish
    int* ret_val;
    for(i = 0; i < num_p; i++) {
        waitpid(producers[i], ret_val, 0);
    }

	    // Send -1 kill signals to the queue to terminate all consumers
    int kill = -1;
    for (i = 0; i < num_c; i++) {
        if (mq_send(qdes,(char *) &kill, sizeof(int), 0) == -1) {
            perror("Main - mq_send() failed");
            exit(1);
        }
    }

    // Wait for all consumers to finish
    for(i = 0; i < num_c; i++) {
        waitpid(consumers[i], ret_val, 0);
    }

    // Close the queue and do cleanup
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }

    if (mq_unlink(MSG_QUEUE_NAME) != 0) {
        perror("mq_unlink() failed");
        exit(3);
    }

    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
	
    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
	exit(0);
}

void producer( int id ) {
	/*
		Produce Data 

		Here we want to open the queue for writing and prepare the data. 
		Using the modulus method we determine whether or not the queue 
		will be written to. 
	*/
	mqd_t msgq = mq_open(MSG_QUEUE_NAME, O_WRONLY);
    if (msgq == -1) {
        printf("Producer - mq_open() failed");
        exit(1);
    }

	int i; 
	for(i = 0; i < num; i++) {
		if (i%num_p == id) {
			if (mq_send(msgq, (char *) &i, sizeof(int), 0) == -1) {
				printf("Message Sending Failed \n");
				exit(1);
			} 
		}
	}

	mq_close(msgq);
	exit(0);
}

void consumer( int id ) {
	mqd_t msgq = mq_open(MSG_QUEUE_NAME, O_RDWR);
    if (msgq == -1) {
        printf("Producer - mq_open() failed");
        exit(1);
    }

	int pt;
    int root;
    while (true){
        // If there's an error receiving from queue
        if (mq_receive(msgq, (char *) &pt, sizeof(int), NULL) == -1) {
            perror("mq_receive() failed");
            printf("Consumer: %d failed.\n", id);
            exit(1);
        } else {
            // Else the consumer read the point correctly

            // Encounters kill signal so break out of loop
            if (pt < 0) {
                break;
            }

            // Calculate and print sqroot
            root = sqrt(pt);
            if (root*root == pt) {
                printf("%d %d %d\n", id, pt, root);
            }
        }
    }

    // Close the queue and exit the process
    mq_close(msgq);
    exit(0);
}
