CC = gcc
CFLAGS = -Wall -Wextra

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c showIP.c createServerSocket.c dieWithClose.c acceptConnections.c communicate.c useThreads.c clientInit.c -lpthread

client: client.c
	$(CC) $(CFLAGS) -o client client.c createClientSocket.c setServerAddress.c dieWithClose.c functions.c

clean:
	rm -f server client
