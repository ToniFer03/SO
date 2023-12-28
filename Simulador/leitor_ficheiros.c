#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"
#include "leitor_ficheiros.h"

/*
    Function: readConfigFile
    Purpose:  Reads the configuration file and populates the Simulador_config structure with the specified parameters.
    Parameters:
      - filename: The name of the configuration file to read.
      - Config: Pointer to the Simulador_config structure where the configuration parameters will be stored.
    Returns:
      - 0 on success.
      - 1 if there is an error opening the file.

    How it works: Opens the file, and checks for every line if it matches one of the keys that is writen on
    the if block, it so passes the value in front of the key to the desired variable in the config structure    
*/
int readConfigFile(const char *filename, struct Simulador_config *Config)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
        return 1; // returns 1 if a mistake appears
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Remove newline character
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        char key[MAX_KEY_LENGTH];
        char value[MAX_VALUE_LENGTH];

        // Parse key and value, ignoring leading and trailing spaces
        if (sscanf(line, " %49[^= ] = %49s ", key, value) == 2)
        {
            // Check key and assign value
            if (strcmp(key, "Probability_Being_Elder") == 0)
            {
                Config->probability_being_elder = atof(value);
            }
            else if (strcmp(key, "Probability_Being_Child") == 0)
            {
                Config->probability_being_child = atof(value);
            }
            else if (strcmp(key, "Probability_Being_Writer") == 0)
            {
                Config->probability_being_writer = atof(value);
            }
            else if (strcmp(key, "Simulation_duration_(seconds)") == 0)
            {
                Config->simulation_duration = atoi(value);
            }
            else if (strcmp(key, "Time_being_simulated_(Hours)") == 0)
            {
                Config->time_being_simulated = atoi(value);
            }
            else if (strcmp(key, "Max_People_Park") == 0)
            {
                Config->max_people_park = atoi(value);
            }
            else if (strcmp(key, "Max_People_Toboggan") == 0)
            {
            	Config->max_people_toboggan = atoi(value);
            }
            else if (strcmp(key, "Max_People_Snack_Bar") == 0)
            {
            	Config->max_people_snack_bar = atoi(value);
            }
            else if (strcmp(key, "Threads_to_create") == 0)
            {
            	Config->threads_to_create = atoi(value);
            }
            else if (strcmp(key, "Timeout_waterpolo_(Seconds)") == 0)
            {
            	Config->timeout_waterpolo = atoi(value);
            }
            else if (strcmp(key, "Wait_time_FamilyWaterslide_(Microseconds)") == 0)
            {
            	Config->wait_time_familyWaterslide = atoi(value);
            }
            else if (strcmp(key, "Wait_time_Toboggan_(Microseconds)") == 0)
            {
            	Config->wait_time_toboggan = atoi(value);
            }
            else if (strcmp(key, "Wait_time_snack_bar_(Microseconds)") == 0)
            {
            	Config->wait_time_snackbar = atoi(value);
            }
            else if (strcmp(key, "Wait_time_waterpolo_(Microseconds)") == 0)
            {
            	Config->Wait_time_waterpolo = atoi(value);
            }
            else if (strcmp(key, "Wait_time_readsignature_(Microseconds)") == 0)
            {
            	Config->wait_time_read_signature = atoi(value);
            }else if (strcmp(key, "Wait_time_writesignature_(Microseconds)") == 0)
            {
            	Config->wait_time_sign_signature = atoi(value);
            }
            else
            {
                // Debugging: Print if key is not recognized
                printf("Unrecognized key: %s\n", key);
            }
        }
    }

    fclose(file);
    return 0; // returns 0 if sucessfull
}
