#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* compile with g++ main.cpp -lpthread -o example.out */

class abc {
private:
        struct thread_data {
			long tid;
            char* message;
        };
    
	pthread_t m_thread;

	static void* do_work(void* arg) {
                struct thread_data* data = (struct thread_data*)arg;
		printf("%s\n", data->message);
		printf("%ld\n", data->tid);
		/* cleanup message */
		free(data->message);
		free(data);
		pthread_exit((void*) 0);
	}
public:


	bool start() {
		pthread_attr_t attr;
                struct thread_data* data;
		pthread_attr_init(&attr);
                data = (struct thread_data*)malloc(sizeof(struct thread_data));
				data->message = (char*)malloc(20 * sizeof(char));
				data->tid = 5;
				sprintf(data->message, "hello world!");
		int rc = pthread_create(&m_thread, &attr, &abc::do_work, (void*)data);
		pthread_attr_destroy(&attr);
		if(rc) {
			fprintf(stderr, "ERROR: while creating thread\n");
			free(data->message);
			free(data);
			return false;
		}
		return true;
	}

	bool wait() {
		void* status;
		int rc = pthread_join(m_thread, &status);
		if(rc) {
			fprintf(stderr, "ERROR: while joining thread\n");
			return false;
		}
		return true;
	}
};

int main() {
	abc a;
	a.start();
	a.wait();
	return 0;
}
