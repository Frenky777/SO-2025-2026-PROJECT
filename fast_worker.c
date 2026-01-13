#include "header.h"

volatile int mam_robote = 0;
void handle_sig2(int sig) {
    mam_robote = 1;
}

int main() {
    signal(SIGUSR2, handle_sig2); //sygnal 2
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0666);
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    if (shmid == -1 || semid == -1) {
        perror("P4 BLAD: Brak zasobow");
        exit(1);
    }
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    sem_p(semid, SEM_MUTEX);
    mag->pid_p4 = getpid(); 
    sem_v(semid, SEM_MUTEX);

    printf("P4: Czekam na wezwanie (PID: %d)\n", getpid());
    srand(time(NULL) ^ getpid()); // do losowania

    while (1) {
        if (mag->koniec_pracy) break;

        if (mam_robote) {
            sem_p(semid, SEM_MUTEX);
             if (mag->pid_truck > 0) {

                double waga_ekspresu = (rand() % 230) / 10.0 + 1.0; 
                
                //sprawdzanie czy paczka wejdzie
                if (mag->waga_ladunku_trucka + waga_ekspresu <= LADOWNOSC_CI) {
                    mag->waga_ladunku_trucka += waga_ekspresu;
                    printf("P4: WRZUCILEM PACZKE %.1f kg Stan trucka: %.1f kg\n", 
                           waga_ekspresu, mag->waga_ladunku_trucka);
                    log_msg(semid, "P4 wrzucil ekspres %.1f kg. Stan trucka: %.1f kg", 
                            waga_ekspresu, mag->waga_ladunku_trucka);
                } else {
                    printf("P4: Ciężarówka pełna, nie zmieściłem ekspresu\n");
                }
            } else {
                printf("P4: Brak ciężarówki w doku Czekam na następną\n");
            }
            sem_v(semid, SEM_MUTEX);
            
            mam_robote = 0;
        }
        sleep(1);
    }
    return 0;
}