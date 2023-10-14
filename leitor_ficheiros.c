#include <stdio.h>

int initial_config_simulador(char *argv){
    FILE *file;

    file = fopen(argv, "r");

    if(file == NULL){
        printf("Não foi possivel abrir o ficheiro %s!\n", argv);
        return 1;
    }

    printf("sucesso!");
    
    fclose(file);
    return 0;
}
