#include "header.h"
// flaga globalna
volatile sig_atomic_t wymuszony_odjazd = 0;

void handle_sig1(int sig) {
    wymuszony_odjazd = 1;
}


int main() {
    signal(SIGUSR1, handle_sig1); //sygnal 1 wymuszony odjazd
    
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0600);
    int shmid = shmget(key, sizeof(Magazyn), 0600);
    if (shmid == -1 || semid == -1) {
        perror("TRUCK BLAD: Brak zasobow");
        exit(1);
    }
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);

    pid_t moj_pid = getpid();
    printf(RUMUNIA_BLUE"TRUCK %d: Dojechalem do firmy. Czekam na wjazd"OCZYSZCZANIE"\n", moj_pid);
    
    // PĘTLA GŁÓWNA 
    while (1) {

        sem_p(semid, SEM_DOK);


        sem_p(semid, SEM_MUTEX);
        mag->pid_truck = moj_pid;
        mag->waga_ladunku_trucka = 0.0;
        mag->objetosc_ladunku_trucka = 0;
        wymuszony_odjazd = 0;
        
        // jesli koniec pracy sprawdza czy na tasmie jest 0 paczke jesli tak to koniec
        if (mag->koniec_pracy && mag->ile_paczek == 0) {
            sem_v(semid, SEM_MUTEX);
            sem_v(semid, SEM_DOK);
            printf("Truck %d: Koniec pracy i brak paczek\n", moj_pid);
            break;
        }
        sem_v(semid, SEM_MUTEX);

        printf(RUMUNIA_BLUE"Truck %d: Podjechalem pod dok."OCZYSZCZANIE"\n", moj_pid);


        while (1) {
            if (wymuszony_odjazd) {
                printf("Truck %d: Nakaz odjazdu!\n", moj_pid);
                log_msg(semid, "Truck %d wymuszony odjazd  ", // dodanie wymuszonego odjazdu logu
                moj_pid);
                break;
            }


            sem_p(semid, SEM_ZAJETE);


            sem_p(semid, SEM_MUTEX);

            while (mag->p4_priorytet == 1) { // jesli p4 zglosci priorytet oddaje mutex i czeka
            sem_v(semid, SEM_MUTEX);  // oddanie klucza
            //usleep(1000);        
            sem_p(semid, SEM_MUTEX);
    

            }
         
             // naprawa problemu z pozostaniem paczek na tasmie
            if (mag->ile_paczek <= 0) {
               
                if (mag->koniec_pracy) {
                    sem_v(semid, SEM_MUTEX);
                    printf("Truck %d: Koniec pracy (tasma pusta). Wyjazd z doku.\n", moj_pid);
                    break; 
                }
                
                sem_v(semid, SEM_MUTEX);
                continue;
            }

            // odczyt danych fifo
            Paczka paczka = mag->tasma[mag->head];


            if (mag->waga_ladunku_trucka + paczka.waga > LADOWNOSC_CI ||
                mag->objetosc_ladunku_trucka + paczka.objetosc > OBJETOSC_CI) {
                

                printf(RUMUNIA_BLUE"Truck %d: Pelny (%.1f kg). Odjezdzam."OCZYSZCZANIE"\n", moj_pid, mag->waga_ladunku_trucka);
                
                sem_v(semid, SEM_MUTEX);
                

                sem_v(semid, SEM_ZAJETE); 
                
                break; 
            }


            mag->head = (mag->head + 1) % POJEMNOSC_TASMY;
            mag->ile_paczek--;
            mag->aktualna_waga_tasmy -= paczka.waga;
            
            mag->waga_ladunku_trucka += paczka.waga;
            mag->objetosc_ladunku_trucka += paczka.objetosc;


            double log_waga_ladunku = mag->waga_ladunku_trucka;

            sem_v(semid, SEM_MUTEX);

            // zwolnienie miejsca na tasmie
            sem_v(semid, SEM_WOLNE);

            printf(RUMUNIA_BLUE"Truck %d zabral paczke %c (%.1f kg). Stan ladunku: %.1f kg"OCZYSZCZANIE"\n", 
                    moj_pid, paczka.typ, paczka.waga, log_waga_ladunku);
            log_msg(semid, "Truck %d zabral paczke %c (%.1f kg). Stan ladunku: %.1f kg", 
                    moj_pid, paczka.typ, paczka.waga, log_waga_ladunku);
            

        }


        sem_p(semid, SEM_MUTEX);
        mag->pid_truck = 0;
        mag->waga_ladunku_trucka = 0.0; 
        sem_v(semid, SEM_MUTEX);

        sem_v(semid, SEM_DOK); 

        printf(RUMUNIA_DARK_BLUE"Truck %d: Wyjazd w trase"OCZYSZCZANIE"\n", moj_pid);
        sleep(TI); 
    }

    shmdt(mag); // odlodzenie od pamieci
    return 0;
}