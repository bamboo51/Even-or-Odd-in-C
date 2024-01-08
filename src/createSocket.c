#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int createSocket(unsigned short port, unsigned int maxClients) {
    int serverSocket;
    struct sockaddr_in serverAddress;

    if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, maxClients) < 0) {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}
