#include "header.h"

volatile sig_atomic_t mam_robote = 0;
void handle_sig2(int sig) {
    mam_robote = 1;
}

int main() {
    signal(SIGUSR2, handle_sig2); //sygnal 2
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0600);
    int shmid = shmget(key, sizeof(Magazyn), 0600);
    if (shmid == -1 || semid == -1) {
        perror("P4 BLAD: Brak zasobow");
        exit(1);
    }
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    sem_p(semid, SEM_MUTEX);
    mag->pid_p4 = getpid(); 
    sem_v(semid, SEM_MUTEX);

    printf(RUMUNIA_RED"P4: Czekam na wezwanie (PID: %d)"OCZYSZCZANIE"\n", getpid());
    srand(time(NULL) ^ getpid()); // do losowania

while (1) {
        if (mag->koniec_pracy) break;

        // Czekanie na sygnał od dyspozytora
        while (!mam_robote && !mag->koniec_pracy) {
            usleep(1000); // Czekaj 
        }

        if (mam_robote) {

            int r = rand() % 3;
            int obj = 0;
            char typ = ' ';

            if (r == 0) {
                obj = 10;
                typ = 'A';
            } else if (r == 1) {
                obj = 20;
                typ = 'B';
            } else {
                obj = 40;
                typ = 'C';
            }
            
            double waga_ekspresu = (rand() % 230) / 10.0 + 1.0; 

            sem_p(semid, SEM_MUTEX); //deklaracja priorytetu fast workera
            mag->p4_priorytet = 1;   // blokowanie trucka
            sem_v(semid, SEM_MUTEX);

            printf("P4: Mam paczke ekspresowa %c (%.1f kg). Szukam ciezarowki\n", 
                   typ, waga_ekspresu);


            while (1) {
                if (mag->koniec_pracy) break;

                sem_p(semid, SEM_MUTEX); 

                if (mag->pid_truck > 0) {

                    if (mag->waga_ladunku_trucka + waga_ekspresu <= LADOWNOSC_CI &&
                        mag->objetosc_ladunku_trucka + obj <= OBJETOSC_CI) {

                      
                        mag->waga_ladunku_trucka += waga_ekspresu; // zapis do ladunku ciezarowki omija kolejke
                        mag->objetosc_ladunku_trucka += obj;
                        
                        printf("P4: WRZUCILEM PACZKE %.1f kg do Trucka %d. Stan: %.1f kg\n", 
                               waga_ekspresu, mag->pid_truck, mag->waga_ladunku_trucka);

                        mag->p4_priorytet = 0; //oddanie priorytetu klucza moze dalej truck pracowac
                        sem_v(semid, SEM_MUTEX); 

                        log_msg(semid, "P4 wrzucil ekspres %.1f kg do Trucka %d. Stan: %.1f kg", 
                                waga_ekspresu, mag->pid_truck, mag->waga_ladunku_trucka);
                        
                        
                        break; 
                    } else {

                        printf("P4: Truck %d pelny/brak miejsca. Czekam na nastepny\n", mag->pid_truck);
                        sem_v(semid, SEM_MUTEX); 
                        usleep(2000); // Czekamy 
                    }
                } else {
                    // Brak ciężarówki w doku
                    sem_v(semid, SEM_MUTEX); 
                    usleep(2000);// Czekamy 
                }
            }
            if (mag->koniec_pracy) {
                sem_p(semid, SEM_MUTEX);
                mag->p4_priorytet = 0;
                sem_v(semid, SEM_MUTEX);
            }  // gdyby symulacja se zakonczyla zabezpiecznie
        
            mam_robote = 0; 
         }
    }
    shmdt(mag);
    return 0;
}