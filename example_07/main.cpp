#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

/* compile with g++ main.cpp -lpthread -std=c++11 -o example.out */

#define NPRODUCER 2
#define NCONSUMER 2

#define N 10000000

#define MBEXTRA (1 << 20)

// Data mutex
pthread_mutex_t mtx_data = PTHREAD_MUTEX_INITIALIZER;
// Next-to-access mutex
pthread_mutex_t mtx_ntam = PTHREAD_MUTEX_INITIALIZER;
// Low priority mutex
pthread_mutex_t mtx_lp = PTHREAD_MUTEX_INITIALIZER;


// Conditional variable to notify all threads to start at the same time
pthread_cond_t barrier = PTHREAD_COND_INITIALIZER;
pthread_cond_t barrier_completed = PTHREAD_COND_INITIALIZER;
pthread_mutex_t barrier_mtx = PTHREAD_MUTEX_INITIALIZER;
int barrier_count = NPRODUCER + NCONSUMER;
int barrier_completed_flag = 0;

/*
 * High priority thread:
 * lock mtx_ntam, lock mtx_data, unlock mtx_ntam, do stuff, unlock mtx_data
 *
 * Low priority thread:
 * lock mtx_lp, lock mtx_ntam, lock mtx_data, unlock mtx_ntam, do stuff, unlock mtx_data, unlock mtx_lp
 */

double A[N];
size_t A_pidx = 0;
size_t A_cidx = 0;

void* do_produce(void* arg) {
	long tid;
	tid = (long)arg;
	printf("Producer Thread %li is created, waiting for start signal\n", tid);
	pthread_mutex_lock(&barrier_mtx);
	barrier_count--;

	if(barrier_count == 0) {
		// all the other threads are accounted for, let the manager know we're ready
		pthread_cond_signal(&barrier);
	}

	while(!barrier_completed_flag) {
		pthread_cond_wait(&barrier_completed, &barrier_mtx);
	}
	pthread_mutex_unlock(&barrier_mtx);

	printf("Producer Thread %li started working\n", tid);
	while(true) {
		pthread_mutex_lock(&mtx_ntam);
		pthread_mutex_lock(&mtx_data);
		if(A_pidx >= N) {
			pthread_mutex_unlock(&mtx_ntam);
			pthread_mutex_unlock(&mtx_data);
			break;
		}
		pthread_mutex_unlock(&mtx_ntam);
		A[A_pidx] = ((A_pidx * tid) / 3.452) + (N - A_pidx);
		A_pidx++;
		pthread_mutex_unlock(&mtx_data);
		//usleep(1000);
	}
	printf("\nProducer Thread %li finished working\n", tid);
	pthread_exit((void*) 0);
}

void* do_consume(void* arg) {
	long tid;
	tid = (long)arg;
	printf("Consumer Thread %li is created, waiting for start signal\n", tid);
	pthread_mutex_lock(&barrier_mtx);
	barrier_count--;

	if(barrier_count == 0) {
		// all the other threads are accounted for, let the manager know we're ready
		pthread_cond_signal(&barrier);
	}

	while(!barrier_completed_flag) {
		pthread_cond_wait(&barrier_completed, &barrier_mtx);
	}
	pthread_mutex_unlock(&barrier_mtx);
	printf("Consumer Thread %li started working\n", tid);
	while(true) {
		pthread_mutex_lock(&mtx_lp);
		pthread_mutex_lock(&mtx_ntam);
		pthread_mutex_lock(&mtx_data);
		pthread_mutex_unlock(&mtx_ntam);
		if(A_cidx >= N) {			
			pthread_mutex_unlock(&mtx_data);
			pthread_mutex_unlock(&mtx_lp);
			break;
		} else if(A_cidx < A_pidx) {
			/* do work */
			//printf("%f ", A[A_cidx]);
			A_cidx++;
		}
		pthread_mutex_unlock(&mtx_data);
		pthread_mutex_unlock(&mtx_lp);
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

	// wait for threads to be created
	pthread_mutex_lock(&barrier_mtx);
	while(barrier_count != 0) {
		pthread_cond_wait(&barrier, &barrier_mtx);
	}
	pthread_mutex_unlock(&barrier_mtx);

	sleep(2);

	// at this point, all threads are created, given them start signal
	pthread_mutex_lock(&barrier_mtx);
	barrier_completed_flag = 1;
	pthread_mutex_unlock(&barrier_mtx);
	pthread_cond_broadcast(&barrier_completed);

	auto start = std::chrono::steady_clock::now();

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

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << "elapsed time: " << elapsed_seconds.count() << 's' << std::endl;

	printf("Main thread exiting\n");
	return 0;
}
