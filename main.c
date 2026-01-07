#include "header.h"
#include <sys/wait.h>

int main() {
    // Tworzenie pliku klucza do ftok
    system("touch ."); 
    key_t key = ftok(".", ID_PROJEKTU);

    // Alokacja
    int shmid = shmget(key, sizeof(Magazyn), IPC_CREAT | 0666);
    int semid = semget(key, LICZBA_SEM, IPC_CREAT | 0666);
    
    // Inicjalizacja pamięci
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);
    mag->head = 0;
    mag->tail = 0;
    mag->ile_paczek = 0;
    mag->aktualna_waga_tasmy = 0.0;
    mag->koniec_pracy = 0;
    mag->waga_ladunku_trucka = 0.0; //dla p4

    // Inicjalizacja semaforów
    semctl(semid, SEM_MUTEX, SETVAL, 1);       // Dostęp otwarty
    semctl(semid, SEM_WOLNE, SETVAL, POJEMNOSC_TASMY); // Cała taśma wolna
    semctl(semid, SEM_ZAJETE, SETVAL, 0);      // Brak paczek

    printf("START SYSTEMU (Wersja Basic)\n");

    // Uruchamiamy ciężarówkę
    if (fork() == 0) { 
        execl("./truck", "truck", NULL); 
        exit(0); 
    }
    if (fork() == 0) { 
        execl("./truck", "truck", NULL); exit(0); 
    }
    if (fork() == 0) { 
        execl("./fast_worker", "fast_worker", NULL); exit(0);  //dla p4
    }
    
    // Uruchamiamy 3 pracowników
    for(int i=1; i<=3; i++) {
        if (fork() == 0) {
            char buf[10];
            sprintf(buf, "%d", i);
            execl("./worker", "worker", buf, NULL);
            exit(0);
        }
    }

    while(1) {
        sleep(1);
    }
    
    return 0;
}