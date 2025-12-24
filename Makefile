CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

TARGETS = dispatcher worker express_worker truck

all: $(TARGETS)

# Kompilacja Dyspozytora (Proces główny)
dispatcher: src/main.c src/ipc_utils.c
	$(CC) $(CFLAGS) $^ -o $@

# Kompilacja Pracowników (P1, P2, P3)
worker: src/worker.c src/ipc_utils.c
	$(CC) $(CFLAGS) $^ -o $@

# Kompilacja Pracownika Ekspresowego (P4)
#express_worker: src/express_worker.c src/ipc_utils.c
#	$(CC) $(CFLAGS) $^ -o $@

# Kompilacja Ciężarówki
#truck: src/truck.c src/ipc_utils.c
#	$(CC) $(CFLAGS) $^ -o $@

# Czyszczenie plików binarnych i tymczasowych
clean:
	rm -f $(TARGETS) *.o