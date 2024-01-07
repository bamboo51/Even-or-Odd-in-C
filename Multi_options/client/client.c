#include <unistd.h>
#include "setServerAddress.c"
#include "createClientSocket.c"

#define WAIT 0
#define READY 1
#define QUIT -1

struct client{
    int socket;
    char name[20];
    int state;
    unsigned int money;
};

void gameInstruction(void){
    printf("*** Game Instruction ***\n");
    printf("1. If you are ready,\t\tpress 'y'\n");
    printf("2. If you want to change money,\tpress 'm'\n");
    printf("   Please note that after you change money, you will be join to the game immediately.\n")
    printf("3. If you want to quit,\t\tpress 'q'\n");
}

int main(int argc, char *argv[]){
    // define client
    struct client client;

    if(argc!=3){
        printf("Usage: %s <Server IP> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // set server IP and port
    struct sockaddr_in serverAddress = setServerAddress(argv[1], atoi(argv[2]));

    // create socket
    client.socket = createClientSocket(serverAddress);
    
    // set name
    printf("Player Name: ");
    scanf("%s", client.name);
    // send name
    if(send(client.socket, client.name, sizeof(client.name), 0)<0){
        perror("send() failed");
        exit(EXIT_FAILURE);
    }

    // set money
    printf("Money: ");
    scanf("%d", &(client.money));
    // send money
    if(send(client.socket, &(client.money), sizeof(client.money), 0)<0){
        perror("send() failed");
        exit(EXIT_FAILURE);
    }

    // game instruction
    gameInstruction();

    //loop for game
    while(1){
        printf("Are you ready? (y/m/q): ");
        char input;
        scanf(" %c", &input);
        if(input=='q'){
            // quit
            client.state = QUIT;
            if(send(client.socket, &(client.state), sizeof(client.state), 0)<0){
                perror("send() failed");
                exit(EXIT_FAILURE);
            }
            break;
        }

        if(input=='m'){
            // send state
            client.state = WAIT;
            if(send(client.socket, &(client.state), sizeof(client.state), 0)<0){
                perror("send() failed");
                exit(EXIT_FAILURE);
            }

            // change money
            printf("New amount of money: ");
            scanf("%d", &(client.money));

            // send money
            if(send(client.socket, &(client.money), sizeof(client.money), 0)<0){
                perror("send() failed");
                exit(EXIT_FAILURE);
            }

            //receive numbers
            unsigned int *numbers = (unsigned int *)calloc(2, sizeof(unsigned int));
            if(recv(client.socket, numbers, 2*sizeof(unsigned int), 0)<0){
                perror("recv() failed");
                exit(EXIT_FAILURE);
            }
            printf("Numbers: %d, %d\n", numbers[0], numbers[1]);
            free(numbers);
        }

        if(input=='y'){
            // ready
            client.state = READY;
            if(send(client.socket, &(client.state), sizeof(client.state), 0)<0){
                perror("send() failed");
                exit(EXIT_FAILURE);
            }

            //receive numbers
            unsigned int *numbers = (unsigned int *)calloc(2, sizeof(unsigned int));
            if(recv(client.socket, numbers, 2*sizeof(unsigned int), 0)<0){
                perror("recv() failed");
                exit(EXIT_FAILURE);
            }
            printf("Numbers: %d, %d\n", numbers[0], numbers[1]);
            free(numbers);
        }
    }

    // Close socket
    if (close(client.socket) < 0) {
        perror("close() failed");
        exit(EXIT_FAILURE);
    }

}