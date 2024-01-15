#include "header.h"
#include <pthread.h>

void *sendReady(void *arg){
    struct client *client = (struct client *)arg;
    if(send(client->socket, "ready", 6, 0)<0){
        DieWithClose("send() failed", client->socket);
    }
    printf("Sent ready to %s\n", client->name);
    return NULL;
}

void *checkReady(void *arg){
    struct client *client = (struct client *)arg;
    if(recv(client->socket, &(client->state), sizeof(client->state), 0)<0){
        DieWithClose("recv() failed", client->socket);
    }else{
        if(client->state==WAIT){
            printf("Player %s skipped\n", client->name);
            return NULL;
        }

        if(client->state==QUIT){
            printf("Player %s quit\n", client->name);
            close(client->socket);
            client->state = QUIT;
            return NULL;
        }

        if(client->state==READY){
            if(recv(client->socket, &(client->money), sizeof(client->money), 0)<0){
                DieWithClose("recv() failed", client->socket);
            }
            printf("Player %s: %lf\n", client->name, client->money);
            return NULL;
        }
    }
    return NULL;
}

void *