/* simulador.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "simulador.h"
#include "leitor_ficheiros.h"
#include "escrita_ficheiros.h"
#include "cliente.h"

// Define a struct to hold the client socket
struct ThreadArgs {
    int client_socket;
};


int main(int argc, char *argv[]){
    /*
    if(argc < 2){
        printf("Necessário passar como argumento um ficheiro de configuração!\n");
        return 1;
    }

    if(load_initial_config_simulador(argv[1]) == 1){
        printf("Programa teve de encerrar devido a um erro ao carregar o ficheiro de configuração ");
        printf("do simulador.\n");
        return 0;
    }
    write_to_file();
    */

    int client_socket = connect_server();

    // Create a struct to hold the client socket pointer
    struct ThreadArgs args;
    args.client_socket = client_socket;


    while (1){ //keep simulador running
        pthread_t person; //initialize pthread_t person

        // Create a thread named "person"
        if (pthread_create(&person, NULL, person_thread, &args) != 0) {
            perror("Thread creation failed");
            exit(1);
        }

        // Wait for the "person" thread to finish (you can add your logic here)

        pthread_join(person, NULL);
    }
    

    return 0;
}


//code executed by the thread
void* person_thread(void* arg) {
    printf("Person thread created.\n");
    int codeMessage = 100; //code that a person was created
    int client_socket = *((int*)arg);

    send_message(codeMessage, client_socket);
    // Perform the behavior of the person here
    sleep(3);
    return NULL;
}
