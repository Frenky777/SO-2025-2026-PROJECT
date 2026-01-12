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
//logger
void log_msg(int semid, const char *format, ...){ 
    sem_p(semid, SEM_MUTEX);

    FILE *f = fopen("symulacja.log", "a");
    if (f == NULL) {
        perror("Blad pliku log");
        sem_v(semid, SEM_MUTEX);
        return;
    }


    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(f, "[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);


    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);

    fprintf(f, "\n");
    fclose(f);

    sem_v(semid, SEM_MUTEX);
}

void clear_log(int semid) {

    sem_p(semid, SEM_MUTEX);


    FILE *f = fopen("symulacja.log", "w"); 
    if (f == NULL) {
        perror("Nie udalo sie wyczyscic loga");
        sem_v(semid, SEM_MUTEX);
        return;
    }



    fclose(f);

    sem_v(semid, SEM_MUTEX);
}