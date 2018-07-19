#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

/* compile with g++ main.cpp -lpthread -o example.out */

#define M 4
#define N 255

#define MBEXTRA (1 << 20)

sem_t semaphore;

double A[M][N];

void* do_work(void* arg) {

	long tid;
	tid = (long)arg;

	/* lock semaphore, otherwise wait */
	printf("Thread %li started working\n", tid);
	for(int i = 0; i < M; i++) {
		for(int j = 0; j < N; j++) {
			A[i][j] = (((double)rand()) / RAND_MAX) * N;
		}
	}
	printf("Thread %li finished working\n", tid);
	/* release semaphore */
	sem_post(&semaphore);
	pthread_exit((void*) NULL);
}

void* do_calculate(void* arg) {
	long tid;
	tid = (long)arg;
	printf("Thread %li is created\n", tid);
	/* lock semaphore, otherwise wait */
	sem_wait(&semaphore);
	printf("Thread %li started working\n", tid);

	for(int i = 0; i < M; i++) {
		double average = 0.0;
		for(int j = 0; j < N; j++) {
			average += A[i][j];
		}
		average = average / N;
		printf("Average of A[%d] is %f\n", i, average);
	}

	printf("Thread %li finished working\n", tid);
	/* release semaphore */
	sem_post(&semaphore);
	pthread_exit((void*) NULL);
}

int main() {
	pthread_t thread_a;
	pthread_t thread_b;
	pthread_attr_t attr;
	size_t stack_size;
	int rc;
	void* status;
	long tid = 0;

	srand(time(NULL));

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
	sem_init(&semaphore, 0, 1);

	/* lock semaphore in order to make thread_b wait */
	sem_wait(&semaphore);

	/* Create a new POSIX Thread */
	rc = pthread_create(&thread_a, &attr, do_work, (void*) tid);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread_a");
		return 2;
	}

	tid++;

	/* Create a new POSIX Thread */
	rc = pthread_create(&thread_b, &attr, do_calculate, (void*) tid);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread_b");
		return 2;
	}

	/* Cleanup pthread_attr_t object, we don't need it anymore */
	pthread_attr_destroy(&attr);

	rc = pthread_join(thread_a, &status);
	if(rc) {
		fprintf(stderr, "ERROR: while waiting for thread to join");
		return 3;
	}

	rc = pthread_join(thread_b, &status);
	if(rc) {
		fprintf(stderr, "ERROR: while waiting for thread to join");
		return 3;
	}

	sem_destroy(&semaphore);
	printf("Main thread exiting\n");
	return 0;
}
