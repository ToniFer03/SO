/* escrita_ficheiros.h */
#ifndef ESCRITA_FICHEIROS_H
#define ESCRITA_FICHEIROS_H

#define ERROR "Error" //Define the error string
#define ROUTINE "Routine" //Define the routine string


//Definition of functions
void logMessage(const char *filename, const char *level, const char *message);
void transformDateString(char *input, char *output);

#endif
