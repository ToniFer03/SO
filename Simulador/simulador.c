/* simulador.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "simulador.h"
#include "leitor_ficheiros.h"
#include "escrita_ficheiros.h"
#include "cliente.h"


int main(int argc, char *argv[]){

    //create log text file name with the date and time from now
    char logFileName[30];  // Adjust the size as needed
    snprintf(logFileName, sizeof(logFileName), "Logs/logfile_%s.txt", getCurrentTimestamp());

    //verify if an argument has been passed
    if(argc < 2){
        logMessage(logFileName, ERROR ,"Não foi passado o argumento de ficheiro de Configuração!");
        return 1;
    }

    //create struct to hold the configuration
    struct Simualador_config config;
    

    //try to read the configuration for the simulator
    if (readConfigFile(argv[1], &config) == 1) {
        logMessage(logFileName, ERROR ,"Erro de carregamento do ficheiro de Configuração!");
        return 0;
    }


    // Use the configuration values
    printf("Probability_Being_Elder: %.2f%%\n", config.probability_being_elder);
    printf("Probability_Being_Child: %.2f%%\n", config.probability_being_child);

    
    int client_socket = connect_server();

    // Create a struct to hold the client socket pointer
    struct ThreadArgs args;
    args.client_socket = client_socket;
    args.log_filename = logFileName;

    //define time variables for the simulation
    time_t start_time = time(NULL);
    time_t current_time = start_time;

    pthread_t person[MAX_NUM_THREADS];
    
    int i;
    for(i = 0; i < MAX_NUM_THREADS; i++){
        // Create a thread named "person"
        if (pthread_create(person + i, NULL, person_thread, &args) != 0) { //check if there was an error
            logMessage(logFileName, ERROR, "Thread creation failed");
            exit(1);
        }
    }

    for (i = 0; i < MAX_NUM_THREADS; i++) {
        // Wait for the thread to finish
        if (pthread_join(person[i], NULL) != 0) {
            // Get the thread ID
            pthread_t thread_id = person[i];

            // Convert the thread ID to a string for printing
            char thread_id_str[20]; // Adjust the size as needed
            snprintf(thread_id_str, sizeof(thread_id_str), "%lu", thread_id);

            char message[100];
            snprintf(message, sizeof(message), "Thread finished with an error. Thread ID: %s", thread_id_str);

            // Log the error message with the thread ID
            logMessage(logFileName, ERROR, message);
        }
    }


    return 0;
    
}

// Function to get the current timestamp
const char *getCurrentTimestamp() {
    static char timestamp[20];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", timeinfo);

    return timestamp;
}

//generates a random number between 0 and the limit
int getRandomNumber(int limit){
    return (rand()%limit);
} 


//code executed by the thread
void* person_thread(void* arg) {
    // Cast the argument to the appropriate structure
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Extract information about the person
    struct Person_info pessoa = args->Pessoa;
    logMessage(args->log_filename, ROUTINE, "Pessoa criada com sucesso");


    // Get a unique seed for the random number generator (e.g., thread ID or current time)
    unsigned int seed = (unsigned int)pthread_self();

    // Seed the random number generator
    srand(seed);

    // Generate a random faixa_etaria for each thread
    pessoa.faixa_etaria = getRandomNumber(3); // Assuming 3 age groups: CRIANCA, ADULTO, IDOSO

    // Use the pessoa information in your logic
    switch (pessoa.faixa_etaria) {
    case CRIANCA:
        // Handle behavior for children
        send_message(110, args->client_socket);
        break;
    case ADULTO:
        // Handle behavior for adults
        send_message(120, args->client_socket);
        break;
    case IDOSO:
        // Handle behavior for the elderly
        send_message(130, args->client_socket);
        break;
    default:
        break;
    }

    sleep(3);
    return NULL;
}


