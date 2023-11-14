#include <stdio.h>
#include <time.h>
#include "escrita_ficheiros.h"
#include "simulador.h"

void logMessage(const char *filename, const char *level, const char *message) {
    // Open the log file in append mode
    FILE *file = fopen(filename, "a");

    if (file != NULL) {
        // Get the current time
        char timestamp[20];
        getCurrentTimestamp(timestamp);

        // Write the log entry to the file
        fprintf(file, "%s | %s | %s\n", timestamp, level, message);

        // Close the file
        fclose(file);
    } else {
        perror("Error opening log file");
    }
}