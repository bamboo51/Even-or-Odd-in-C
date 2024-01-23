#include "header.h"

void clientInit(struct client *client, int serverSocket){
    // initialize client
    client->state = WAIT;
    client->money = 0;
    client->answer = false;
    client->serverSocket = serverSocket;
}