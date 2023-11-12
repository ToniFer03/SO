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


//generates a random number between 0 and the limit
int getRandomNumber(int limit){
    srand(time(NULL));
    return (rand()%limit);
} 


//code executed by the thread
void* person_thread(void* arg) {
    printf("Person thread created.\n");
    int codeMessage = 100; //code that a person was created
    int client_socket = *((int*)arg);

    send_message(codeMessage, client_socket);
    
    switch (getRandomNumber(5)){
    case 0:
        send_message(110, client_socket);
        break;
    case 1:
        send_message(120, client_socket);
        break;
    case 2:
        send_message(130, client_socket);
        break;
    case 3:
        send_message(140, client_socket);
        break;
    case 4:
        send_message(150, client_socket);
        break;
    default:
        break;
    }

    sleep(3);
    return NULL;
}
