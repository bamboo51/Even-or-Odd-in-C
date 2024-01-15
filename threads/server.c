#include "header.h"
#include <pthread.h>

void showIP(void);
int createServerSocket(unsigned short port, unsigned int maxClients);
void acceptConnections(struct client *clients, int maxClients, int serverSocket);
void acceptNewClients(struct client *clients, int serverSocket, int *emptyClients, int *emptyClientsCount);

// functions for threads
void useThreads(struct client *clients, int maxClients, pthread_t *thread, void *(*function)(void *));
void *sendReady(void *arg);
void *checkReady(void *arg);

// generate random number
unsigned int *randomNumbers(void){
    unsigned int *numbers = (unsigned int *)calloc(2, sizeof(unsigned int));
    numbers[0] = rand()%6+1;
    numbers[1] = rand()%6+1;
    printf("Numbers: %d, %d\n", numbers[0], numbers[1]);
    return numbers;
}

int main(int argc, char *argv[]){
    // define port and maxClients
    if(argc!=3){
        printf("Usage: %s <port> <maxClients>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    unsigned short port = atoi(argv[1]);
    if(port<1024){
        // 1024 is the minimum port number that can be used
        printf("Port number must be greater than 1024\n");
        exit(EXIT_FAILURE);
    }
    int maxClients = atoi(argv[2]);

    // define pthreads
    pthread_t thread[maxClients];

    // create socket
    int serverSocket = createServerSocket(port, maxClients);
    showIP();
    printf("%d\nWaiting for incoming connections...\n", port);

    // create client array
    struct client *clients = (struct client *)malloc(maxClients*sizeof(struct client));
    if(clients==NULL) DieWithClose("malloc() failed", serverSocket);

    // accept connections
    acceptConnections(clients, maxClients, serverSocket);
    useThreads(clients, maxClients, thread, sendReady);
    
    // set empty client socket
    int *emptyClients = (int *)calloc(maxClients, sizeof(int));
    int emptyClientsCount = 0;

    // loop
    while(1){
        if(emptyClientsCount==maxClients){
            printf("All clients quit\n");
            break;
        }

        // check if there are any new clients
        acceptNewClients(clients, serverSocket, emptyClients, &emptyClientsCount);

        // all money
        double money = 0;

        // check if any clients are ready
        useThreads(clients, maxClients, thread, checkReady);
        for(int i=0;i<maxClients;i++){
            if(clients[i].state==QUIT){
                emptyClients[emptyClientsCount] = i;
                emptyClientsCount++;
            }
            money += clients[i].money;
        }
        printf("All money: %lf\n", money);

        // generate numbers
        unsigned int *numbers = randomNumbers();

        // send numbers
        useThreads(clients, maxClients, thread, sendNumbers);


    }
    

    // free all clients
    free(clients);
    free(emptyClients);

    // close server socket
    close(serverSocket);

    return 0;
}