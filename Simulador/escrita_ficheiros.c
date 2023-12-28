#include <stdio.h>
#include <time.h>
#include <string.h>
#include "simulador.h"
#include "escrita_ficheiros.h"

/*
   Function: openFile
   Purpose:  Opens a file in append mode and returns the file pointer.
   Parameters:
      - filename: The name of the file to be opened.
   Returns:
      - A pointer to the opened file.
*/
FILE *openFile(const char *filename)
{
    // Open the log file
    FILE *file = fopen(filename, "a");
    return file;
}



/*
   Function: closeFile
   Purpose:  Closes the specified file.
   Parameters:
      - file: A pointer to the file to be closed.
*/
void closeFile(FILE *file)
{
    fclose(file);
}



/*
   Function: logMessage
   Purpose:  Writes a log entry to the specified file.
   Parameters:
      - file: A pointer to the log file.
      - level: The log level, indicating the severity of the message (e.g., INFO, ERROR).
      - message: The log message to be written.
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
   Function: transformDateString
   Purpose:  Transforms a date string from the format "AAAAMMDDHHMMSS" to a more readable format.
   Parameters:
      - input: The input date string in the format "AAAAMMDDHHMMSS".
      - output: The output string with the transformed date in a readable format.
*/
void transformDateString(char *input, char *output)
{
    int year, month, day, hour, minute, second;

    // Parse the input string
    sscanf(input, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);

    // Format the output string
    sprintf(output, "%04d-%02d-%02d at %02d:%02d:%02d", year, month, day, hour, minute, second);
}