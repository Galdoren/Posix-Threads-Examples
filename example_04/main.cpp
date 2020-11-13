#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

/* compile with g++ main.cpp -lpthread -o example.out */

#define NTHREADS 4

#define M NTHREADS
#define N 10000000

#define MBEXTRA (1 << 20)

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

double A[M][N];

void* do_work(void* arg) {

	long tid;

	tid = (long)arg;
	printf("Thread %li started working\n", tid);
	/* 
	   This is not an example for optimizing the threads, 
	   we specifically want each thread to reach every element in the array.
	*/
	for(int j = 0; j < M; j++) {
		for(int i = 0; i < N; i++) {
			/*
			   We lock the shared mutex between different threads. This is a blocking method,
			   which means that thread will wait here until it can lock the mutex.
			   T1 -> waiting, T2->waiting, T3->locked mutex, T4->waiting
			   T1 -> waiting, T2->waiting, T3->unlocked mutex, T4->waiting
			   T1 -> locked mutex, T2->waiting, T3->waiting, T4->waiting (a random thread obtains the lock, you can't decide which thread locks the mutex first here, it's randomly picked by the Kernel)
			   T1 -> unlocked mutex, T2->waiting, T3->waiting, T4->waiting
			   .
			   .
			   .
			*/
			pthread_mutex_lock(&mtx);
			/* This section between lock-unlock is usually called as critical section. */
			A[j][i] = ((i * j) / 3.452) + (N - i);
			/* 
			   We unlock the mutex since we don't need it anymore,
			   so that other threads waiting for the thread can lock the mutex.
			*/
			pthread_mutex_unlock(&mtx);
			/*
			if(pthread_mutex_trylock(&mtx)) {
				A[tid][i] = ((i * tid) / 3.452) + (N - i);
				pthread_mutex_unlock(&mtx);
			} else {
				i--;
			}
			*/
		}
	}

	printf("Thread %li finished working\n", tid);
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
	/* Create multiple threads */
	for(long tid = 0; tid < NTHREADS; tid++) {
		/* 
		   Create a new POSIX Thread. Please not that we can use the same 
		   thread attribute struct while creating multiple threads.
		*/
		rc = pthread_create(&thread[tid], &attr, do_work, (void*) tid);
		if(rc) {
			fprintf(stderr, "ERROR: while creating thread");
			return 2;
		}
	}

	/* Cleanup pthread_attr_t object, we don't need it anymore */
	pthread_attr_destroy(&attr);

	/* Wait for all threads to finish */
	for(long tid = 0; tid < NTHREADS; tid++) {
		rc = pthread_join(thread[tid], &status);
		if(rc) {
			fprintf(stderr, "ERROR: while waiting for thread to join");
			return 3;
		}
	}
	printf("Main thread exiting\n");
	//pthread_exit((void*) 0);
	return 0;
}
