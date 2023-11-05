#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"
#include "server.h"


int main() {
    server_socket_create();
    int connection_status = connect_client();

    if (connection_status == 1){
        printf("Good Connection \n");
        fflush(stdout); // Flush the output buffer to ensure the message is displayed

        while (connection_status){ //while there is a connection continues
            connection_status = check_client_disconnect();
        }
        

    } else {
        printf("Connection failed \n");
        exit(-1);
    }
    

    return 0;
}
