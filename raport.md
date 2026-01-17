# Raport 

## Spis treści

- [Podstawowe informacje](#basic-info)
- [Środowisko](#environment)
- [Uruchamianie](#running)
  - [Parametry przekazywane poprzez arguementy](#arg-params)
  - [Parametry wyznaczane losowo](#random-params)
- [Architektura](#arch)
  - [Typy procesów](#proc-types)
    - [Main](#main-type)
    - [Dyspozytor](#dispatcher-type)
    - [Truck](#truck-type)
    - [PracownikP1-P3](#p1-p3-type)
    - [PracownikP4](#p4-type)
- [Testy](#tests)

<a name="#basic-info"></a>
## Informacje

**Autor**: Franciszek Pakos

**Temat**: 10 - Magazyn Firmy spedycyjnej

<a name="#environment"></a>
## Środowisko

**System operacyjny**: Windows 11 // WSL2 // Ubuntu 24.04.3

**Zarządzanie kompilacją**: Makefile

**Kompilator**: GCC

Instrukcja uruchomienia
Skompiluj projekt:

Bash
make all
Uruchom główny proces symulacji:

Bash
./main


Bash
./dispatcher
Parametry uruchomieniowe
Główny proces main automatycznie powołuje do życia procesy potomne, przekazując im odpowiednie argumenty funkcją execl:

Worker: Otrzymuje ID (1, 2 lub 3), co definiuje typ generowanych paczek (A, B lub C).

Truck: Uruchamiany bez argumentów, działa w pętli obsługi doku.

Fast Worker: Uruchamiany jako oddzielny proces dedykowany do paczek ekspresowych.

Losowość w systemie
Zgodnie z wymaganiami, system wprowadza elementy losowe:

Waga paczek: Losowana w przedziale 0.1–25.0 kg (Workerzy) oraz 1.0–23.0 kg (Fast Worker).

Czas powrotu ciężarówki: Czas TI oraz losowe opóźnienia w pętli.


Architektura Systemu
System opiera się na modelu wieloprocesowym. Komunikacja między procesami odbywa się poprzez mechanizmy IPC Systemu V.

Mechanizmy IPC i Synchronizacja
Pamięć Współdzielona (Shared Memory):

Przechowuje strukturę Magazyn, zawierającą bufor cykliczny taśmy, liczniki wagi/objętości, PID-y procesów oraz flagi sterujące 
(koniec_pracy, p4_priorytet).

Semafory (Semaphores): Wykorzystano zestaw 4 semaforów do ścisłej synchronizacji:

SEM_MUTEX (0): Mutex binarny chroniący sekcję krytyczną (dostęp do pamięci współdzielonej).

SEM_WOLNE (1): Zlicza wolne miejsca na taśmie (blokuje producentów, gdy taśma pełna).

SEM_ZAJETE (2): Zlicza paczki gotowe do pobrania (blokuje konsumenta/ciężarówkę, gdy taśma pusta).

SEM_DOK (3): Semiprywatny semafor zapewniający, że w doku znajduje się tylko jedna ciężarówka naraz.

Opis Procesów
Main (Zarządca)
Inicjalizuje zasoby (tworzy IPC), uruchamia procesy potomne (fork + exec) i nadzoruje ich pracę (wait). Posiada handler sygnału SIGINT (Ctrl+C), który gwarantuje bezpieczne usunięcie semaforów i pamięci w przypadku przerwania programu.

Dyspozytor (Panel sterowania)
Interfejs użytkownika pozwalający na wysyłanie sygnałów do procesów:

1 -> Wysyła SIGUSR1 do Trucka (wymuszony odjazd).

2 -> Wysyła SIGUSR2 do Fast Workera (zlecenie ekspresowe).

3 -> Ustawia flagę końca pracy w pamięci dzielonej.

Truck (Konsument)
Proces reprezentujący ciężarówkę.

Pobiera paczki z taśmy zgodnie z zasadą FIFO.

Implementuje mechanizm ustępowania pierwszeństwa: Przed wejściem do sekcji krytycznej sprawdza flagę p4_priorytet. Jeśli jest ustawiona, zwalnia Mutex i usypia, pozwalając pracownikowi P4 załadować paczkę ekspresową poza kolejnością.

Odjeżdża po zapełnieniu (waga/objętość) lub na żądanie dyspozytora.

Pracownicy P1-P3 (Producenci)
Symulują standardowych pracowników.

Każdy generuje specyficzny typ paczki (A, B, C).

Sprawdzają limity udźwigu taśmy przed położeniem towaru.

Działają w oparciu o semafory SEM_WOLNE i SEM_MUTEX.

Pracownik P4 (Priorytet)
Obsługuje przesyłki ekspresowe.

Działa w trybie uśpienia, oczekując na sygnał SIGUSR2.

Po otrzymaniu sygnału ustawia flagę p4_priorytet w pamięci współdzielonej.

Omija kolejkę taśmy transportowej i ładuje paczkę bezpośrednio na ciężarówkę, wykorzystując mechanizm priorytetu zaimplementowany w procesie Truck.

Scenariusze Testowe:


Test 1: Przepełnienie bufora taśmy (Synchronizacja)
Cel: Sprawdzenie poprawności modelu Producent-Konsument.


Test 2: Priorytet paczki ekspresowej (P4)
Cel: Weryfikacja mechanizmu priorytetów i obsługi sygnałów.


Test 3: Przekroczenie udźwigu taśmy
Cel: Weryfikacja logiki biznesowej i ograniczeń fizycznych.

Test 4: Wymuszony odjazd
Cel: Sprawdzenie obsługi sygnału SIGUSR1 przez ciężarówkę.


Test 5: Bezpieczne zamykanie i czyszczenie zasobów
Cel: Weryfikacja braku wycieków zasobów IPC.

