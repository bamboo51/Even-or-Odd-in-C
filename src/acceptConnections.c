#include "header.h"
#include <sys/select.h>

void *acceptConnections(void *arg){
    struct client *clients = (struct client *)arg;
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);
    clients->socket = accept(clients->serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
    if(clients->socket<0) DieWithClose("accept() failed", clients->serverSocket);
    if(recv(clients->socket, clients->name, sizeof(clients->name), 0)<0) DieWithClose("recv() failed", clients->socket);
    return(NULL);
}

void *acceptNew(void *arg){
    struct client *clients = (struct client *)arg;
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);

    // Set up the file descriptor set for select
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(clients->serverSocket, &readSet);

    // Set up the timeout
    struct timeval timeout;
    timeout.tv_sec = 5;  // Set the timeout to 5 seconds
    timeout.tv_usec = 0;

    // Wait for a connection with a timeout
    int ready = select(clients->serverSocket + 1, &readSet, NULL, NULL, &timeout);
    if (ready == -1) {
        clients->state = QUIT;
        DieWithClose("select() failed", clients->serverSocket);
    } else if (ready == 0) {
        // Timeout occurred
        printf("Connection timeout\n");
        clients->state = QUIT;
        close(clients->socket);
        // You can handle this case according to your needs, for example, return from the function or exit the program.
        return NULL;
    } else {
        // Connection is ready to be accepted
        clients->socket = accept(clients->serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if(clients->socket < 0) {
            DieWithClose("accept() failed", clients->serverSocket);
        }
    }

    // Continue with the rest of the function (e.g., receiving data)
    if(recv(clients->socket, clients->name, sizeof(clients->name), 0) < 0) {
        DieWithClose("recv() failed", clients->socket);
    }

    return NULL;
}