# ---------------------------------------------------------
# wpisz: make           -> zbuduje tylko main 
# wpisz: make worker    -> zbuduje tylko worker
# wpisz: make truck     -> zbuduje tylko truck
# wpisz: make all       -> zbuduje wszystko na raz
# ---------------------------------------------------------


default: main

# ---instrukcje ---

# Buduje tylko głównego zarządcę 
main: main.c sem.c
	gcc -o main main.c sem.c -pthread

# Buduje zwykłego pracownika
worker: worker.c sem.c
	gcc -o worker worker.c sem.c -pthread

# Buduje szybkiego pracownika
fast_worker: fast_worker.c sem.c
	gcc -o fast_worker fast_worker.c sem.c -pthread

# Buduje ciężarówkę
truck: truck.c sem.c
	gcc -o truck truck.c sem.c -pthread

# Buduje dyspozytora
dispatcher: dispatcher.c sem.c
	gcc -o dispatcher dispatcher.c sem.c -pthread

#buduje wszystko naraznbmh 
all: main worker fast_worker truck dispatcher

g: main worker truck

# Sprzątanie
clean:
	rm -f main worker fast_worker truck dispatcher