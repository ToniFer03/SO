/* leitor_ficheiros.h */
#ifndef LEITOR_FICHEIROS_H
#define LEITOR_FICHEIROS_H

#define MAX_LINE_LENGTH 100 // Max number of characters in a line to be read
#define MAX_KEY_LENGTH 50   // Max number of characters a key can have
#define MAX_VALUE_LENGTH 50 // Max number of characters a value can have

// definition of functions
int readConfigFile(const char *filename, struct Simulador_config *Config);

#endif
