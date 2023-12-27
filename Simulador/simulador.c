/* simulador.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "simulador.h"
#include "leitor_ficheiros.h"
#include "escrita_ficheiros.h"
#include "cliente.h"

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

// semaphore
sem_t entry_into_park;    // entry into the park
sem_t sem_priority_elder; // semaphore for elder priority
sem_t sem_priority_adult; // semaphore for adult priority
sem_t sem_priority_child; // semaphore for child priority
sem_t sem_toboggan;
sem_t sem_snack_bar;
sem_t snack_empty;
sem_t snack_full;
sem_t sem_water_polo;

// mutex
pthread_mutex_t access_familyWaterSlide;
pthread_mutex_t access_toboggan;
pthread_mutex_t snack;
pthread_mutex_t lock;
pthread_mutex_t water_polo_lock;

// Global variable
int nu = 1;
int players_water_polo = 0;


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

    int client_socket = connect_server(); // get the client socket
    calculateTimeScale(&config, client_socket);

    // initialize the semaphore
    sem_init(&entry_into_park, 0, config.max_people_park);
    sem_init(&sem_priority_child, 0, 1);
    sem_init(&sem_priority_adult, 0, 1);
    sem_init(&sem_priority_elder, 0, 1);
    sem_init(&sem_toboggan, 0, config.max_people_toboggan);
    sem_init(&sem_snack_bar, 0, config.max_people_snack_bar);
    sem_init(&snack_empty, 0, 10);
    sem_init(&snack_full, 0, 0);
    sem_init(&sem_water_polo, 0, 14);

    // initialize the mutex_locks
    pthread_mutex_init(&access_familyWaterSlide, NULL);
    pthread_mutex_init(&access_toboggan, NULL);
    pthread_mutex_init(&snack, NULL);
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&water_polo_lock, NULL);

    // Create a struct to pass to the Thread
    struct ThreadArgs args;
    args.client_socket = client_socket;
    args.logfile = logfile;
    args.config = config;
    args.start_time = time(NULL);

    printf("Start time: %ld\n", args.start_time);
    pthread_t person[config.threads_to_create]; // create the maximum number of threads
    pthread_t barista[1];
    int i;

    // Create a thread named "barista"
    if (pthread_create(barista, NULL, barista_thread, &args) != 0)
    { // check if there was an error
        logMessage(logfile, ERROR, "Thread creation failed");
        exit(1);
    }


    /*
        This loop will be in charge of creating the number of threads at the beggining,
        the MAX_NUM_THREADS is defined in the header file of simulador. In case there is an error it
        will use the logMessage to print that there was an error.
        Also uses Mutex to make sure only a thread at a time has access to the file
    */

    for (i = 0; i < config.threads_to_create; i++)
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
    for (i = 0; i < config.threads_to_create; i++)
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

    send_message(999, -1, -1, -1, client_socket); // Message that says the simulation has ended

    closeFile(logfile);

    // deleting semaphores
    sem_destroy(&entry_into_park);
    sem_destroy(&sem_priority_child);
    sem_destroy(&sem_priority_adult);
    sem_destroy(&sem_priority_elder);
    sem_destroy(&sem_toboggan);
    sem_destroy(&sem_snack_bar);
    sem_destroy(&snack_empty);
    sem_destroy(&snack_full);
    sem_destroy(&sem_water_polo);

    // deleting mutex
    pthread_mutex_destroy(&access_familyWaterSlide);
    pthread_mutex_destroy(&access_toboggan);
    pthread_mutex_destroy(&snack);
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&water_polo_lock);

    return 0;
}


