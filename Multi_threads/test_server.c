#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_count = 0;
int clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_message(char *message, int current_client) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] != current_client) {
            send(clients[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    char ready_message[] = "ready";
    char numbers_message[BUFFER_SIZE];

    // Wait for "ready" message from all clients
    int all_clients_ready = 0; // Flag to track if all clients are ready

    while (!all_clients_ready) {
        all_clients_ready = 1; // Assume all clients are ready

        for (int i = 0; i < client_count; i++) {
            recv(clients[i], buffer, BUFFER_SIZE, 0);
            if (strcmp(buffer, ready_message) != 0) {
                all_clients_ready = 0;
                break;
            }
        }
    }

    printf("All clients are ready\n");

    // Generate and send random numbers to all clients
    int number1 = rand() % 100;
    int number2 = rand() % 100;
    sprintf(numbers_message, "%d %d", number1, number2);
    send_message(numbers_message, client_socket);

    // Wait for "ready" message from client
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (strcmp(buffer, ready_message) == 0) {
        printf("Client %d is ready\n", client_socket);
    }

    // Clean up and close the client socket
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client_socket) {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    pthread_t tid;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // Bind server socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to bind server socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for clients...\n");

    while (1) {
        socklen_t client_address_length = sizeof(client_address);

        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Failed to accept client connection");
            exit(EXIT_FAILURE);
        }

        // Add client socket to the clients array
        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = client_socket;
            printf("Client %d connected\n", client_socket);

            // Create a new thread to handle the client
            pthread_create(&tid, NULL, handle_client, &clients[client_count - 1]);
        } else {
            printf("Maximum number of clients reached. Connection rejected.\n");
            close(client_socket);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    return 0;
}
