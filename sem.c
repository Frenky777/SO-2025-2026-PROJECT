#include "header.h"

// Funkcja P (Czekaj)
void sem_p(int semid, int sem_num) {
    struct sembuf bufor;
    bufor.sem_num = sem_num;
    bufor.sem_op = -1; //-1
    bufor.sem_flg = 0;
    if (semop(semid, &bufor, 1) == -1) {
        if(errno != EINTR) perror("Blad sem_p");
    }
    
}

// Funkcja V (Zwolnij)
void sem_v(int semid, int sem_num) {
    struct sembuf bufor;
    bufor.sem_num = sem_num;
    bufor.sem_op = 1; //+1
    bufor.sem_flg = 0;
    
    if (semop(semid, &bufor, 1) == -1) {
        perror("Blad sem_v");
    }
}