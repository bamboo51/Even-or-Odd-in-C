#include <unistd.h>
#include "header.h"
#include "functions.h"

struct sockaddr_in setServerAddress(char *serverIP, unsigned short port);
int createClientSocket(struct sockaddr_in serverAddress);

void gameInstruction(void){
    printf("*** Game Instruction ***\n");
    printf("1. If you are ready,\t\t\tpress 'y'\n");
    printf("2. If you want to add more money,\tpress 'a'\n");
    printf("   Please note that after you add money, you will have to wait to next round\n");
    printf("3. If you want to quit,\t\t\tpress 'q'\n");
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
    if(send(client.socket, client.name, sizeof(client.name), 0)<0) DieWithClose("send() failed", client.socket);

    // set money
    printf("Initial money: ");
    scanf("%lf", &(client.money));

    // game instruction
    gameInstruction();
    printf("Waiting for other players...\n");
    char ready[6];
    if(recv(client.socket, ready, sizeof(ready), 0)<0) DieWithClose("recv() failed", client.socket);
    printf("Other players are ready\n");

    //loop for game
    while(1){
        printf("Are you ready? (y/a/q): ");
        char input;
        scanf(" %c", &input);
        if(input=='q'){
            // quit
            client.state = QUIT;
            if(send(client.socket, &(client.state), sizeof(client.state), 0)<0)DieWithClose("send() failed", client.socket);
            break;
        }

        if(input=='a'){
            // send state
            client.state = WAIT;
            if(send(client.socket, &(client.state), sizeof(client.state), 0)<0)DieWithClose("send() failed", client.socket);

            // change money
            printf("Add money: ");
            double addMoney;
            scanf("%lf", &(addMoney));
            client.money += addMoney;
        }

        if(input=='y'){
            // input money for this round
            printf("Input money for this round: ");
            double money;
            scanf("%lf", &(money));
            while(money>client.money){
                printf("You don't have enough money\n");
                printf("Input money for this round: ");
                scanf("%lf", &(money));
            }
            client.money -= money;

            // ready
            client.state = READY;
            if(send(client.socket, &(client.state), sizeof(client.state), 0)<0)DieWithClose("send() failed", client.socket);

            //send money
            if(send(client.socket, &(money), sizeof(money), 0)<0)DieWithClose("send() failed", client.socket);

            // guess even or odd
            printf("Guess even or odd (e/o): ");
            scanf(" %c", &input);

            //receive numbers
            unsigned int *numbers = (unsigned int *)calloc(2, sizeof(unsigned int));
            if(recv(client.socket, numbers, 2*sizeof(unsigned int), 0)<0)DieWithClose("recv() failed", client.socket);
            print_dice(numbers[0]);
            print_dice(numbers[1]);

            // check the answer
            if((numbers[0]+numbers[1])%2==0 && input=='e'){
                printf("You win\n");
                client.answer = true;
                if(send(client.socket, &(client.answer), sizeof(client.answer), 0)<0)DieWithClose("send() failed", client.socket);
            }else if((numbers[0]+numbers[1])%2==1 && input=='o'){
                printf("You win\n");
                client.answer = true;
                if(send(client.socket, &(client.answer), sizeof(client.answer), 0)<0)DieWithClose("send() failed", client.socket);
            }else{
                printf("You lose\n");
                client.answer = false;
                if(send(client.socket, &(client.answer), sizeof(client.answer), 0)<0)DieWithClose("send() failed", client.socket);
            }
            free(numbers);

            // receive money
            money = 0;
            if(recv(client.socket, &(money), sizeof(money), 0)<0)DieWithClose("recv() failed", client.socket);
            client.money += money;
            printf("Your money: %lf\n", client.money);
        }
    }

    // Close socket
    close(client.socket);
}