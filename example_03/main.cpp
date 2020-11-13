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
	/* 
	  A large double array of double, which will exceed the default stack size in most architectures. 
	  Fortunately, we've increased the stack size beforehand while creating the thread.
	*/
	double A[N][N];
	int i, j;
	unsigned long int tid;
	size_t stack_size;



	tid = pthread_self();
	/* get the stack size, you should see that it'll be equal to the value we've set before creating this thread */
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
	/* lets see the default stack size for a POSIX thread first */
	pthread_attr_getstacksize(&attr, &stack_size);
	printf("Default stack size = %li bytes\n", stack_size);
	/* Lets set the stack size to the value of N * N + MBEXTRA */
	stack_size = sizeof(double)* N * N + MBEXTRA;
	pthread_attr_setstacksize(&attr, stack_size);

	/* Create a new POSIX Thread, don't forget to pass in the attribute so that we set the stack size for the thread */
	rc = pthread_create(&thread, &attr, do_work, (void*) NULL);
	/* Cleanup pthread_attr_t object, we don't need it anymore */
	pthread_attr_destroy(&attr);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread");
		return 2;
	}
	/* Wait for thread to finish */
	rc = pthread_join(thread, &status);
	if(rc) {
		fprintf(stderr, "ERROR: while waiting for thread to join");
		return 3;
	}
	return 0;
}
