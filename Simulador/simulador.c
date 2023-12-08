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

// semaphore
sem_t entry_into_park;    // entry into the park
sem_t sem_priority_elder; // semaphore for elder priority
sem_t sem_priority_adult; // semaphore for adult priority
sem_t sem_priority_child; // semaphore for child priority

// mutex
pthread_mutex_t access_familyWaterSlide;

int main(int argc, char *argv[])
{

    // create log text file eith the name containig date and time from now
    char logFileName[40];                                                                     // Adjust the size as needed
    snprintf(logFileName, sizeof(logFileName), "Logs/logfile_%s.txt", getCurrentTimestamp()); // create name of log file

    FILE *logfile = openFile(logFileName); // function to open the logfile

    // verify if an argument has been passed
    if (argc < 2)
    {
        logMessage(logfile, ERROR, "Não foi passado o argumento de ficheiro de Configuração!");
        return 1;
    }

    // create struct to hold the configuration
    struct Simulador_config config;

    // try to read the configuration for the simulator
    if (readConfigFile(argv[1], &config) == 1)
    {
        logMessage(logfile, ERROR, "Erro de carregamento do ficheiro de Configuração!");
        return 0;
    }

    config.atraction_number = 3;          // temporary while figuring out how to create dinamic mutex
    int client_socket = connect_server(); // get the client socket

    sem_init(&entry_into_park, 0, config.max_people_park); // initialize the semaphore
    sem_init(&sem_priority_child, 0, 1);
    sem_init(&sem_priority_adult, 0, 1);
    sem_init(&sem_priority_elder, 0, 1);

    pthread_mutex_init(&access_familyWaterSlide, NULL);

    // Create a struct to pass to the Thread
    struct ThreadArgs args;
    args.client_socket = client_socket;
    args.logfile = logfile;
    args.config = config;

    // define time variables for the simulation
    time_t start_time = time(NULL);
    time_t current_time = start_time;

    pthread_t person[MAX_NUM_THREADS]; // create the maximum number of threads
    int i;
    /*
        This loop will be in charge of creating the number of threads at the beggining,
        the MAX_NUM_THREADS is defined in the header file of simulador. In case there is an error it
        will use the logMessage to print that there was an error.
        Also uses Mutex to make sure only a thread at a time has access to the file
    */
    for (i = 0; i < MAX_NUM_THREADS; i++)
    {
        // Create a thread named "person"
        if (pthread_create(person + i, NULL, person_thread, &args) != 0)
        { // check if there was an error
            logMessage(logfile, ERROR, "Thread creation failed");
            exit(1);
        }
    }

    /*
        This second loop is in charge of closing the threads down, if everythings goes down fine it will
        not do anything. In case there is an error follow a similar logic to the one above and prints
        that there was an error, aswell as the number of the thread responsible for it
    */
    for (i = 0; i < MAX_NUM_THREADS; i++)
    {
        // Wait for the thread to finish
        if (pthread_join(person[i], NULL) != 0)
        {
            // Get the thread ID
            pthread_t thread_id = person[i];

            // Convert the thread ID to a string for printing
            char thread_id_str[20]; // Adjust the size as needed
            snprintf(thread_id_str, sizeof(thread_id_str), "%lu", thread_id);

            char message[100];
            snprintf(message, sizeof(message), "Thread finished with an error. Thread ID: %s", thread_id_str);

            // Log the error message with the thread ID
            logMessage(logfile, ERROR, message);
        }
    }

    closeFile(logfile);

    // deleting semaphores
    sem_destroy(&entry_into_park); // destroy semaphore
    sem_destroy(&sem_priority_child);
    sem_destroy(&sem_priority_adult);
    sem_destroy(&sem_priority_elder);

    // deleting mutex
    pthread_mutex_destroy(&access_familyWaterSlide);

    return 0;
}

/*
    Function responsible getting the current time and date, for the log file name
    and every entry of it. So that we can now when everything happened
*/
const char *getCurrentTimestamp()
{
    static char timestamp[20];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", timeinfo);

    return timestamp;
}

