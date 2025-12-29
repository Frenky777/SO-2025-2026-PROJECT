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

//(ftok)
#define ID_PROJEKTU 'M' 


typedef struct {
    int liczba_paczek_na_tasmie;
    double aktualna_waga_na_tasmie;
    int czy_ciezarowka_jest_dostepna;
    
} Magazyn;


#define SEM_DOSTEP_DO_PAMIECI 0  // Mutex 
#define SEM_PELNA_TASMA 1        // Miejsce na taśmie

#endif