#ifndef SERVER_H
#define SERVER_H

#define PORT 9370 // Defines the port that will listen for the connection

// Definition of functions
void server_socket_create();
int connect_client();
int check_client_disconnect();
void decode_message(int code[2]);

#endif