/*
    Function responsible getting the current time and date, for the log file name
    and every entry of it. So that we can know when everything happened
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
    This function calculates how much each second represents in real time and sends it to the monitor.
    To note it uses, integer division
*/
void calculateTimeScale(struct Simulador_config * config, int client_socket){
    int temp_real_time = config->time_being_simulated * 3600; //turns the hours into seconds
    send_message(100, temp_real_time/config->simulation_duration, -1, -1, client_socket);
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
    Function that will send the info on how much time each person spent doing a certain activity
*/
void send_time_monitor(int code0, int code1, struct timeval starttime, struct timeval endtime, int client_socket)
{
    struct timeval diff;
    timersub(&endtime, &starttime, &diff); // calculate difference and put it on the var diff
    send_message(code0, code1, 0, diff.tv_sec, client_socket); // Sends the diff in seconds
    send_message(code0, code1, 1, diff.tv_usec, client_socket); // Sends the diff in microseconds
    
}


/*
    This funtion handles the logic of the water park, it will give priority to elderly, then children
    and finally the adults to enter the water slide
*/
void familyWaterSlide(struct Person_info pessoa_thread, int clientSocket)
{
    gettimeofday(&pessoa_thread.time_start_waiting_famwaterslide, NULL); //Get the time person entered line
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
    gettimeofday(&pessoa_thread.time_exit_waiting_famWaterslide, NULL); //Get the time person exited the line

    if (pessoa_thread.faixa_etaria == 0) // criança
    {
        send_message(0, 2, 0, pessoa_thread.id, clientSocket);
        send_time_monitor(1, 5, pessoa_thread.time_start_waiting_famwaterslide, pessoa_thread.time_exit_waiting_famWaterslide, clientSocket);
    }
    else if (pessoa_thread.faixa_etaria == 2) // Idoso
    {
        send_message(0, 2, 1, pessoa_thread.id, clientSocket);
        send_time_monitor(1, 6, pessoa_thread.time_start_waiting_famwaterslide, pessoa_thread.time_exit_waiting_famWaterslide, clientSocket);
    }
    else // Adulto
    {
        send_message(0, 2, 2, pessoa_thread.id, clientSocket);
        send_time_monitor(1, 7, pessoa_thread.time_start_waiting_famwaterslide, pessoa_thread.time_exit_waiting_famWaterslide, clientSocket);
    }

    usleep(100000); 
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


void toboggan(struct Person_info pessoa_thread, int clientSocket)
{
    bool waited_on_line = false;

    // if sem_trywait returns a non-zero value, it means the semaphore is zero (the queue is full), so the person leaves.
    if (sem_trywait(&sem_toboggan) != 0)
    {
        printf("Person %d left the toboggan (Queue is full).\n", pessoa_thread.id);
        send_message(0, 1, 1, pessoa_thread.id, clientSocket); // Person gave up on toboggan
        return;
    }

    // If pthread_mutex_trylock returns a non-zero value, it means the mutex is locked (the toboggan is in use), so the person waits.
    if (pthread_mutex_trylock(&access_toboggan) != 0)
    {
        int giveUpChance = getRandomNumber(99);
        if (giveUpChance > pessoa_thread.patience)
        {
            printf("Person %d left the toboggan (Gave up).\n", pessoa_thread.id);
            send_message(0, 1, 1, pessoa_thread.id, clientSocket); // Person gave up on toboggan
            sem_post(&sem_toboggan);
            return;
        }
        printf("Person %d is waiting for the toboggan.\n", pessoa_thread.id);
        gettimeofday(&pessoa_thread.time_start_waiting_toboggan, NULL);
        waited_on_line = true;
        pthread_mutex_lock(&access_toboggan);
    } 


    // There is a problem that if the toboggan is not in use, it send 0 as start time and sends the end time, so the 
    // time in the other side is wrong

    printf("Person %d is using the toboggan.\n", pessoa_thread.id);

    if(waited_on_line){
        // Sent time waiting in line for toboggan
        gettimeofday(&pessoa_thread.time_exit_waiting_toboggan, NULL);
        send_time_monitor(1, 2, pessoa_thread.time_start_waiting_toboggan, pessoa_thread.time_exit_waiting_toboggan, clientSocket);
    } 
    else // In the case the person didnt wait, it just sends 0 as the time on line
    {
        send_time_monitor(1, 2, pessoa_thread.time_start_waiting_toboggan, pessoa_thread.time_start_waiting_toboggan, clientSocket);
    }

    usleep(200000); // sleep for 0.5s
    printf("Person %d is done using the toboggan.\n", pessoa_thread.id);
    send_message(0, 1, 0, pessoa_thread.id, clientSocket); // Person used the toboggan

    pthread_mutex_unlock(&access_toboggan);
    sem_post(&sem_toboggan);

}


void snack_bar(struct Person_info pessoa_thread, int clientSocket)
{
    /*
        If there are people on the queue, for the snackbar, the patience of the person
        is tested to see if it will give up or not
    */
    if (sem_trywait(&sem_snack_bar) != 0)
    {
        int giveUpChance = getRandomNumber(99);
        if (giveUpChance > pessoa_thread.patience)
        {
            printf("Person %d left the bar (Gave up).\n", pessoa_thread.id);
            send_message(0, 3, 1, pessoa_thread.id, clientSocket); // Message the person gave up
            return;
        }
    }

    gettimeofday(&pessoa_thread.time_start_waiting_snack, NULL);
    printf("Person %d is waiting at the bar.\n", pessoa_thread.id); // Print when a person is waiting

    sem_wait(&snack_full);
    pthread_mutex_lock(&snack);
    int drink = buffer[out];
    out = (out + 1) % BUFFER_SIZE; // This makes it circular
    send_message(0, 3, 0, pessoa_thread.id, clientSocket); // Message the person got a snack
    
    // Send the wait time for the snack
    gettimeofday(&pessoa_thread.time_exit_waiting_snack, NULL);
    send_time_monitor(1, 4, pessoa_thread.time_start_waiting_snack, pessoa_thread.time_exit_waiting_snack, clientSocket);
    
    printf("Person %d got a snack.\n", pessoa_thread.id);
    
    pthread_mutex_unlock(&snack);
    sem_post(&snack_empty);
    sem_post(&sem_snack_bar);
}


void water_polo(struct Person_info pessoa_thread, int clientSocket)
{
    // Signal that a player has entered
    sem_wait(&sem_water_polo);

    int temp_entry_time = time(NULL); // Get time person entered the waterpolo queue for a team (for timeout porpouses)
    gettimeofday(&pessoa_thread.time_start_waiting_waterpolo, NULL); // For statistics

    pthread_mutex_lock(&water_polo_lock);
    players_water_polo++;
    pthread_mutex_unlock(&water_polo_lock);

    // While waiting for 14 players
    while (players_water_polo < 14)
    {
        if(temp_entry_time + 5 < time(NULL))
        {
            sem_post(&sem_water_polo);
            pthread_mutex_lock(&water_polo_lock);
            players_water_polo--;
            pthread_mutex_unlock(&water_polo_lock);
            send_message(0, 4, 1, pessoa_thread.id, clientSocket); // Didnt play waterpolo
            return;
        }
        else {
            usleep(50000); //sleep for better performance
            continue;
        }
    }

    gettimeofday(&pessoa_thread.time_exit_waiting_waterpolo, NULL);
    send_time_monitor(1, 8, pessoa_thread.time_start_waiting_waterpolo, pessoa_thread.time_exit_waiting_waterpolo, clientSocket);

    usleep(100000); // Time to simulate using the atraction
    send_message(0, 4, 0, pessoa_thread.id, clientSocket);
    pthread_mutex_lock(&water_polo_lock);
    players_water_polo--;
    pthread_mutex_unlock(&water_polo_lock);

    // Signal that a player has exited
    sem_post(&sem_water_polo);
}


// function to manage the entry intro the atractions
void manageAtractions(struct Person_info pessoa_thread, int clientSocket)
{
    bool stay_on_park = true;
    int nextAtraction = 0;

    // When all atractions are visited, the person can exit de park
    while (stay_on_park)
    {
        // Will keep giving a random atraction until its one that he hasnt visited
        do
        {
            nextAtraction = rand() % 4;
        } while (pessoa_thread.visited_Atractions[nextAtraction]);

        // Sends the person to the atraction that was choosen
        switch (nextAtraction)
        {
        case 0:
            send_message(2, 2, 0, pessoa_thread.id, clientSocket); // Person entered family waterslide
            familyWaterSlide(pessoa_thread, clientSocket);
            send_message(2, 2, 1, pessoa_thread.id, clientSocket); // Person exited family waterslide
            pessoa_thread.visited_Atractions[0] = true;
            break;
        case 1:
            send_message(2, 1, 0, pessoa_thread.id, clientSocket); // Person entered toboggan
            toboggan(pessoa_thread, clientSocket);
            send_message(2, 1, 1, pessoa_thread.id, clientSocket); // Person exited toboggan
            pessoa_thread.visited_Atractions[1] = true;
            break;
        case 2:
            send_message(2, 3, 0, pessoa_thread.id, clientSocket); // Person entered Snackbar
            snack_bar(pessoa_thread, clientSocket);
            send_message(2, 3, 1, pessoa_thread.id, clientSocket); // Person exited Snackbar
            pessoa_thread.visited_Atractions[2] = true;
            break;
        case 3:
            send_message(2, 4, 0, pessoa_thread.id, clientSocket); // Person entered the waterpolo
            water_polo(pessoa_thread, clientSocket);
            send_message(2, 4, 1, pessoa_thread.id, clientSocket); // Person exited the waterpolo
            pessoa_thread.visited_Atractions[3] = true;
        default:
            break;
        }

        // Code that checks if the person has visited every atraction, if so exits the park
        if(!pessoa_thread.visited_Atractions[0]){
            continue;
        }

        if(!pessoa_thread.visited_Atractions[1]){
            continue;
        }

        if(!pessoa_thread.visited_Atractions[2]){
            continue;
        }

        if(!pessoa_thread.visited_Atractions[3]){
            continue;
        }

        stay_on_park = false;

    }
}


void *barista_thread()
{
    int drink = 1;
    while (1)
    {
        sem_wait(&snack_empty);
        pthread_mutex_lock(&snack);
        buffer[in] = drink;
        printf("Barista made a drink.\n");
        in = (in + 1) % BUFFER_SIZE; // This makes it circular
        drink++;
        pthread_mutex_unlock(&snack);
        sem_post(&snack_full);
        usleep(200000);
    }
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

    // Initializes the array of visited atractions with all false
    for(int i = 0; i < 10; i++){
        pessoa_thread.visited_Atractions[i] = false;
    }

    /*
        This part of the code creates a unique seed for every thread so that every
        each time one of them calls rand it gives a different result. The seed is the thread_id
        so that it is different on every thread
    */
    unsigned int seed = (unsigned int)pthread_self();
    srand(seed);

    // lock for mutual exclusion when giving an id to the thread
    pthread_mutex_lock(&lock);
    char str[50];
    sprintf(str, "Pessoa %d criada com sucesso", nu);

    // Writing to the log file the person was created
    logMessage(args->logfile, ROUTINE, str);
    pessoa_thread.patience = rand() % 51 + 30;
    pessoa_thread.id = nu;
    nu++;
    pthread_mutex_unlock(&lock);

    determineAgeGroup(args, &pessoa_thread); // Call the function to assign the person an age group

    // Semaphore that handles how many people are allowed on the park
    gettimeofday(&pessoa_thread.time_on_line_park, NULL); // get time function got on line for the park
    send_message(2, 0, 0, pessoa_thread.id, args->client_socket);
    sem_wait(&entry_into_park);
    
    /*
        This will check if the time stipulated for simulation has elapsed or not, if yes the thread will not enter the park
    */
    if(args->start_time + args->config.simulation_duration > time(NULL)){ 
        gettimeofday(&pessoa_thread.time_entry_park, NULL); // get time function entered the park
        send_time_monitor(1, 0, pessoa_thread.time_on_line_park, pessoa_thread.time_entry_park, args->client_socket); // Send time on the line
        
        send_message(2, 0, 1, pessoa_thread.id, args->client_socket); // Message person entered the park
        manageAtractions(pessoa_thread, args->client_socket); //function that manages the entry into attractions
        sem_post(&entry_into_park); 
        send_message(2, 0, 2, pessoa_thread.id, args->client_socket); // Message the person exited the park
        
        gettimeofday(&pessoa_thread.time_exit_park, NULL); // get time function exited the park
        send_time_monitor(1, 1, pessoa_thread.time_entry_park, pessoa_thread.time_exit_park, args->client_socket); // Send time on the park
        send_message(0, 0, 0, pessoa_thread.id, args->client_socket); // Message the person used the park
    } else {
        printf("Park is closed \n");
        send_message(0, 0, 1, pessoa_thread.id, args->client_socket); // Message the person couldnt use the park
        send_message(2, 0, 3, pessoa_thread.id, args->client_socket);
        sem_post(&entry_into_park);
    }

    return NULL;
}
