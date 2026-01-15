#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <stdarg.h>

#define RUMUNIA_BLUE   "\033[1;34m" // Niebieski truck
#define RUMUNIA_DARK_BLUE   "\033[0;34m" // Ciemno niebieski truck
#define RUMUNIA_YELLOW "\033[1;33m" // Zolty slave (worker)
#define RUMUNIA_RED    "\033[1;31m" // Czerwony (fast worker)
#define RUMUNIA_GREEN    "\033[1;32m" // Zielony do maina
#define OCZYSZCZANIE    "\033[0m"    // Reset 

//ftok

#define ID_PROJEKTU 'M'  // do pamieci wspoldzielonej
#define MAX_WAGA_TASMY 400.0   // M 
#define POJEMNOSC_TASMY 5     // K 
#define MAX_WAGA_PACZKI 25.0   //
#define LADOWNOSC_CI 100.0    // W 
#define OBJETOSC_CI 500        // V 
#define TI 10 // czas oczekiwania ciezarowek Ti

#define LIMIT_CIEZAROWEK 15//limit ciezarowek

#define LOGOWANIE_DO_PLIKU 1 // 1 Wlaczone, 0 wylaczone

// --- SEMAFORY 
#define SEM_MUTEX 0      // Chroni pamięć
#define SEM_WOLNE 1      // Liczy wolne miejsca na taśmie
#define SEM_ZAJETE 2   
#define SEM_DOK 3  // Liczy paczki na taśmie 
#define LICZBA_SEM 4

typedef struct {
    int id_pracownika;   
    char typ;            
    double waga;        
    int objetosc;        
} Paczka;

typedef struct {
    // kolejka FIFO
    Paczka tasma[POJEMNOSC_TASMY];
    int head; // bierze ciężarówka
    int tail; // kładzie pracownik
    int ile_paczek;
    double aktualna_waga_tasmy;
    int objetosc_ladunku_trucka;
    double waga_ladunku_trucka; // zeby P4 wiedział ile jest
    pid_t pid_truck;            // do sygnałów
    pid_t pid_p4;

    int koniec_pracy;
} Magazyn;


void sem_p(int semid, int sem_num); // Czekaj (Opuść)
void sem_v(int semid, int sem_num); // Sygnalizuj (Podnieś)

void log_msg(int semid, const char *format, ...); //logger
void clear_log(int semid); //celaner do logow

#endif