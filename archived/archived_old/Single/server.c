#include "header.h"

int number[2];
int old_number[2];
void randomNumber(){
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

int main(void){
    int serverSocket, clientSocket;
    unsigned int adlen;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family        = AF_INET;
    serverAddress.sin_addr.s_addr   = htonl(INADDR_ANY);
    serverAddress.sin_port          = htons(PORT);

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 1);

    while(1){
        int recvMsgSize;
        char receiveBuffer[RCVBUFSIZE];
        memset(receiveBuffer, '\0', sizeof(char)*RCVBUFSIZE);
        adlen = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &adlen);
        printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));

        while(strcmp(receiveBuffer, "ready")!=0){
            // get message from client
            if((recvMsgSize=recv(clientSocket, receiveBuffer, RCVBUFSIZE, 0))<0){
                printf("recv Error\n");
            }
        }

        randomNumber();
        printf("Client is ready\n");
        send(clientSocket, number, sizeof(number), 0);
        printf("Sent %d%d ", number[0], number[1]);
        printf("\n");
    }
}