#include "header.h"
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#define MAX_CLIENTS 4

void showIP(void);
int createSocket(void);
void HandleClient(int clientSocket);
int acceptConnection(int serverSocket);

int maxClients = MAX_CLIENTS;
int new_maxClients = MAX_CLIENTS;

int main(int argc, char **argv){
    // define maxClients
    if(argc==2){
        new_maxClients=atoi(argv[1]);
    }
    else{
        new_maxClients=MAX_CLIENTS;
    }

    int serverSocket, *clientSocket;

    serverSocket = createSocket();
    showIP();
    printf("Waiting for incoming connections...\n");

    char buffer[RCVBUFSIZE];
    char quit[] = "quit";
    int number[2]={0, 0};

    while(1){
        maxClients = new_maxClients;
        if(maxClients==0){
            printf("All clients quit\n");
            break;
        }

        // accept connections
        clientSocket = (int *)malloc(maxClients*sizeof(int));
        for(int i=0; i<maxClients; i++){
            clientSocket[i] = acceptConnection(serverSocket);
            printf("Client %d connected\n", i);
        }

        // check if all clients are ready
        for(int i=0; i<maxClients; i++){
            memset(buffer, '\0', sizeof(char)*RCVBUFSIZE);
            if(recv(clientSocket[i], buffer, RCVBUFSIZE, 0)<0){
                DieWithError("recv() failed");
            }
            printf("Received from client %d: %s\n", i, buffer);
            if(strcmp(buffer, quit)==0){
                printf("Client %d quit\n", i);
                clientSocket[i] = -1;
                close(clientSocket[i]);
                new_maxClients -= 1;
                continue;
            }
        }

        // send number to all clients
        memset(buffer, '\0', sizeof(char)*RCVBUFSIZE);
        number[0] = rand()%6+1;
        number[1] = rand()%6+1;
        sprintf(buffer, "%d%d", number[0], number[1]);
        printf("%s\n", buffer);

        // close socket
        for(int i=0; i<maxClients; i++){
            if(clientSocket[i]==-1){
                continue;
            }
            if(send(clientSocket[i], buffer, RCVBUFSIZE, 0)<0){
                DieWithError("send() failed");
            }
            close(clientSocket[i]);
        }

        free(clientSocket);
    }
}

int createSocket(void){
    int serverSocket;
    struct sockaddr_in serverAddress;

    if((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
        DieWithError("socket() failed");
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family        = AF_INET;
    serverAddress.sin_addr.s_addr   = htonl(INADDR_ANY);
    serverAddress.sin_port          = htons(PORT);

    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0){
        DieWithError("bind() failed");
    }

    if(listen(serverSocket, maxClients)<0){
        DieWithError("listen() failed");
    }

    return serverSocket;
}

int acceptConnection(int serverSocket){
    int clientSocket;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if(clientSocket<0){
        DieWithError("accept() failed");
    }

    return clientSocket;
}

void showIP(void) {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;

    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *)ifa->ifa_addr;
            if (strcmp(ifa->ifa_name, "eth0") == 0) {
                printf("Server: %s:%d\n", inet_ntoa(sa->sin_addr), PORT);
                break; // Stop after finding the first match (eth0)
            }
        }
    }

    freeifaddrs(ifap);
}
