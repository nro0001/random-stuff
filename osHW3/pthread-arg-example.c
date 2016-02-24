/* Example of passing an argument to a thread */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/* An argument of this struct type will be passed to each thread */
typedef struct {
	const char *message;
	int value;
} arg_t;

void *thread_function(void *targ) {
	arg_t *arg = (arg_t *)targ;
	printf("My argument has message %s and value %d\n",
		arg->message, arg->value);
	return NULL;
}

int main() {
	pthread_t thread_handle;
	arg_t arg;

	arg.message = "Hello";
	arg.value = 12345;

	/* Pass (pointer to) arg to the thread */
	if (pthread_create(&thread_handle, NULL, thread_function, &arg) != 0) {
		fprintf(stderr, "pthread_create failed\n");
		return 1;
	}

	if (pthread_join(thread_handle, NULL) != 0) {
		fprintf(stderr, "pthread_join failed\n");
		return 1;
	}
	return 0;
}
