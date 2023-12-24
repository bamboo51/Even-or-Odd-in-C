#include "header.h"
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#define MAX_CLIENTS 2

int client_count = 0;
int clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handleClient(void *arg);
void generateNumbers(int *numbers);
void sendMessage(char *message, int currentClient);
void showIP(void);

int main(void){
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    // create socket
    if((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
        DieWithError("socket() failed");
    }

    // set server address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family        = AF_INET;
    serverAddress.sin_addr.s_addr   = htonl(INADDR_ANY);
    serverAddress.sin_port          = htons(PORT);

    // bind socket
    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0){
        DieWithError("bind() failed");
    }

    // listen for incoming connections
    if(listen(serverSocket, MAX_CLIENTS)<0){
        DieWithError("listen() failed");
    }

    printf("Server IP: ");
    showIP();
    printf("Listening on port %d\n", PORT);
    printf("Waiting for clients...\n");

    while(1){
        socklen_t clientAddressLength = sizeof(clientAddress);

        // accept client connection
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if(clientSocket<0){
            DieWithError("accept() failed");
        }

        // add client socket to the clients array
        pthread_mutex_lock(&clients_mutex);
        if(client_count<MAX_CLIENTS){
            clients[client_count++] = clientSocket;
            printf("Client %d connected\n", client_count);

            pthread_t tid;
            // create new thread for client
            pthread_create(&tid, NULL, handleClient, &clients[client_count-1]);
        }else{
            printf("Maximum number of clients reached. Connection rejected.\n");
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    return 0;
}

void generateNumbers(int *number){
    int old_number[2];
    old_number[0] = number[0];
    old_number[1] = number[1];

    srand(time(NULL));
    number[0] = rand()%6+1;
    srand(time(NULL)+rand());
    number[1] = rand()%6+1;

    while(number[0]==old_number[0] && number[1]==old_number[1]){
        srand(time(NULL));
        number[0] = rand()%6+1;
        srand(time(NULL)+rand());
        number[1] = rand()%6+1;
    }
}

void sendMessage(char *message, int currentClient){
    pthread_mutex_lock(&clients_mutex);
    for(int i=0;i<client_count;i++){
        
            if(send(clients[i], message, RCVBUFSIZE, 0)<0){
                DieWithError("send() failed");
            }
            printf("Sent %s\n", message);
        
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handleClient(void *arg){
    int clientSocket = *((int *)arg);
    char ready[] = "ready";
    char buffer[RCVBUFSIZE];
    int numbers[2]={0, 0};

    // wait "ready" message from all clients
    int all_ready = 0;
    while(!all_ready){
        all_ready = 1; // assume all clients are ready
        for(int i=0; i<client_count; i++){
            if(recv(clients[i], buffer, RCVBUFSIZE, 0)<0){
                DieWithError("recv() failed");
            }
            if(strcmp(buffer, ready)!=0){
                all_ready = 0;
                break;
            }
        }
    }
    printf("All clients are ready\n");

    // generate and send random numbers to all clients
    generateNumbers(numbers);
    memset(buffer, 0, RCVBUFSIZE);
    sprintf(buffer, "%d%d", numbers[0], numbers[1]);
    printf("%s\n", buffer);
    sendMessage(buffer, clientSocket);
    printf("Sent numbers to all clients\n");
    sendMessage("true", clientSocket);
    client_count = 0;

    // clean up and close the client socket
    pthread_mutex_lock(&clients_mutex);
    /*
    for(int i=0; i<client_count; i++){
        
        if(clients[i]==clientSocket){
            clients[i] = clients[client_count-1];
            client_count--;
            break;
        }
    }
    */
    printf("Client %d disconnected\n", client_count+1);
    pthread_mutex_unlock(&clients_mutex);
    close(clientSocket);
    pthread_exit(NULL);
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
