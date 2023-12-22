#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

int server_socket;                           // Declare server_socket at the file scope
int client_socket;                           // Declare client_socket at the file scope
struct sockaddr_in server_addr, client_addr; // Store information about sockets
socklen_t client_addr_len;                   // Store the size of the client address

/*
    This function is responsible for creating a socket server to be able to communicate with
    other processes
*/
void server_socket_create()
{
    // Creates a new socket with an IPv4 address
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed"); // If it's -1, socket creation failed
        exit(1);
    }

    server_addr.sin_family = AF_INET;                     // It's an IPv4 address
    server_addr.sin_port = htons(PORT);                   // Define the port you want to use
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Set the IP address to the loopback address

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 1) == 0)
    {
        printf("Server created successfully and waiting for a connection...\n");
    }
    else
    {
        perror("Listen failed");
        exit(1);
    }
}

/*
    This function is responsible for decoding the messages that it receives, receives an argument of type int
    and checks the code in a switch to check what kind of action need to be taken.
*/
void decode_message(int code[2])
{
    // Define static variables to keep track of counts
    static int child = 0;
    static int adult = 0;
    static int elder = 0;
    static int toboggan = 0;
    static int snack_bar = 0;

    switch (code[0])
    {
    case 100:
        printf("A person was created.                                                 \n");
        break;
    case 110:
        child++;
        break;
    case 120:
        adult++;
        break;
    case 130:
        elder++;
        break;
    case 140:
        printf("Entry into the park (%d)                                                 \n", code[1]);
        break;
    case 150:
        printf("Exited the park (%d)                                                     \n", code[1]);
        break;
    case 160:
        printf("Criança (Pessoa %d) entrou no escorrega                                           \n", code[1]);
        break;
    case 170:
        printf("Adulto (Pessoa %d) entrou no escorrega                                           \n", code[1]);
        break;
    case 180:
        printf("Idoso (Pessoa %d) entrou no escorrega                                           \n", code[1]);
        break;
    case 190:
    	toboggan++;
    	break;
    case 200:
    	snack_bar++;
    	break;
    default:
        printf("Unknown message code: %d                                              \n", code[0]);
        break;
    }

    // Print counts on the last line, \r makes it so that the cursor goes to the first line
    printf("Child: %d, Adult: %d, Elder: %d, Toboggan Uses: %d, Snacks Bought: %d \r",
           child, adult, elder, toboggan, snack_bar);

    // Flush the output to ensure it's displayed immediately
    fflush(stdout);
}

/*
    Function responsible for receiving messages for the other process, and check if
    the other process disconnected
*/
int check_client_disconnect()
{
    int code[2] = {};
    int bytes_received = recv(client_socket, &code, sizeof(code), 0);

    if (bytes_received <= 0)
    {
        // The client has disconnected or an error occurred, so exit the loop
        printf("\n");
        printf("Client disconnected or an error occurred. Exiting the loop.\n");
        return 0;
    }
    else
    {
        decode_message(code);
        return 1;
    }
}

/*
    This function is responsible for connecting to a client when receiving a request to connect
*/
int connect_client()
{
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}




