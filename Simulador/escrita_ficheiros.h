/* escrita_ficheiros.h */
#ifndef ESCRITA_FICHEIROS_H
#define ESCRITA_FICHEIROS_H

#define ERROR "Error" //Define the error string
#define ROUTINE "Routine" //Define the routine string


//Definition of functions
FILE* openFile(const char *filename);
void closeFile(FILE* file);
void logMessage(FILE* file, const char *level, const char *message);
void transformDateString(char *input, char *output);

#endif
