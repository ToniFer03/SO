/* simulador.h */
#ifndef SIMULADOR_H
#define SIMULADOR_H

#define MAX_NUM_THREADS 1000

// Define enum values
enum Faixa_etaria {
    CRIANCA,
    ADULTO,
    IDOSO
};

//define struct that contains the config information
struct Simulador_config{
    double probability_being_elder;
    double probability_being_child;
    int simulation_duration;
    int time_being_simulated;
    int atraction_number;
    int max_people_park;
};

// Define a struct to hold person information
struct Person_info {
    enum Faixa_etaria faixa_etaria; // Include the enum member
    int * visited_Atractions; //creates an array of all the visited atractions
};

// Define a struct to hold the client socket and person information
struct ThreadArgs {
    int client_socket;
    char *log_filename;
    struct Simulador_config config;
    struct Person_info pessoa;
};

void* person_thread(void* arg); // Function executed by the thread
const char *getCurrentTimestamp(); //Function to get the timestamp from now in char

#endif
