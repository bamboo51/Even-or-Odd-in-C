#include "header.h"

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serverAddress;
    char *serverIP;
    char message[] = "ready";
    int number[2];
    char buffer[RCVBUFSIZE];
    char state[2];

    if(argc != 2){
        fprintf(stderr, "Usage: %s <Server IP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    serverIP = argv[1];

    // construct the server address structure
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family        = AF_INET;
    serverAddress.sin_addr.s_addr   = inet_addr(serverIP);
    serverAddress.sin_port          = htons(PORT);
    printf("If you want to quit, press q\n");

    // initial money
    double money;
    printf("Input money: ");
    scanf("%lf", &money);

    while (1) {
        // create socket using TCP (outside the loop)
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            DieWithError("socket() failed");
        }

        // establish the connection to the server (outside the loop)
        if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            DieWithError("connect() failed");
        }

        printf("Are you ready? [y/q]: ");
        scanf("%s", state);
        printf("Wait until all players are ready\n");

        if (strcmp(state, "q") == 0) {
            if (send(sock, "quit", strlen("quit"), 0) <= 0) {
                DieWithError("send() failed");
            }
            break;
        } else if (strcmp(state, "y") == 0) {
            if (send(sock, message, strlen(message), 0) <= 0) {
                DieWithError("send() failed");
            }
            memset(buffer, 0, RCVBUFSIZE);

            if (recv(sock, buffer, RCVBUFSIZE, 0) < 0) {
                DieWithError("recv() failed");
            } else {
                number[0] = buffer[0]-'0';
                number[1] = buffer[1]-'0';
                memset(buffer, 0, RCVBUFSIZE);
                printf("You got %d%d\n", number[0], number[1]);
                memset(buffer, 0, RCVBUFSIZE);
            }
        } else {
            printf("Bad input\n");
        }

        close(sock);
    }

    return 0;
}
