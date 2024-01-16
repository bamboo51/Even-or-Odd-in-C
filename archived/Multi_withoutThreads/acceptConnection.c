#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

int acceptConnection(int serverSocket){
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
    if(clientSocket<0){
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }
    return clientSocket;
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
        perror("select() failed");
        exit(EXIT_FAILURE);
    } else if (ready == 0) {
        // Timeout occurred
        return -1; // You can handle this case according to your needs
    } else {
        // Connection is ready to be accepted
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            perror("accept() failed");
            exit(EXIT_FAILURE);
        }
        return clientSocket;
    }
}