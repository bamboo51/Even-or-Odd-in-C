#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void DieWithClose(char *errorMessage, int sock){
    perror(errorMessage);
    close(sock);
    exit(EXIT_FAILURE);
}