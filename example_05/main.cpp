#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

/* compile with g++ main.cpp -lpthread -o example.out */

#define NTHREADS 8

#define M NTHREADS
#define N 100000000

#define MBEXTRA (1 << 20)

sem_t semaphore;

double A[M][N];

void* do_work(void* arg) {

	long tid;
	tid = (long)arg;

	/* lock semaphore, otherwise wait */
	sem_wait(&semaphore);
	printf("Thread %li started working\n", tid);
	for(int i = 0; i < N; i++) {
		A[tid][i] = ((i * tid) / 3.452) + (N - i);
	}
	printf("Thread %li finished working\n", tid);
	/* release semaphore */
	sem_post(&semaphore);
	pthread_exit((void*) NULL);
}

int main() {
	pthread_t thread[NTHREADS];
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

	stack_size = sizeof(double)* M * N + MBEXTRA;
	pthread_attr_setstacksize(&attr, stack_size);

	/**
	 * @brief sem_init
	 * @arg sem_t *sem Specifies the semaphore to be initialized
	 * @arg pshared Specifies if this semaphore is going to be shared among processes or between threads,
	 * A non-zero value means semaphore is shared between processes, and a value of zero means semaphore
	 * is going to be shared between threads.
	 * @arg value Specifies the value to assign to the newly initialized semaphore
	 */
	sem_init(&semaphore, 0, NTHREADS / 4);

	for(long tid = 0; tid < NTHREADS; tid++) {
		/* Create a new POSIX Thread */
		rc = pthread_create(&thread[tid], &attr, do_work, (void*) tid);
		if(rc) {
			fprintf(stderr, "ERROR: while creating thread");
			return 2;
		}
	}

	/* Cleanup pthread_attr_t object, we don't need it anymore */
	pthread_attr_destroy(&attr);

	for(long tid = 0; tid < NTHREADS; tid++) {
		rc = pthread_join(thread[tid], &status);
		if(rc) {
			fprintf(stderr, "ERROR: while waiting for thread to join");
			return 3;
		}
	}
	sem_destroy(&semaphore);
	printf("Main thread exiting\n");
	return 0;
}
