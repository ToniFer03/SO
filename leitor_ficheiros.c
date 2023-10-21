#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXBUF 1024 //defines the max size for the buffer as 1024
#define DELIM " = " //the delimitation that separetes the value and the key


//defines the date structure of config file being on the the type name = value
struct config_file_format {
   char key[MAXBUF]; //name given to that configuration
   char value[MAXBUF]; //value given to that name
};


//receives the current line of the file as well as the cfg
void parse_config_line(char *line, struct config_file_format *cfg) {
    char *temp = strdup(line); //creates a duplicate of the string and returns a poiter to ir
    char *token;

    //receives a string and separates into tokens using the DELIM defined above
    token = strtok(temp, DELIM); //gets the first part of the token
    
    //if the token is not null passes that value to the key
    if(token != NULL) {
        strncpy(cfg->key, token, MAXBUF);
    }

    //NULL tells it to start from where it stopped on the last call
    token = strtok(NULL, DELIM); //gets the second part
    
    //if it is not NULL passes the value to the value
    if(token != NULL) {
        strncpy(cfg->value, token, MAXBUF);
    }

    free(temp); //clears the memory that it was pointing
};


int load_initial_config_simulador(char *argv){
    FILE *file;

    file = fopen(argv, "r"); //opens file on read mode

    if(file == NULL){ //if it is null, returns 1 as error code
        printf("Não foi possivel abrir o ficheiro %s!\n", argv);
        return 1; 
    }

    char line[MAXBUF]; //give line a max size of MAXBUF
    struct config_file_format cfg; //creates a cfg variable based on the struct defined

    //while the line on the file is not empty keep reading
    while(fgets(line, sizeof(line), file) != NULL) {
        // If the line is a newline character, break out of the loop
        if(strcmp(line, "\n") == 0) {
            break;
        }

        // Clear the key and value fields
        memset(cfg.key, 0, MAXBUF);
        memset(cfg.value, 0, MAXBUF);

        parse_config_line(line, &cfg);
        if(strlen(cfg.key) > 0) {
            printf("%s = %s", cfg.key, cfg.value); // Removed '\n' from printf
        }
    }

    if(feof(file)) { // Check if end of file was reached
        printf("sucesso! \n"); //debugging line
    }
    
    fclose(file);
    return 0; //returns if a success
}

