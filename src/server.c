#include "header.h"
#include <pthread.h>
#include <time.h>

void showIP(void);
int createServerSocket(unsigned short port, unsigned int maxClients);
void acceptNewClients(struct client *clients, int serverSocket, int *emptyClients, int *emptyClientsCount);

// functions for threads
void useThreads(struct client *clients, int maxClients, pthread_t *thread, void *(*function)(void *));
void *sendReady(void *arg);
void *checkReady(void *arg);
void *acceptConnections(void *arg);
void *sendNumbers(void *arg);
void *sendMoney(void *arg);
void *acceptNew(void *arg);

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
    for(int i=0;i<maxClients;i++){
        clientInit(&clients[i], serverSocket);
    }

    // accept connections
    useThreads(clients, maxClients, thread, acceptConnections);
    
    // set empty client socket
    int *emptyClients = (int *)calloc(maxClients, sizeof(int));

    srand(time(NULL));
    // loop
    while(1){
        int emptyClientsCount = 0;
        for(int i=0;i<maxClients;i++){
            printf("Player %s: %d\n", clients[i].name, clients[i].state);
            if(clients[i].state==QUIT){
                emptyClients[emptyClientsCount] = i;
                emptyClientsCount++;
            }
        }
        if(emptyClientsCount==maxClients){
            printf("All clients quit\n");
            break;
        }

        // accept new clients
        pthread_t acceptThread[emptyClientsCount];
        struct client *newClients = (struct client *)malloc(emptyClientsCount*sizeof(struct client));
        for (int i = 0; i < emptyClientsCount; i++) {
            clientInit(&newClients[i], serverSocket);
            pthread_create(&acceptThread[i], NULL, acceptNew, &newClients[i]);
        }
        for (int i = 0; i < emptyClientsCount; i++) {
            pthread_join(acceptThread[i], NULL);
            clients[emptyClients[i]] = newClients[i];
        }
        free(newClients);

        useThreads(clients, maxClients, thread, sendReady);

        // all money
        double money = 0;

        // check if any clients are ready
        useThreads(clients, maxClients, thread, checkReady);
        for(int i=0;i<maxClients;i++){
            if(clients[i].state==READY){
                money += clients[i].money;
            }
        }
        printf("All money: %lf\n", money);

        // generate numbers
        unsigned int *numbers = randomNumbers();
        for(int i=0;i<maxClients;i++){
            if(clients[i].state==READY){
                clients[i].randomNumbers[0] = numbers[0];
                clients[i].randomNumbers[1] = numbers[1];
            }
        }
        free(numbers);

        // send numbers
        useThreads(clients, maxClients, thread, sendNumbers);

        // check answers
        double winMoney = 0;
        for(int i=0;i<maxClients;i++){
            if(clients[i].state==READY){
                if(clients[i].answer==true){
                    winMoney += clients[i].money;
                }
            }
        }

        // calculate money
        if(winMoney==0){
            printf("No one wins\n");
        }else{
            money = money / winMoney;
        }
        for(int i=0;i<maxClients;i++){
            if(clients[i].state==READY){
                if(clients[i].answer==true){
                    clients[i].money =  clients[i].money * money;
                }else{
                    clients[i].money = 0;
                }
            }
        }

        // distribute money
        useThreads(clients, maxClients, thread, sendMoney);
        
    }
    

    // free all clients
    free(clients);
    free(emptyClients);

    // close server socket
    close(serverSocket);

    return 0;
}
