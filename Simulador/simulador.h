/* simulador.h */
#ifndef SIMULADOR_H
#define SIMULADOR_H

#define MAX_NUM_THREADS 50 // The number of threads that are going to be created at the beggining

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
    int atraction_number;
    int max_people_park;
    int max_people_toboggan;
    int max_people_snack_bar;
};

// Define a struct to hold person information for each thread
struct Person_info
{
    enum Faixa_etaria faixa_etaria; // Include the enum member
    int *visited_Atractions;        // creates an array of all the visited atractions
    int id;
    int patience;
};

// Define a struct to hold the client socket and person information
struct ThreadArgs
{
    int client_socket;
    FILE *logfile;
    struct Simulador_config config;
};

// Definition of functions
void *person_thread(void *arg);    // Function executed by the thread
void *barista_thread();
const char *getCurrentTimestamp(); // Function to get the timestamp from now in char

#endif
