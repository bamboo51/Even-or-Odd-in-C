#include <pthread.h>
#include "header.h"

void useThreads(struct client *clients, int maxClients, pthread_t *thread, void *(*function)(void *)){
    for(int i=0;i<maxClients;i++){
        pthread_create(&thread[i], NULL, function, &clients[i]);
    }
    for(int i=0;i<maxClients;i++){
        pthread_join(thread[i], NULL);
    }
    //pthread_exit(NULL);
}