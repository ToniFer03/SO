#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int write_to_file() {
    FILE *file;

    file = fopen("test_log.txt", "w");
    srand(time(NULL)); //uses time as a seed to create a random number

    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d\n", rand()); //writes a random number on every line
    }

    fclose(file);

    return 0;
}
