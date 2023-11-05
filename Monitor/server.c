#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"


void server_socket_create() {
    int server_socket; // Store the file descriptor of the server
    int client_socket; // Store the file descriptor of the client
    struct sockaddr_in server_addr, client_addr; // Store information about sockets
    socklen_t client_addr_len; // Store the size of the client address

    // Creates a new socket with an IPv4 address
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed"); // If it's -1, socket creation failed
        exit(1);
    }

    server_addr.sin_family = AF_INET; // It's an IPv4 address
    server_addr.sin_port = htons(PORT); // Define the port you want to use
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Set the IP address to the loopback address

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 1) == 0) {
        printf("Server created successfully and waiting for a connection...\n");
    } else {
        perror("Listen failed");
        exit(1);
    }

    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("Client connection failed");
        exit(1);
    }
}