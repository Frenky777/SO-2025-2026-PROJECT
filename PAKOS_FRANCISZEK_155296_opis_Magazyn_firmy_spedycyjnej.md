Temat 10 – Magazyn firmy spedycyjnej.
W magazynie przy taśmie transportowej pracuje trzech pracowników oznaczonych przez P1, P2 i P3.
Pracownicy układają na taśmę przesyłki o gabarytach odpowiednio A (64x38x8 cm), B (64x38x19
cm) i C (64x38x41 cm). Wszystkie paczki muszą mieć maksymalną wagę 25 kg (wartość losowa z
15
zakresu 0,1kg-25,0kg) – można przyjąć założenie: czym mniejsza paczka tym mniejszy ciężar. W
magazynie znajduje się również pracownik P4, który jest odpowiedzialny za załadunek przesyłek
ekspresowych. Przesyłki ekspresowe dostarczane są osobno - nie są umieszczane na taśmie.
Przesyłki ekspresowe mają wyższy priorytet – ich załadunek musi odbyć się w pierwszej kolejności.
Pakiet przesyłek ekspresowych może zawierać tylko przesyłki o gabarytach A, B i C, których waga
(pojedynczej przesyłki) jest mniejsza niż 25kg.
Na końcu taśmy stoi ciężarówka o ładowności W jednostek [kg] oraz dopuszczalnej objętości ładunku
V [m3], którą należy zawsze załadować do pełna. Wszyscy pracownicy starają się układać przesyłki
na taśmie najszybciej jak to możliwe. Taśma może przetransportować w danej chwili maksymalnie K
przesyłek. Jednocześnie jednak taśma ma ograniczony udźwig: maksymalnie M jednostek masy, tak,
że niedopuszczalne jest położenie np. samych tylko najcięższych przesyłek (K*25kg > M). Przesyłki
„zjeżdżające" z taśmy muszą od razu trafić na samochód dokładnie w takiej kolejności jak zostały
położone na taśmie. Po zapełnieniu ciężarówki na jej miejsce pojawia się natychmiast (jeżeli jest
dostępna!) nowa o ładowności W oraz dopuszczalnej objętości ładunku V. Łączna liczba ciężarówek
wynosi N. Ciężarówki rozwożą przesyłki i po czasie Ti wracają do magazynu.
Na polecenie dyspozytora (sygnał 1) ciężarówka, która w danym momencie stoi przy taśmie może
odjechać z magazynu z niepełnym ładunkiem.
Po otrzymaniu od dyspozytora polecenia (sygnał 2) pracownik P4 dostarcza do stojącej przy taśmie
ciężarówki pakiet przesyłek ekspresowych.
Po otrzymaniu od dyspozytora polecenia (sygnał 3) pracownicy kończą pracę. Ciężarówki kończą
pracę po rozwiezieniu wszystkich przesyłek.
Napisz programy symulujące działanie dyspozytora, pracowników i ciężarówek. Raport z przebiegu
symulacji zapisać w pliku (plikach) tekstowym.

github: https://github.com/Frenky777/SO-2025-2026-PROJECT
testy:
Test 1: Przepełnienie bufora taśmy, sprawdzam czy pracownicy wstrzymują prace gdy na taśmie zabraknie miejsca.
Test 2: Przekroczenie udźwigu taśmy, sprawdzam co sie stanie jak dam najciezsze paczki.
Test 3: Obsługa paczki ekspresowej, sprawdzam czy P4 omija kolejke tasmy.
Test 4: Odjazd niepełnej cieżarówki, sprawdzam czy odjedzie jak wymusze odjechanie.
Test 5: Koniec programu, sprawdzam czy program nie jest wirusem i nie zostawia jakichs "zombie".