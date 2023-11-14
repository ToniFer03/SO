#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

int server_socket; // Declare server_socket at the file scope
int client_socket; // Declare client_socket at the file scope
struct sockaddr_in server_addr, client_addr; // Store information about sockets
socklen_t client_addr_len; // Store the size of the client address


void server_socket_create() {
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
}

//code to connect to the client
int connect_client(){
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        return -1;
    }
    else {
        return 1;
    }
}


// Function to decode and handle messages
void decode_message(int code) {
    // Define static variables to keep track of counts
    static int child = 0;
    static int adult = 0;
    static int elder = 0;

    switch (code) {
        case 100:
            printf("A person was created.                                                 \n");
            break;
        case 110:
            printf("The person is a child.                                        \n");
            child++;
            break;
        case 120:
            printf("The person is an Adult.                                          \n");
            adult++;
            break;
        case 130:
            printf("The person is an Elder.                                             \n");
            elder++;
            break;
        default:
            printf("Unknown message code: %d                                              \n", code);
            break;
    }

    // Print counts on the last line
    printf("Child: %d, Adult: %d, Elder: %d \r", 
        child, adult, elder);

    // Flush the output to ensure it's displayed immediately
    fflush(stdout);
}


// Inside the check_client_disconnect function
int check_client_disconnect() {
    int code = 0;
    int bytes_received = recv(client_socket, &code, sizeof(code), 0);

    if (bytes_received <= 0) {
        // The client has disconnected or an error occurred, so exit the loop
        printf("Client disconnected or an error occurred. Exiting the loop.\n");
        return 0;
    } else {
        decode_message(code);
        return 1;
    }
}

