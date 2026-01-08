#include "header.h"
// flaga globalna
volatile int wymuszony_odjazd = 0;

void handle_sig1(int sig) {
    wymuszony_odjazd = 1;
}


int main() {
    signal(SIGUSR1, handle_sig1); //sygnal 1
    
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0666);
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    double waga_ladunku = 0;
    int obj_ladunku = 0;
    pid_t moj_pid = getpid();
    printf("TRUCK %d: Dojechalem do firmy. Czekam na wjazd...\n", moj_pid);

    sem_p(semid, SEM_DOK);

    printf("TRUCK %d: Wjechalem pod rampe! Zaczynam zaladunek.\n", moj_pid);


    mag->pid_truck = moj_pid;
    mag->waga_ladunku_trucka = 0;
    while (1) {
        if (mag->koniec_pracy) break;

        //Czekamy na paczkę
        sem_p(semid, SEM_ZAJETE);
        //Blokujemy pamięć
        sem_p(semid, SEM_MUTEX);

        // Bierzemy paczkę
        Paczka p = mag->tasma[mag->head];
        mag->head = (mag->head + 1) % POJEMNOSC_TASMY;
        mag->ile_paczek--;
        mag->aktualna_waga_tasmy -= p.waga;

        printf("TRUCK %d: Biorę %c (%.1f kg). Stan: %.1f/%.0f\n", 
               moj_pid, p.typ, p.waga, waga_ladunku + p.waga, LADOWNOSC_CI);

        waga_ladunku += p.waga;
        obj_ladunku += p.objetosc;

        mag->waga_ladunku_trucka = waga_ladunku;// waga dla p4 w shared memory

        sem_v(semid, SEM_MUTEX);
        sem_v(semid, SEM_WOLNE); // Robimy miejsce na taśmie

        // Czy pełna?
        if (waga_ladunku >= LADOWNOSC_CI || obj_ladunku >= OBJETOSC_CI) {
            printf("--- PELNA (%.1f kg) --- ODJEZDZAM ---\n", waga_ladunku);
            sleep(2);
            //reset liczników
            waga_ladunku = 0;
            obj_ladunku = 0;
            printf("--- NOWA CIEZAROWKA ---\n");
        }
        
        sleep(1);
    }
    mag->pid_truck = 0;
    
    // zwolnienie miejsca dla kolejnej ciezarowki

    sem_v(semid, SEM_DOK);
    
    shmdt(mag);
    return 0;
}