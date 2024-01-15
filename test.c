#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> // For dynamic memory allocation

void *print(void *arg) {
    int *thread_arg = (int *)arg;
    int i = *thread_arg;
    printf("In thread: %d\n", i);
    free(thread_arg); // Free the dynamically allocated memory
}

int main(void) {
    pthread_t thread[2];

    for (int i = 0; i < 2; i++) {
        int *arg = (int *)malloc(sizeof(int)); // Allocate memory for each thread's argument
        *arg = i; // Set the value of the argument
        pthread_create(&thread[i], NULL, print, arg); // Pass the dynamically allocated argument
    }

    // Wait for threads to finish before exiting
    for (int i = 0; i < 2; i++) {
        pthread_join(thread[i], NULL);
    }
    pthread_exit(NULL);

    return 0;
}
