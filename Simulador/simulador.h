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
struct Simualador_config{
    double probability_being_elder;
    double probability_being_child;
};

// Define a struct to hold person information
struct Person_info {
    enum Faixa_etaria faixa_etaria; // Include the enum member
};

// Define a struct to hold the client socket and person information
struct ThreadArgs {
    int client_socket;
    char *log_filename;
    struct Person_info Pessoa;
};

// Function executed by the thread
void* person_thread(void* arg);

//Function to get the timestamp from now in char
const char *getCurrentTimestamp();

#endif
