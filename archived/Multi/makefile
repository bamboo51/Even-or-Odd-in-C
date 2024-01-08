CC = gcc
CFLAGS = -Wall -Wextra

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c -pthread

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client
