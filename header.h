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
//ftok

#define ID_PROJEKTU 'M'
#define MAX_WAGA_TASMY 400.0   // M 
#define POJEMNOSC_TASMY 10     // K 
#define MAX_WAGA_PACZKI 25.0   //
#define LADOWNOSC_CI 1000.0    // W 
#define OBJETOSC_CI 500        // V 


#define LIMIT_CIEZAROWEK 3 //limit ciezarowek


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