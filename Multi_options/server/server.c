#include "showIP.c"
#include "createSocket.c"
#include "acceptConnection.c"
#include <unistd.h>

#define WAIT 0
#define READY 1
#define QUIT -1

struct client{
    int socket;
    char name[20];
    int state;
    unsigned int money;
};

// initialize client
void clientInit(struct client *client);

int main(int argc, char *argv[]){

    // define maxClients
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
    unsigned int maxClients = atoi(argv[2]);
    unsigned int new_maxClients = maxClients;

    // create socket
    int serverSocket = createSocket(port, maxClients);
    showIP();
    printf("%d\nWaiting for incoming connections...\n", port);

    // create client array
    struct client *clients = (struct client *)malloc(maxClients*sizeof(struct client));
    if (clients == NULL) {
        perror("malloc() failed");
        close(serverSocket);  // Close server socket before exit
        exit(EXIT_FAILURE);
    }

    // accept connections
    for(int i=0; i<maxClients; i++){
        clients[i].socket = acceptConnection(serverSocket);
        clientInit(&clients[i]);
        printf("Player %d: %s,\t%d\n", i, clients[i].name, clients[i].money);
    }

    // set empty client
    unsigned int *emptyClients = (unsigned int *)calloc(maxClients, sizeof(unsigned int));
    unsigned int emptyClientsCount = 0;

    // loop
    while(1){
        // check if all clients quit
        if(emptyClientsCount==maxClients){
            printf("All clients quit\n");
            break;
        }

        // check if there are any new clients
        /*
        int newClient = acceptConnection(serverSocket);
        printf("Check new client: %d\n", newClient);
        if(newClient>0){
            // check if there is any empty client
            if(emptyClientsCount>0){
                // assign new client to empty client
                clients[emptyClients[emptyClientsCount-1]].socket = newClient;
                clientInit(&clients[emptyClients[emptyClientsCount-1]]);
                printf("Player %d: %s,\t%d\n", emptyClients[emptyClientsCount-1], clients[emptyClients[emptyClientsCount-1]].name, clients[emptyClients[emptyClientsCount-1]].money);
                emptyClients[emptyClientsCount-1] = 0;
                emptyClientsCount--;
            }else{
                close(newClient);
            }
        }
        */
        

        // check if any client is ready
        printf("Check if any client is ready\n");
        for(int i=0;i<maxClients;i++){
            if(recv(clients[i].socket, &(clients[i].state), sizeof(clients[i].state), 0)>=0){
                if(clients[i].state==WAIT){
                    // set new money
                    if(recv(clients[i].socket, &(clients[i].money), sizeof(clients[i].money), 0)<0){
                        perror("recv() failed");
                        exit(EXIT_FAILURE);
                    }
                    clients[i].state = READY;
                }
                
                if(clients[i].state==QUIT){
                    printf("Player %d quit\n", i);
                    close(clients[i].socket);
                    clients[i].state = QUIT;
                    emptyClients[emptyClientsCount] = i;
                    emptyClientsCount++;
                }

                if(clients[i].state==READY){
                    printf("Player %d is ready\n", i);
                }
            }else{
                perror("recv() failed");
                exit(EXIT_FAILURE);
            }
        }

        // generate random number
        unsigned int *numbers = (unsigned int *)calloc(2, sizeof(unsigned int));
        numbers[0] = rand()%6+1;
        numbers[1] = rand()%6+1;
        printf("Numbers: %d, %d\n", numbers[0], numbers[1]);

        // send numbers to all clients
        for(int i=0;i<maxClients;i++){
            if(clients[i].state==READY){
                if(send(clients[i].socket, numbers, 2*sizeof(unsigned int), 0)<0){
                    perror("send() failed");
                    exit(EXIT_FAILURE);
                }
                clients[i].state = WAIT;
            }
        }

        free(numbers);
    }


    // Free all clients
    free(clients);
    free(emptyClients);

    // Close socket
    if (close(serverSocket) < 0) {
        perror("close() failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void clientInit(struct client *client){
    if(recv(client->socket, client->name, sizeof(client->name), 0)<0){
        perror("recv() failed");
        exit(EXIT_FAILURE);
    }
    
    client->state = WAIT;

    if(recv(client->socket, &(client->money), sizeof(client->money), 0)<0){
        perror("recv() failed");
        exit(EXIT_FAILURE);
    }
}