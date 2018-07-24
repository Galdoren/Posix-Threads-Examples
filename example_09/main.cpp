#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_cond_t work_01 = PTHREAD_COND_INITIALIZER;
pthread_cond_t work_02 = PTHREAD_COND_INITIALIZER;
pthread_cond_t work_03 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t work_mtx = PTHREAD_MUTEX_INITIALIZER;

void* do_work_01(void* arg) {
    printf("Starting work_01 thread\n");
    pthread_mutex_lock(&work_mtx);
    printf("Signalling work_03\n");
    pthread_cond_signal(&work_03);
    printf("Waiting for work_01\n");
    pthread_cond_wait(&work_01, &work_mtx);
    pthread_mutex_unlock(&work_mtx);
    printf("Exiting work_01 thread\n");
    pthread_exit((void*)NULL);
}

void* do_work_02(void* arg) {
    printf("Starting work_02 thread\n");
    pthread_mutex_lock(&work_mtx);
    printf("Waiting for work_02\n");
    pthread_cond_wait(&work_02, &work_mtx);
    printf("Signalling work_01\n");
    pthread_cond_signal(&work_01);
    pthread_mutex_unlock(&work_mtx);
    printf("Exiting work_02 thread\n");
    pthread_exit((void*)NULL);
}

void* do_work_03(void* arg) {
    printf("Starting work_03 thread\n");
    pthread_mutex_lock(&work_mtx);
    printf("Waiting for work_03\n");
    pthread_cond_wait(&work_03, &work_mtx);
    printf("Signalling work_02\n");
    pthread_cond_signal(&work_02);
    pthread_mutex_unlock(&work_mtx);
    printf("Exiting work_03 thread\n");
    pthread_exit((void*)NULL);
}

int main() {
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;
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
    rc = pthread_create(&thread_3, &attr, do_work_03, (void*) NULL);
    /* Cleanup pthread_attr_t object, we don't need it anymore */
    pthread_attr_destroy(&attr);
    if(rc) {
        fprintf(stderr, "ERROR: while creating thread");
        return 2;
    }

    sleep(1);

    /* Create a new POSIX Thread */
    rc = pthread_create(&thread_2, &attr, do_work_02, (void*) NULL);
    /* Cleanup pthread_attr_t object, we don't need it anymore */
    pthread_attr_destroy(&attr);
    if(rc) {
        fprintf(stderr, "ERROR: while creating thread");
        return 2;
    }

    sleep(1);

    /* Create a new POSIX Thread */
    rc = pthread_create(&thread_1, &attr, do_work_01, (void*) NULL);
    /* Cleanup pthread_attr_t object, we don't need it anymore */
    pthread_attr_destroy(&attr);
    if(rc) {
        fprintf(stderr, "ERROR: while creating thread");
        return 2;
    }

    sleep(1);

    rc = pthread_join(thread_1, &status);
    if(rc) {
        fprintf(stderr, "ERROR: while waiting for thread to join");
        return 3;
    }

    rc = pthread_join(thread_2, &status);
    if(rc) {
        fprintf(stderr, "ERROR: while waiting for thread to join");
        return 3;
    }

    rc = pthread_join(thread_3, &status);
    if(rc) {
        fprintf(stderr, "ERROR: while waiting for thread to join");
        return 3;
    }
    printf("Exiting main thread\n");
    return 0;
}
