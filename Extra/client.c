#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

void setServerAddress(struct sockaddr_in *serverAddress, unsigned int serverPort);

int main(int argc, char *argv[]){
    int clientSocket;
    struct sockaddr_in serverAddress;
    char *serverIP;
    unsigned int serverPort;

    // set server IP and port
    if(argc!=3){
        fprintf(stderr, "Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    serverIP = argv[1];
    serverPort = atoi(argv[2]);

    // construct the server address structure
    setServerAddress(&serverAddress, serverPort);



}

void setServerAddress(struct sockaddr_in *serverAddress, unsigned int serverPort){
    memset(serverAddress, 0, sizeof(*serverAddress));
    serverAddress->sin_family       = AF_INET;
    serverAddress->sin_addr.s_addr  = htonl(INADDR_ANY);
    serverAddress->sin_port         = htons(serverPort);
}