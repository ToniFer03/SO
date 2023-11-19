#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"
#include "leitor_ficheiros.h"

/*
    This function is responsible for reading the configuration file that contains the essential
    information when running the program. Its use, and how can be easily changed can be found on the
    read me file.
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
