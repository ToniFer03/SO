/* simulador.h */
#include <stdbool.h>
#include <sys/time.h>
#ifndef SIMULADOR_H
#define SIMULADOR_H

//#define MAX_NUM_THREADS 500  The number of threads that are going to be created at the beggining

// Define enum values
// This enum value represents the age groups
enum Faixa_etaria
{
    CRIANCA,
    ADULTO,
    IDOSO
};

// Defininition of Structures
// define struct that contains the config information read from the file
struct Simulador_config
{
    double probability_being_elder;
    double probability_being_child;
    int simulation_duration;
    int time_being_simulated;
    int max_people_park;
    int max_people_toboggan;
    int max_people_snack_bar;
    int threads_to_create;
};

// Define a struct to hold person information for each thread
struct Person_info
{
    enum Faixa_etaria faixa_etaria; // Include the enum member
    bool visited_Atractions[10];        // creates an array of all the visited atractions
    int id;
    int patience;
    struct timeval time_on_line_park; // Time the person arrived on line at the park
    struct timeval time_entry_park; // will hold the time the person entered the park
    struct timeval time_exit_park; // will hold the time the person entered the park
    struct timeval time_start_waiting_toboggan; // Will hold time person entered the line of the toboggan
    struct timeval time_exit_waiting_toboggan; // Will hold time person exited the line of the toboggan
    struct timeval time_start_waiting_snack;  // Will hold the time person started waiting for a snack
    struct timeval time_exit_waiting_snack;  // Will hold the time the person got served the snack
    struct timeval time_start_waiting_famwaterslide; // Will hold the time the person started waiting for the waterslide
    struct timeval time_exit_waiting_famWaterslide; // Will hold the time the person used used the waterslide
    struct timeval time_start_waiting_waterpolo; // Will hold the time for the person to start waiting for waterpolo
    struct timeval time_exit_waiting_waterpolo; // Will hold the time for the persos to start playing waterpolo
};

// Define a struct to hold the client socket and person information
struct ThreadArgs
{
    int client_socket;
    FILE *logfile;
    struct Simulador_config config;
    time_t start_time;
};

// Definition of functions
void *person_thread(void *arg);    // Function executed by the thread
void *barista_thread();
const char *getCurrentTimestamp(); // Function to get the timestamp from now in char
void calculateTimeScale(struct Simulador_config *config, int client_socket);

#endif
