#include "header.h"
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/in.h>

void *ThreadMain(void *arg);
int acceptConnection(int serverSocket);
int createServerSocket(unsigned short port);
void HandleTCPClient(int clientSocket);
void generateNumbers(void);
void showIP(void);

struct ThreadArgs{
    int clientSocket;
};
int numbers[2]={0, 0};
int i=0;
int maxClients;

int main(int argc, char *argv[]){
    int serverSocket, clientScoket;
    unsigned short serverPort;
    pthread_t threadID;

    // define max clients
    if(argc == 1){
        maxClients = MAX_CLIENTS;
    }else{
        maxClients = atoi(argv[1]);
    }
    printf("%d\n", maxClients);
    showIP();

    serverPort = PORT;
    serverSocket = createServerSocket(serverPort);
    printf("Show IP\n");

    for(;;){
        if(i%maxClients==0){
            generateNumbers();
            i=0;
        }

        clientScoket = acceptConnection(serverSocket);

        // create separate thread for each client
        if(pthread_create(&threadID, NULL, ThreadMain, (void *) clientScoket) != 0){
            DieWithError("pthread_create() failed");
        }

        printf("with thread %ld\n", (long int) threadID);
    }

    return 0;
}

int acceptConnection(int serverSocket){
    int clientSocket;
    struct sockaddr_in clientAddress;
    unsigned int clientLength;

    // wait for client to connect
    clientLength = sizeof(clientAddress);
    if((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength)) < 0){
        DieWithError("accept() failed");
    }

    // clientSocket is connected to a client
    printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));

    return clientSocket;

}

void *ThreadMain(void *arg){
    int clientSocket = (int) arg;

    // detach thread
    pthread_detach(pthread_self());

    // receive message from client
    HandleTCPClient(clientSocket);

    return (NULL);
}

int createServerSocket(unsigned short port){
    int sock;
    struct sockaddr_in serverAddress;

    // create socket
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        DieWithError("socket() failed");
    }

    // construct server address structure
    memset(&serverAddress, 0, sizeof(serverAddress));   // zero out structure
    serverAddress.sin_family = AF_INET;                 // internet address family
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);  // any incoming interface
    serverAddress.sin_port = htons(port);               // local port

    // bind to the local address
    if(bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
        DieWithError("bind() failed");
    }

    // mark the socket so it will listen for incoming connections
    if(listen(sock, MAX_CLIENTS) < 0){
        DieWithError("listen() failed");
    }

    return sock;
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
                printf("%s\n", inet_ntoa(sa->sin_addr));
                break; // Stop after finding the first match (eth0)
            }
        }
    }

    freeifaddrs(ifap);
}

void HandleTCPClient(int clientScoket){
    char ready[] = "ready";
    char buffer[BUFSIZE];
    int receiveMessageSize;

    // receive message from client
    if((receiveMessageSize = recv(clientScoket, buffer, BUFSIZE, 0)) < 0){
        DieWithError("recv() failed");
    }

    if(strcmp(buffer, ready)==0){
        i++;
    }
    
    if(i==maxClients){
        // send numbers to client
        if(send(clientScoket, numbers, sizeof(numbers), 0) != sizeof(numbers)){
            DieWithError("send() failed");
        }
    }
    close(clientScoket);
}

void generateNumbers(void){
    srand(time(NULL));
    numbers[0] = rand() % 6 + 1;
    srand(time(NULL)+rand());
    numbers[1] = rand() % 6 + 1;
}