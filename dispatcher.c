#include "header.h"

int main() {
    // 1. Podłączamy się do istniejącej pamięci (bez tworzenia nowej!)
    key_t key = ftok(".", ID_PROJEKTU);
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    if (shmid == -1) {
        printf("BŁĄD: Nie mogę połączyć się z Magazynem.\n");
        exit(1);
    }
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);


    printf("\nPANEL DYSPOZYTORA \n");
    printf("PID Trucka w doku: %d\n", mag->pid_truck);
    printf("PID Fast Workera: %d\n", mag->pid_p4);
    printf("----------------------------------------------------------\n");
    printf("1. Nakaz natychmiastowego odjazdu\n");
    printf("2. Zaladunek ekspresowy\n");
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
        } else {
            printf("Blad Fast Workera\n");
        }
    }

    shmdt(mag);
    return 0;
}