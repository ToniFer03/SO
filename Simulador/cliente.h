#ifndef CLIENTE_H
#define CLIENTE_H

#define PORT 9370 //defines the port of the connection
#define SERVER_ADDR "127.0.0.1" //defines the server address as the loopback

int connect_server();
void send_message(int codeMessage, int client_socket);

#endif // CLIENTE_H
