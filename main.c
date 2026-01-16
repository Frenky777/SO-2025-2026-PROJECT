#include "header.h"
#include <sys/wait.h>


// zmienne globalne zeby handler je widzial
int globalny_shmid = -1;
int globalny_semid = -1;

// Handler dla Ctrl+C 
void handle_sigint(int sig) {
    printf(RUMUNIA_GREEN"\nMAIN: Otrzymano Ctrl+C. Sprzątanie"OCZYSZCZANIE"\n");

    

    // usun semafory
    if (globalny_semid != -1) {
        if (semctl(globalny_semid, 0, IPC_RMID) == -1) {
            perror("MAIN: Błąd usuwania semaforów w handlerze");
        } else {
            printf(RUMUNIA_GREEN"MAIN: Semafory usunięte."OCZYSZCZANIE"\n");
        }
    }

    // usun pamiec wspoldzielona
    if (globalny_shmid != -1) {
        if (shmctl(globalny_shmid, IPC_RMID, NULL) == -1) {
            perror("MAIN: Błąd usuwania pamięci dzielonej w handlerze");
        } else {
            printf(RUMUNIA_GREEN"MAIN: Pamięć dzielona usunięta."OCZYSZCZANIE"\n");
        }
    }

    kill(0, SIGKILL);
    exit(0);
}


void handle_sig1(int sig) {
}
void handle_sig2(int sig) {   
}

int main() {
    //sygnal do ctrl c
    signal(SIGINT, handle_sigint);

    signal(SIGUSR1, handle_sig1); 
    signal(SIGUSR2, handle_sig2);
    // Tworzenie pliku klucza do ftok
    system("touch ."); 

    if (system("touch .") == -1) {
        perror("MAIN BLAD: Nie udalo sie utworzyc pliku klucza");
        exit(1);
    }

    key_t key = ftok(".", ID_PROJEKTU);

    if (key == -1) {
        perror("MAIN BLAD: ftok");
        exit(1);
    }
    // Alokacja
    int shmid = shmget(key, sizeof(Magazyn), IPC_CREAT | 0600);
    if (shmid == -1) {
        perror("MAIN BLAD: shmget pamiec");
        exit(1);
    }
    globalny_shmid = shmid; // przypisanie do zmiennnej globalnej

    int semid = semget(key, LICZBA_SEM, IPC_CREAT | 0600);

    if (semid == -1) {
        perror("MAIN BLAD: semget semafor");
        shmctl(shmid, IPC_RMID, NULL);
        exit(1);
    }
    globalny_semid = semid; // przypisanie do zmiennnej globalnej

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
    semctl(semid, SEM_DOK, SETVAL, 1);

    printf(RUMUNIA_GREEN"START SYSTEMU"OCZYSZCZANIE" \n");

    clear_log(semid);



    if (fork() == 0) { 
        execl("./fast_worker", "fast_worker", NULL); 
        exit(0);  //dla p4
    }
    // uruchamiamy 3 ciezarowki
    for(int i=0; i<LIMIT_CIEZAROWEK; i++) {
        if (fork() == 0) { 
            char buf[15];
            execl("./truck", "truck", buf, NULL); 
            exit(0); 
        }
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



    while (wait(NULL) > 0); 
    
    printf(RUMUNIA_GREEN"\nMAIN: Wszystkie procesy zakończyły pracę. Sprzątam system"OCZYSZCZANIE"\n");


    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Błąd usuwania semaforów");
    } else {
        printf(RUMUNIA_GREEN"MAIN: Semafory usunięte."OCZYSZCZANIE"\n");
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Błąd usuwania pamięci dzielonej");
    } else {
        printf(RUMUNIA_GREEN"MAIN: Pamięć dzielona usunięta."OCZYSZCZANIE"\n");
    }
    
    printf(RUMUNIA_GREEN"MAIN: Koniec programu."OCZYSZCZANIE"\n");
    return 0;
}