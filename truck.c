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
    
    // PĘTLA GŁÓWNA 
    while (1) {
        if (mag->koniec_pracy) break;

       
        printf("TRUCK %d: Dojechalem do firmy. Czekam na wjazd...\n", moj_pid);
        sem_p(semid, SEM_DOK);

      
        sem_p(semid, SEM_MUTEX);
        mag->pid_truck = moj_pid;      
        mag->waga_ladunku_trucka = 0;  
        wymuszony_odjazd = 0;          
        sem_v(semid, SEM_MUTEX);

        printf("TRUCK %d: Podstawiony pod rampe! Zaczynam zaladunek.\n", moj_pid);

        
        waga_ladunku = 0;
        obj_ladunku = 0;

        // PĘTLA ZAŁADUNKU PACZEK
        while (1) {
            if (mag->koniec_pracy) break;
            
           
            if (wymuszony_odjazd) {
                printf("TRUCK %d: Otrzymalem nakaz odjazdu (Sygnał 1)!\n", moj_pid);
                break; 
            }

            
            sem_p(semid, SEM_MUTEX);
            double aktualna_waga_calosc = mag->waga_ladunku_trucka;
            sem_v(semid, SEM_MUTEX);

            if (aktualna_waga_calosc >= LADOWNOSC_CI || obj_ladunku >= OBJETOSC_CI) {
                printf("TRUCK %d: PELNA (%.1f kg) --- ODJEZDZAM ---\n", moj_pid, aktualna_waga_calosc);
                break;
            }

          
            sem_p(semid, SEM_ZAJETE);

           
            if (wymuszony_odjazd) {
                sem_v(semid, SEM_ZAJETE); // Oddajemy paczkę 
                break;
            }

            sem_p(semid, SEM_MUTEX);
            
            // Pobieramy paczkę
            Paczka p = mag->tasma[mag->head];
            mag->head = (mag->head + 1) % POJEMNOSC_TASMY;
            mag->ile_paczek--;
            mag->aktualna_waga_tasmy -= p.waga;

            
            mag->waga_ladunku_trucka += p.waga; 
            
            
            waga_ladunku = mag->waga_ladunku_trucka; 
            obj_ladunku += p.objetosc;

            sem_v(semid, SEM_MUTEX);
            sem_v(semid, SEM_WOLNE); // zwalnianie miejsca workerom

            printf("TRUCK %d: Biorę %c (%.1f kg). Ladunek: %.1f kg\n", moj_pid, p.typ, p.waga, waga_ladunku);
            log_msg(semid, "Truck %d zabral paczke %c (%.1f kg). Stan ladunku: %.1f kg", 
                    moj_pid, p.typ, p.waga, waga_ladunku);
            
            sleep(1); 
        }

        // 3. Wyjazd z doku (Koniec kursu)
        printf("TRUCK %d: Opuszczam dok...\n", moj_pid);
        
        sem_p(semid, SEM_MUTEX);
        mag->pid_truck = 0;          
        mag->waga_ladunku_trucka = 0; 
        sem_v(semid, SEM_MUTEX);
        
        sem_v(semid, SEM_DOK); // dla innej ciężarówki

        if (mag->koniec_pracy) break;


        printf("TRUCK %d: Rozwożę towar... znikam na 5s\n", moj_pid);
        sleep(5); 
        printf("TRUCK %d: Wracam do magazynu!\n", moj_pid);
    }

    shmdt(mag);

    shmdt(mag);
    return 0;
}