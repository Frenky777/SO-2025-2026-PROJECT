#include "header.h"

volatile int mam_robote = 0;
void handle_sig2(int sig) {
    mam_robote = 1;
}

int main() {

    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0666);
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    mag->pid_p4 = getpid(); 
    printf("P4 (Ekspres): Czekam na wezwanie (PID: %d)\n", getpid());

    while (1) {
        if (mag->koniec_pracy) break;

        if (mam_robote) {
            sem_p(semid, SEM_MUTEX);
            // P4 ładuje bezpośrednio na trucka 
            double waga_ekspresu = 10.0;
            mag->waga_ladunku_trucka += waga_ekspresu;
            printf("P4: ZALADOWALEM! Stan trucka: %.1f kg\n", mag->waga_ladunku_trucka);
            sem_v(semid, SEM_MUTEX);
            
            mam_robote = 0;
        }
    }
    return 0;
}