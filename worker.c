#include "header.h"

int main(int argc, char *argv[]) {

    int id_prac;
    if (argc > 1) {
        id_prac = atoi(argv[1]);
    } else {
        id_prac = 1; 
    }
     srand(time(NULL) ^ getpid());



    // podlaczenie do pamieci wspoldzielonej i zasobow
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0600);
    int shmid = shmget(key, sizeof(Magazyn), 0600);
    
    if (shmid == -1 || semid == -1) {
        perror("Blad podlaczenia pracownika");
        exit(1);
    }
    
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);
    printf(RUMUNIA_YELLOW"Pracownik P%d zaczyna prace."OCZYSZCZANIE"\n", id_prac);

    while (1) {
        if (mag->koniec_pracy) break;

        //przygotowanie packzi
        Paczka p;
        p.id_pracownika = id_prac;
        

        if (id_prac == 1) {
            // P1 robi tylko małe A
            p.typ = 'A';
            p.objetosc = 10; 
            // Waga 0.1 - 5.0 kg
            p.waga = ((rand() % 50) / 10.0) + 0.1;
        }
        else if (id_prac == 2) {
            // P2 robi tylko średnie B
            p.typ = 'B';
            p.objetosc = 20;
            // Waga 5.0 - 15.0 kg
            p.waga = ((rand() % 100) / 10.0) + 5.0;
        }
        else {
            // P3 robi tylko duże C 
            p.typ = 'C';
            p.objetosc = 40;
            // Waga 15.0 - 25.0 kg
            p.waga = ((rand() % 100) / 10.0) + 15.0;
        }
        //sleep(1); 
        sem_p(semid, SEM_WOLNE);

        // blokujemy dostep do pamieci
        sem_p(semid, SEM_MUTEX);

        // Jesli ktos w miedzyczasie oglosil koniec pracy
        if (mag->koniec_pracy) {
            sem_v(semid, SEM_MUTEX);
            sem_v(semid, SEM_WOLNE); // Oddajemy pobrany slot
            break;
        }

        //Sprawdzenie wagi 
        if (mag->aktualna_waga_tasmy + p.waga > MAX_WAGA_TASMY) {
 
            
            sem_v(semid, SEM_MUTEX); 
            sem_v(semid, SEM_WOLNE); 
            
            usleep(100000); // 0.1 sekundy

            continue; 
        }

        //Polozenie paczki
        mag->tasma[mag->tail] = p;
        mag->tail = (mag->tail + 1) % POJEMNOSC_TASMY;
        mag->ile_paczek++;
        mag->aktualna_waga_tasmy += p.waga;



        double log_waga = mag->aktualna_waga_tasmy;
        int log_ile = mag->ile_paczek;

        sem_v(semid, SEM_MUTEX);
        
        sem_v(semid, SEM_ZAJETE);

        printf(RUMUNIA_YELLOW "P%d: Położył paczkę %c (%.1f kg). Waga taśmy: %.1f kg" OCZYSZCZANIE "\n", 
               id_prac, p.typ, p.waga, log_waga);
        
        log_msg(semid, "Pracownik P%d polozyl paczke %c (%.1f kg). Waga tasmy: %.1f kg", 
                id_prac, p.typ, p.waga, log_waga);

        
        
    }

        


    shmdt(mag);
    return 0;
}