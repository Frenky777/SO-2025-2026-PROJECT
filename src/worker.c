#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uzycie: %s <identyfikator_pracownika>\n", argv[0]);
        return 1;
    }

    char *id = argv[1];
    printf("Pracownik %s: Rozpoczynam prace w magazynie...\n", id);


    while(1) {

        sleep(2);
    }

    return 0;
}