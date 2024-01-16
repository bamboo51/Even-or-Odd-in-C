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
        printf("Player %s: ", client->name);
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
                printf("Player %s: ", client->name);
                DieWithClose("recv() failed", client->socket);
            }
            printf("Player %s: %lf\n", client->name, client->money);
            return NULL;
        }
    }
    return NULL;
}

void *sendNumbers(struct client *client){
    if(client->state==READY){
        if(send(client->socket, &(client->randomNumbers), sizeof(client->randomNumbers), 0)<0){
            printf("Player %s: ", client->name);
            DieWithClose("send() failed", client->socket);
        }
        printf("Sent numbers %d%d to %s\n", client->randomNumbers[0], client->randomNumbers[1], client->name);
        if(recv(client->socket, &(client->answer), sizeof(client->answer), 0)<0){
            printf("Player %s: ", client->name);
            DieWithClose("recv() failed", client->socket);
        }
    }
    return NULL;
}

void *sendMoney(struct client *client){
    if(client->state==READY){
        if(send(client->socket, &(client->money), sizeof(client->money), 0)<0){
            printf("Player %s: ", client->name);
            DieWithClose("send() failed", client->socket);
        }
        printf("Sent money %lf to %s\n", client->money, client->name);
    }
    clientInit(client, client->serverSocket);
    return NULL;
}