CC = gcc
CFLAGS = -Wall -Wextra

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c showIP.c createServerSocket.c DieWithClose.c acceptConnections.c communicate.c useThreads.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client