/*
    Function responsible for generating a random Number between 0 and a specific number
    that we send to the funcion
*/
int getRandomNumber(int limit)
{
    return (rand() % limit);
}

/*
    This function is used to determine the age of a certain person based on the probabilities
    that where writen on the config file. It receives as pointers the args, that contain the
    simulator config that was read from the file and a Struct of type Person_info where it
    will set the faixa_etaria on it (an enum type of Child = 0, Adult = 1, and Elder = 2)
    at random based on the probabilities given
*/
void determineAgeGroup(struct ThreadArgs *args, struct Person_info *pessoa_thread)
{
    double randomValue = ((double)rand() / RAND_MAX) * 100.0;

    if (randomValue < args->config.probability_being_child)
    {
        pessoa_thread->faixa_etaria = 0;
    }
    else if (randomValue < args->config.probability_being_elder + args->config.probability_being_child)
    {
        pessoa_thread->faixa_etaria = 2;
    }
    else
    {
        pessoa_thread->faixa_etaria = 1;
    }
};

/*
    This funtion handles the logic of the water park, it will give priority to elderly, then children
    and finally the adults to enter the water slide
*/
void familyWaterSlide(struct Person_info pessoa_thread, int clientSocket)
{
    if (pessoa_thread.faixa_etaria == 0) // criança
    {
        sem_wait(&sem_priority_child);
    }
    else if (pessoa_thread.faixa_etaria == 2) // idoso
    {
        sem_wait(&sem_priority_elder);
    }
    else // adulto
    {
        sem_wait(&sem_priority_adult);
    }

    pthread_mutex_lock(&access_familyWaterSlide);

    if (pessoa_thread.faixa_etaria == 0) // criança
    {
        send_message(160, clientSocket);
    }
    else if (pessoa_thread.faixa_etaria == 1) // adulto
    {
        send_message(170, clientSocket);
    }
    else // idoso
    {
        send_message(180, clientSocket);
    }

    pthread_mutex_unlock(&access_familyWaterSlide);

    if (pessoa_thread.faixa_etaria == 0) // criança
    {
        sem_post(&sem_priority_child);
    }
    else if (pessoa_thread.faixa_etaria == 2) // idoso
    {
        sem_post(&sem_priority_elder);
    }
    else // adulto
    {
        sem_post(&sem_priority_adult);
    }
}

// function to manage the entry intro the atractions
void manageAtractions(struct Person_info pessoa_thread, int clientSocket)
{
    familyWaterSlide(pessoa_thread, clientSocket); // calls the fucntion of the logic of the water slide
}

/*
    The code in here represents everything that will be executed by the threads. It receives
    an argument of the struct type ThreadArg, that contains the simulator config as well as
    the name of the log files to be writen and the client socket to be able to send messages
*/
void *person_thread(void *arg)
{
    struct ThreadArgs *args = (struct ThreadArgs *)arg; // Cast the argument to the appropriate structure
    struct Person_info pessoa_thread;                   // Create the person_info to hold information about the person

    // Writing to the log file the person was created
    logMessage(args->logfile, ROUTINE, "Pessoa criada com sucesso");

    /*
        This part of the code creates a unique seed for every thread so that every
        each time one of them calls rand it gives a different result. The seed is the thread_id
        so that it is different on every thread
    */
    unsigned int seed = (unsigned int)pthread_self();
    srand(seed);

    determineAgeGroup(args, &pessoa_thread); // Call the function to assign the person an age group

    // Test the semaphore that will only let 200 people enter the park at a single time
    sem_wait(&entry_into_park);
    // send_message(140, args->client_socket); // Message that a person entered the park
    sleep(3);
    manageAtractions(pessoa_thread, args->client_socket);
    sem_post(&entry_into_park);
    // send_message(150, args->client_socket); // Message that a person left the park

    return NULL;
}
