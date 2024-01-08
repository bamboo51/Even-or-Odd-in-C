#include "header.h"

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serverAddress;
    char *serverIP;
    char message[] = "ready";
    int number[2];
    char state[2];

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

    while(1){
        // set number to 0
        number[0] = 0;
        number[1] = 0;

        printf("Are you ready? [y/q]: ");
        scanf("%s", state);
        
        if(strcmp(state, "q")==0){
            close(sock);
            break;
        }else if(strcmp(state, "y")==0){
            // create socket using TCP
            if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
                DieWithError("socket() failed");
            }
            
            // establish the connection to the server
            if(connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0){
                DieWithError("connect() failed");
            }

            if(send(sock, message, strlen(message), 0)<=0){
                DieWithError("send() failed");
            }

            if(recv(sock, number, sizeof(number), 0)<=0){
                DieWithError("recv() failed");
            }else{
                printf("received: ");
                for(int i=0;i<2;i++){
                    printf("%d", number[i]);
                }
                printf("\n");
            }
        }else{
            printf("Bad input\n");
        }
        
        close(sock);
    }

}