#include <stdio.h>
#include <time.h>
#include <string.h>
#include "simulador.h"
#include "escrita_ficheiros.h"

/*
    Function to open a file
*/
FILE *openFile(const char *filename)
{
    // Open the log file
    FILE *file = fopen(filename, "a");
    return file;
}

/*
    Funtion that receives an argument of the type FILE and closes it
*/
void closeFile(FILE *file)
{
    fclose(file);
}

/*
    This function is responsible for writing to the log files. Receives the name
    of the file that it needs to write, the level of the message (if it is an ERROR,
    ROUTINE, ETC) and the message to be written to the file
*/
void logMessage(FILE *file, const char *level, const char *message)
{
    // Get the current time, for the message
    char timestamp[20];
    char formated_timestamp[20];
    strcpy(timestamp, getCurrentTimestamp());

    transformDateString(timestamp, formated_timestamp);

    // Write the log entry to the file
    fprintf(file, "%s | %s | %s\n", formated_timestamp, level, message);
}

/*
    This function is responsible for getting the string of the current time on the format
    20231105150523 (AAAAMMDDHHMMSS) and transforming in a string that is easier to read
    on the log file
*/
void transformDateString(char *input, char *output)
{
    int year, month, day, hour, minute, second;

    // Parse the input string
    sscanf(input, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);

    // Format the output string
    sprintf(output, "%04d-%02d-%02d at %02d:%02d:%02d", year, month, day, hour, minute, second);
}