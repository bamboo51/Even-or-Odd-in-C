#include <stdbool.h>

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
};