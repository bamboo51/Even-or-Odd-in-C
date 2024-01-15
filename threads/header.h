#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// define states
#define WAIT 0
#define READY 1
#define QUIT -1

struct client{
    int socket;
    char name[20];
    int state;
    double money;
    bool answer;
    int serverSocket;
};

void DieWithClose(char *errorMessage, int sock);
