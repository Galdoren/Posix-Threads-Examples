#include <pthread.h>
#include <stdio.h>

/* compile with g++ main.cpp -lpthread -o example.out */

void* do_work(void* arg) {
	printf("abc\n");
	pthread_exit((void*) 0);
}

int main() {
	pthread_t thread;
	pthread_attr_t attr;
	int rc;
	void* status;

	/* Create Thread attribute object */
	rc = pthread_attr_init(&attr);
	if(rc) {
		fprintf(stderr, "ERROR: while creating thread attribute object");
		return 1;
	}

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
