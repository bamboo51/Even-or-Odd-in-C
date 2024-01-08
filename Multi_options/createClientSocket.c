#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int createClientSocket(struct sockaddr_in serverAddress) {
    int clientSocket;
    if ((clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    return clientSocket;
}