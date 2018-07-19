#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* compile with g++ main.cpp -lpthread -o example.out */

#define N 10000
#define KB(x) ((size_t) (x) << 10)
#define MB(x) ((size_t) (x) << 20)

#define MBEXTRA 1000000

pthread_attr_t attr;

void* do_work(void* arg) {
	double A[N][N];
	int i, j;
	unsigned long int tid;
	size_t stack_size;



	tid = pthread_self();
	pthread_attr_getstacksize(&attr, &stack_size);
	printf("Thread id %ld: stack size = %li bytes\n", tid, stack_size);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			A[i][j] = ((i * j) / 3.452) + (N - i);
		}
	}
	pthread_exit((void*) NULL);
}

int main() {
	pthread_t thread;
	size_t  stack_size;
	int rc;
	void* status;

	/* Create Thread attribute object */
	rc = pthread_attr_init(&attr);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread attribute object");
		return 1;
	}

	pthread_attr_getstacksize(&attr, &stack_size);
	printf("Default stack size = %li bytes\n", stack_size);
	stack_size = sizeof(double)* N * N + MBEXTRA;
	pthread_attr_setstacksize(&attr, stack_size);

	/* Create a new POSIX Thread */
	rc = pthread_create(&thread, &attr, do_work, (void*) NULL);
	/* Cleanup pthread_attr_t object, we don't need it anymore */
	pthread_attr_destroy(&attr);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread");
		return 2;
	}

	rc = pthread_join(thread, &status);
	if(rc) {
		fprintf(stderr, "ERROR: while waiting for thread to join");
		return 3;
	}
	return 0;
}
