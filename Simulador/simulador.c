/* simulador.c */
#include <stdio.h>
#include "simulador.h"
#include "leitor_ficheiros.h"
#include "escrita_ficheiros.h"
#include "cliente.h"

int main(int argc, char *argv[]){
    /*
    if(argc < 2){
        printf("Necessário passar como argumento um ficheiro de configuração!\n");
        return 1;
    }

    if(load_initial_config_simulador(argv[1]) == 1){
        printf("Programa teve de encerrar devido a um erro ao carregar o ficheiro de configuração ");
        printf("do simulador.\n");
        return 0;
    }
    write_to_file();
    */

    connect_server();

    while (1){
    }
    

    return 0;
}