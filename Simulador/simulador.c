/* simulador.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include "simulador.h"
#include "leitor_ficheiros.h"
#include "escrita_ficheiros.h"
#include "cliente.h"


//Mutex
pthread_mutex_t trinco_log; //access to log file

//semaphore
sem_t entry_into_park; //entry into the park



int main(int argc, char *argv[]){

    //create log text file name with the date and time from now
    char logFileName[30];  // Adjust the size as needed
    snprintf(logFileName, sizeof(logFileName), "Logs/logfile_%s.txt", getCurrentTimestamp()); //create name of log file

    //verify if an argument has been passed
    if(argc < 2){
        logMessage(logFileName, ERROR ,"Não foi passado o argumento de ficheiro de Configuração!");
        return 1;
    }

    //create struct to hold the configuration
    struct Simulador_config config;
    

    //try to read the configuration for the simulator
    if (readConfigFile(argv[1], &config) == 1) {
        logMessage(logFileName, ERROR ,"Erro de carregamento do ficheiro de Configuração!");
        return 0;
    }
    
    config.atraction_number = 5; //temporary while figuring out how to create dinamic mutex
    int client_socket = connect_server(); //get the client socket

    pthread_mutex_init(&trinco_log, NULL); //initialize the mutex
    sem_init(&entry_into_park, 0, config.max_people_park); //initialize the semaphore

    // Create a struct to pass to the Thread
    struct ThreadArgs args;
    args.client_socket = client_socket;
    args.log_filename = logFileName;
    args.config = config;

    //define time variables for the simulation
    time_t start_time = time(NULL);
    time_t current_time = start_time;


    pthread_t person[MAX_NUM_THREADS]; // create the maximum number of threads
    int i;
    for(i = 0; i < MAX_NUM_THREADS; i++){
    // Create a thread named "person"
        if (pthread_create(person + i, NULL, person_thread, &args) != 0) { //check if there was an error
            pthread_mutex_lock(&trinco_log);
            logMessage(logFileName, ERROR, "Thread creation failed");
            pthread_mutex_unlock(&trinco_log);
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
            pthread_mutex_lock(&trinco_log);
            logMessage(logFileName, ERROR, message);
            pthread_mutex_unlock(&trinco_log);
        }
    }


    pthread_mutex_destroy(&trinco_log);  //destroy the mutex
    sem_destroy(&entry_into_park); //destroy semaphore
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


// Determines the age group for the person
int determineAgeGroup(double probability_elder, double probability_child, unsigned int* seed) {
    double randomValue = ((double)rand_r(seed) / RAND_MAX) * 100.0;

    if (randomValue < probability_child) {
        return 0;
    } else if (randomValue < probability_child + probability_elder) {
        return 2;
    } else {
        return 1;
    }
}


//function to manage the entry intro the atractions
void manageAtractions(struct ThreadArgs * args){

}


//code executed by the thread
void* person_thread(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;  // Cast the argument to the appropriate structure

    // Writing to the log file the person was created
    pthread_mutex_lock(&trinco_log);
    logMessage(args->log_filename, ROUTINE, "Pessoa criada com sucesso");
    pthread_mutex_unlock(&trinco_log);

    // Get a unique seed for the random number generator (e.g., thread ID or current time)
    unsigned int seed = (unsigned int)pthread_self();
    srand(seed);

    // Generate a random faixa_etaria for each thread
    args->pessoa.faixa_etaria = determineAgeGroup(args->config.probability_being_elder, args->config.probability_being_child, &seed);

    sem_wait(&entry_into_park); //semaphore that handles how many people can enter the park
    send_message(140, args->client_socket);
    sleep(3);
    sem_post(&entry_into_park);
    send_message(150, args->client_socket);

    /*
    // Use the pessoa information in your logic
    switch (pessoa.faixa_etaria) {
    case 0:
        // Handle behavior for children
        send_message(110, args->client_socket);
        break;
    case 1:
        // Handle behavior for adults
        send_message(120, args->client_socket);
        break;
    case 2:
        // Handle behavior for the elderly
        send_message(130, args->client_socket);
        break;
    default:
        break;
    }
    */

    sleep(3);
    return NULL;
}


