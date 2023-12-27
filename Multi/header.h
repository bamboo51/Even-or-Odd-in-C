#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define MAX_CLIENTS 4
#define PORT 10000
#define BUFSIZE 32

void DieWithError(char *errorMessage){
    perror(errorMessage);
    exit(EXIT_FAILURE);
}