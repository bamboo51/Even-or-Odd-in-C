#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

struct sockaddr_in setServerAddress(char *serverIP, unsigned short port) {
    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family        = AF_INET;
    serverAddress.sin_addr.s_addr   = inet_addr(serverIP);
    serverAddress.sin_port          = htons(port);

    return serverAddress;
}