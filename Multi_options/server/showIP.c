#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void showIP(void) {
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
                printf("Server: %s:", inet_ntoa(sa->sin_addr));
                break; // Stop after finding the first match (eth0)
            }
        }
    }

    freeifaddrs(ifap);
}