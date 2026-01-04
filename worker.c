#include "header.h"

int main(int argc, char *argv[]) {

    int id_prac;
    if (argc > 1) {
        id_prac = atoi(argv[1]);
    } else {
        id_prac = 1; 
    }
    // srand(time(NULL) ^ getpid());
    srand(time(NULL) ^ (getpid() * 1000));



    // podlaczenie do pamieci wspoldzielonej i zasobow
    key_t key = ftok(".", ID_PROJEKTU);
    int semid = semget(key, LICZBA_SEM, 0666);
    int shmid = shmget(key, sizeof(Magazyn), 0666);
    
    if (shmid == -1 || semid == -1) {
        perror("Blad podlaczenia pracownika");
        exit(1);
    }
    
    Magazyn *mag = (Magazyn*)shmat(shmid, NULL, 0);
    printf("Pracownik P%d zaczyna prace.\n", id_prac);

    while (1) {
        if (mag->koniec_pracy) break;

        //przygotowanie packzi
        Paczka p;
        p.id_pracownika = id_prac;
        

        int los = rand() % 3; 
        if (los == 0) { p.typ = 'A'; p.objetosc = 10; } 
        else if (los == 1) { p.typ = 'B'; p.objetosc = 20; } 
        else { p.typ = 'C'; p.objetosc = 40; } 
        

        p.waga = ((double)(rand() % 250) / 10.0) + 0.1;

        sleep(1); 

        // kladzenie na tasmie
        
        // czekanie na wolne miejsce
        sem_p(semid, SEM_WOLNE);
        // blokowanie pamieci
        sem_p(semid, SEM_MUTEX);


        if (mag->aktualna_waga_tasmy + p.waga > MAX_WAGA_TASMY) {
            printf("P%d: Za ciezko (%.1f kg)! Czekam...\n", id_prac, mag->aktualna_waga_tasmy);
            
        
            sem_v(semid, SEM_MUTEX);
            sem_v(semid, SEM_WOLNE); 
            sleep(1); 
            continue; 
        }

        // polozenie paczki
        mag->tasma[mag->tail] = p;
        mag->tail = (mag->tail + 1) % POJEMNOSC_TASMY;
        mag->ile_paczek++;
        mag->aktualna_waga_tasmy += p.waga;

        printf("P%d: Dal paczke %c (%.1f kg). Stan: %d/%d\n", 
               id_prac, p.typ, p.waga, mag->ile_paczek, POJEMNOSC_TASMY);

        // odblokowanie pamieci
        sem_v(semid, SEM_MUTEX);
        sem_v(semid, SEM_ZAJETE);
    }

    shmdt(mag);
    return 0;
}