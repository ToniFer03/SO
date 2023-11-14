#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"

#define MAX_LINE_LENGTH 100
#define MAX_KEY_LENGTH 50
#define MAX_VALUE_LENGTH 50

int readConfigFile(const char *filename, struct Simualador_config *Config){
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
        return 1; //returns 1 if a mistake appears
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline character
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        char key[MAX_KEY_LENGTH];
        char value[MAX_VALUE_LENGTH];

        // Parse key and value, ignoring leading and trailing spaces
        if (sscanf(line, " %49[^= ] = %49s ", key, value) == 2) {
            // Check key and assign value
            if (strcmp(key, "Probability_Being_Elder") == 0) {
                Config->probability_being_elder = atof(value);
            } else if (strcmp(key, "Probability_Being_Child") == 0) {
                Config->probability_being_child = atof(value);
            } else {
                // Debugging: Print if key is not recognized
                printf("Unrecognized key: %s\n", key);
            }
        }
    }

    fclose(file);
    return 0; //returns 0 if sucessfull
}
