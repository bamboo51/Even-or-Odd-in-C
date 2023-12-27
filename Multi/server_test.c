#include "header.h"
#include <ifaddrs.h>
#include <netinet/in.h>

int maxClients;

void showIP(void);

int main(int argc, char *argv[]){
    if(argc == 1){
        maxClients = MAX_CLIENTS;
    }else{
        maxClients = atoi(argv[1]);
    }
    showIP();
}

void showIP(void){
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;

    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *)ifa->ifa_addr;
            if (strcmp(ifa->ifa_name, "eth0") == 0) {
                printf("%s\n", inet_ntoa(sa->sin_addr));
                break; // Stop after finding the first match (eth0)
            }
        }
    }

    freeifaddrs(ifap);
}