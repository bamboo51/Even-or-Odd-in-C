#include "header.h"

void clientInit(struct client *client){
    // initialize client
    client->state = WAIT;
    client->money = 0;
    client->answer = false;
    if(recv(client->socket, client->name, 20, 0)<0){
        DieWithClose("recv() failed", client->socket);
    }
}

void acceptConnections(struct client *clients, int maxClients, int serverSocket){
    for(int i=0;i<maxClients;i++){
        struct sockaddr_in clientAddress;
        unsigned int clientLength = sizeof(clientAddress);
        clients[i].socket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if(clients[i].socket<0) DieWithClose("accept() failed", serverSocket);
        clientInit(&clients[i]);
        printf("Player %s connected\n", clients[i].name);
    }
}

int acceptConnectionWithTimeout(int serverSocket, int timeoutInSeconds) {
    struct sockaddr_in clientAddress;
    socklen_t clientLength = sizeof(clientAddress);

    // Set up the file descriptor set for select
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(serverSocket, &readSet);

    // Set up the timeout
    struct timeval timeout;
    timeout.tv_sec = timeoutInSeconds;
    timeout.tv_usec = 0;

    // Wait for a connection with a timeout
    int ready = select(serverSocket + 1, &readSet, NULL, NULL, &timeout);
    if (ready == -1) {
        DieWithClose("select() failed", serverSocket);
    } else if (ready == 0) {
        // Timeout occurred
        return -1; // You can handle this case according to your needs
    } else {
        // Connection is ready to be accepted
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        DieWithClose("accept() failed", serverSocket);
        return clientSocket;
    }
    
    return -1;
}

void acceptNewClients(struct client *clients, int serverSocket, int *emptyClients, int *emptyClientsCount){
    for(int i=0;i<*emptyClientsCount;i++){
        int newClientSocket = acceptConnectionWithTimeout(serverSocket, 1);
        printf("Check new client: %d\n", newClientSocket);
        if(newClientSocket>0){
            //check if there is any empty client
            if(*emptyClientsCount>0){
                clients[emptyClients[*emptyClientsCount-1]].socket = newClientSocket;
                clientInit(&clients[emptyClients[*emptyClientsCount-1]]);
                printf("Player %d: %s,\t%lf\n", emptyClients[*emptyClientsCount-1], clients[emptyClients[*emptyClientsCount-1]].name, clients[emptyClients[*emptyClientsCount-1]].money);
                emptyClients[*emptyClientsCount-1] = 0;
                *emptyClientsCount--;
            }else{
                close(newClientSocket);
            }
        }
    }
}