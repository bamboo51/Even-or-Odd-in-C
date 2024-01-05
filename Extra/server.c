#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#define PORT 10000
#define MAX_CLIENTS 4

// state for game play
#define WAIT 0
#define READY 1
#define QUIT -1

typedef struct client{
    unsigned int socket;
    char name[20];
    double money;
    int state;
} client;

int maxClients = MAX_CLIENTS;
int new_maxClients = MAX_CLIENTS;

int createSocket(unsigned int port);
void showIP(unsigned int port);

int main(int argc, char *argv[]){
    // define maxClients
    fprintf(stdout, "Number of clients: ");
    fscanf(stdin, "%d", &maxClients);
    if(maxClients<1 || maxClients>MAX_CLIENTS){
        fprintf(stderr, "Invalid number of clients\n");
        exit(EXIT_FAILURE);
    }
    new_maxClients = maxClients;
    
    // set port number
    unsigned int port;
    if(argc!=2){
        port = PORT;
    }else{
        if(atoi(argv[1])<1024 || atoi(argv[1])>65535){
            fprintf(stderr, "Invalid port number\n");
            exit(EXIT_FAILURE);
        }
        port = atoi(argv[1]);
    }
    
    // create socket
    int serverSocket = createSocket(port);
    showIP(port);
    printf("Waiting for incoming connections...\n");

    while(1){
        
    }
    return 0;
}

int createSocket(unsigned int port){
    int serverSocket;
    struct sockaddr_in serverAddress;

    // create socket
    if((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // construct local address structure
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family        = AF_INET;
    serverAddress.sin_addr.s_addr   = htonl(INADDR_ANY);
    serverAddress.sin_port          = htons(port);

    // bind to the local address
    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // listen to the client
    if(listen(serverSocket, maxClients)<0){
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}

void showIP(unsigned int port){
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
                fprintf(stdout, "Server: %s:%d\n", inet_ntoa(sa->sin_addr), port);
                break; // Stop after finding the first match (eth0)
            }
        }
    }

    freeifaddrs(ifap);
}