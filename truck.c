#include "header.h"

int main() {
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0666);
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    double waga_ladunku = 0;
    int obj_ladunku = 0;

    printf("CIEZAROWKA: Podstawiona.\n");

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

        printf("CIEZAROWKA: Biorę %c od P%d. Waga tasmy: %.1f Stan: %d/%d\n", 
               p.typ, p.id_pracownika, mag->aktualna_waga_tasmy, mag->ile_paczek, POJEMNOSC_TASMY);

        waga_ladunku += p.waga;
        obj_ladunku += p.objetosc;

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

    shmdt(mag);
    return 0;
}