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
void decode_message(int code[4])
{
    // Define static variables to keep track of counts
    static int quit_Toboggan = 0;
    static int quit_Snack_bar = 0;
    static int quit_FamilyWaterSlide = 0;
    static int used_Toboggan = 0;
    static int used_Snack_bar = 0;
    static int used_FamilyWaterSlide = 0;

    if(code[0] == 0) // Code that means its related to atractions
    {
        switch (code[1]) // Switch that refers to an attraction in particular
        {
        case 0:
            if(code[2] == 0) // If its 0, the person used the attraction
            {
                printf("A person (%d) used the Family Waterslide                                      \n", code[3]); // Code 3 refers to the person ID
                used_FamilyWaterSlide += 1;
            } else { // Else it quit waiting to use it
                printf("A person (%d) waited too much time and quit using the Family Waterslide       \n", code[3]);
                quit_FamilyWaterSlide += 1;
            }
            break;
        
        case 1:
            if(code[2] == 0) // If its 0, the person used the attraction
            {
                printf("A person (%d) used the Toboggan                                               \n", code[3]); // Code 3 refers to the person ID
                used_Toboggan += 1;
            } else { // Else it quit waiting to use it
                printf("A person (%d) waited too much time and quit using the Toboggan                \n", code[3]);
                quit_Toboggan += 1;
            }
            break;
        
        case 2:
            if(code[2] == 0) // If its 0, the person used the attraction
            {
                printf("A person (%d) used the Snackbar                                               \n", code[3]); // Code 3 refers to the person ID
                used_Snack_bar += 1;
            } else { // Else it quit waiting to use it
                printf("A person (%d) waited too much time and quit using the Snackbar                \n", code[3]);
                quit_Snack_bar += 1;
            }
            break;

        default:
            break;
        }
    }

    // Prints the counts of every atraction on the terminal
    printf("                                                                            \n");
    printf("Number of people that used an atraction:                                    \n");
    printf("Family Waterslide:..................%d                                      \n", used_FamilyWaterSlide);
    printf("Toboggan:...........................%d                                      \n", used_Toboggan);
    printf("Snackbar:...........................%d                                      \n", used_Snack_bar);
    printf("Number of people who quit while on line:                                    \n");
    printf("Family Waterslide:..................%d                                      \n", quit_FamilyWaterSlide);
    printf("Toboggan:...........................%d                                      \n", quit_Toboggan);
    printf("Snackbar:-------------------------- %d                                      \n",quit_Snack_bar);
    printf("\033[9A\033[0G\033[?25l"); // This print puts the cursor 9 lines up and on the first caracter of the line

    // Flush the output to ensure it's displayed immediately
    fflush(stdout);
}

/*
    Function responsible for receiving messages for the other process, and check if
    the other process disconnected
*/
int check_client_disconnect()
{
    int code[4] = {};
    int bytes_received = recv(client_socket, &code, sizeof(code), 0);

    if (bytes_received <= 0)
    {
        // The client has disconnected or an error occurred, so exit the loop
        printf("\033[999;999H");
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




