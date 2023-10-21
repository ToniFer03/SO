/* simulador.c */
#include <stdio.h>
#include "simulador.h"
#include "leitor_ficheiros.h"

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Necessário passar como argumento um ficheiro de configuração!\n");
        return 1;
    }

   if(load_initial_config_simulador(argv[1]) == 1){
        printf("Programa teve de encerrar devido a um erro ao carregar o ficheiro de configuração ");
        printf("do simulador.\n");
        return 0;
   }

    return 0;
}