#include "header.h"

int main() {

    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0666); // pobranie semaforow
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    if (shmid == -1 || semid == -1) {
        printf("BŁĄD: Nie mogę połączyć się z Magazynem.\n");
        exit(1);
    }
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    while(1) {
    printf("\nPANEL DYSPOZYTORA \n");
    printf("PID Trucka w doku: %d\n", mag->pid_truck);
    printf("PID Fast Workera: %d\n", mag->pid_p4);
    printf("----------------------------------------------------------\n");
    printf("1. Nakaz natychmiastowego odjazdu\n");
    printf("2. Zaladunek ekspresowy\n");
    printf("3. KONIEC PRACY Sygnał 3\n");
    printf("0. Wyjscie\n");
    printf("----------------------------------------------------------\n");

    int wybor;
    printf("Twoj wybor: ");
    scanf("%d", &wybor);

    if (wybor == 1) {
        if (mag->pid_truck > 0) {
            kill(mag->pid_truck, SIGUSR1);
            printf("Wyslano rozkaz odjazdu do PID %d.\n", mag->pid_truck);
        } else {
            printf("Blad Brak ciezarowki\n");
        }
    } 
    else if (wybor == 2) {
        if (mag->pid_p4 > 0) {
            kill(mag->pid_p4, SIGUSR2);
            printf("Wyslano rozkaz ekspresu do PID %d.\n", mag->pid_p4);
        } 
    }
    else if (wybor == 3) {
            printf("ZAMYKANIE SYSTEMU\n");
            
            sem_p(semid, SEM_MUTEX);
            mag->koniec_pracy = 1;
            sem_v(semid, SEM_MUTEX);
            printf("Budzenie procesów\n");
            for(int i=0; i < POJEMNOSC_TASMY ; i++) {
                sem_v(semid, SEM_WOLNE); 
                sem_v(semid, SEM_ZAJETE); 
            } // troche prymitywne ale jak na ten moment dziala 
            sem_v(semid, SEM_DOK); // na wszelki wypadek do trucka
            break;
    } 
    else if (wybor == 0) {
            break;
        }
    else {
        printf("zly wybor");
    }
    }

    shmdt(mag);
    return 0;
    }

    
