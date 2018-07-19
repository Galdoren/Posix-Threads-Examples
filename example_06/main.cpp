#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* compile with g++ main.cpp -lpthread -o example.out */

#define NPRODUCER 1
#define NCONSUMER 32

#define N 1000

#define MBEXTRA (1 << 20)

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

double A[N];
size_t A_pidx = 0;
size_t A_cidx = 0;

void* do_produce(void* arg) {
	long tid;
	tid = (long)arg;
	printf("Producer Thread %li started working\n", tid);
	while(true) {
		pthread_mutex_lock(&mtx);
		if(A_pidx >= N) {
			pthread_mutex_unlock(&mtx);
			break;
		}
		A[A_pidx] = ((A_pidx * tid) / 3.452) + (N - A_pidx);
		A_pidx++;
		pthread_mutex_unlock(&mtx);
		usleep(1000);
	}
	printf("\nProducer Thread %li finished working\n", tid);
	pthread_exit((void*) 0);
}

void* do_consume(void* arg) {
	long tid;
	tid = (long)arg;
	printf("Consumer Thread %li started working\n", tid);
	while(true) {
		pthread_mutex_lock(&mtx);
		if(A_cidx >= N) {
			pthread_mutex_unlock(&mtx);
			break;
		} else if(A_cidx < A_pidx) {
			/* do work */
			printf("%f ", A[A_cidx]);
			A_cidx++;
		}
		pthread_mutex_unlock(&mtx);
	}
	printf("\nConsumer Thread %li finished working\n", tid);
	pthread_exit((void*) 0);
}

int main() {
	pthread_t producers[NPRODUCER];
	pthread_t consumers[NCONSUMER];
	pthread_attr_t attr;
	size_t stack_size;
	int rc;
	void* status;
	long tid;

	/* Create Thread attribute object */
	rc = pthread_attr_init(&attr);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread attribute object");
		return 1;
	}

	stack_size = sizeof(double)* N + MBEXTRA;
	pthread_attr_setstacksize(&attr, stack_size);

	for(long tid = 0; tid < NPRODUCER; tid++) {
		/* Create a new POSIX Thread */
		rc = pthread_create(&producers[tid], &attr, do_produce, (void*) tid);
		if(rc) {
			fprintf(stderr, "ERROR: while creating producer thread");
			return 2;
		}
	}

	for(long tid = 0; tid < NCONSUMER; tid++) {
		/* Create a new POSIX Thread */
		rc = pthread_create(&consumers[tid], &attr, do_consume, (void*) tid);
		if(rc) {
			fprintf(stderr, "ERROR: while creating consumer thread");
			return 2;
		}
	}

	/* Cleanup pthread_attr_t object, we don't need it anymore */
	pthread_attr_destroy(&attr);

	for(long tid = 0; tid < NPRODUCER; tid++) {
		rc = pthread_join(producers[tid], &status);
		if(rc) {
			fprintf(stderr, "ERROR: while waiting for producer thread to join");
			return 3;
		}
	}

	for(long tid = 0; tid < NCONSUMER; tid++) {
		rc = pthread_join(consumers[tid], &status);
		if(rc) {
			fprintf(stderr, "ERROR: while waiting for consumer thread to join");
			return 3;
		}
	}
	printf("Main thread exiting\n");
	return 0;
}
