#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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