#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cliente.h"
#include "escrita_ficheiros.h"

/*
    Function responsible for connection to the server
*/
int connect_server()
{
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);                   // Use the same port as the server
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR); // Replace with the server's IP address

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        exit(1);
    }

    return client_socket; // returns the client socket so that it can send messages
}


/*
    Funtion responsible for sending the codes for the server to decode. It receives the code to be send
    (and INT) and the socket to send it too.
*/
void send_message(int codeMessage, int atractionNumber, int quitAtraction, int id, int client_socket)
{
    int code[4] = {codeMessage, atractionNumber, quitAtraction, id};

    // Send the integer code to the server
    int bytes_sent = send(client_socket, &code, sizeof(code), 0);
}
