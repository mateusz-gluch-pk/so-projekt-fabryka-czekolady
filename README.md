# so-projekt-fabryka-czekolady
## Temat 3 – Fabryka czekolady.

### Opis - wymagania

 - Fabryka produkuje dwa rodzaje czekolady. 
 - Na stanowisku produkcyjnym 1 jest produkowana czekolada ze składników A, B i C. 
 - Na stanowisku produkcyjnym 2 jest produkowana czekolada ze składników A, B i D. 
 - Składniki przechowywane są w magazynie o pojemności N jednostek. 
 - Składniki A i B zajmują jedną jednostkę magazynową, składnik C dwie, a składnik D trzy jednostki. 
 - Składniki pobierane są z magazynu, przenoszone na stanowisko produkcyjne 1 lub 2 i używane do produkcji czekolady (typ_1 lub typ_2). 
 - Jednocześnie trwają dostawy składników A, B, C i D do magazynu.
 - Składniki pochodzą z 4 niezależnych źródeł i dostarczane są w nieokreślonych momentach czasowych. 
 - Fabryka przyjmuje do magazynu maksymalnie dużo składników dla zachowania płynności produkcji.
 - Fabryka kończy pracę po otrzymaniu polecenia_1 od dyrektora. Magazyn kończy pracę po otrzymaniu polecenia_2 od dyrektora. 
 - Dostawcy przerywają dostawy po otrzymaniu polecenia_3 od dyrektora. 
 - Fabryka i magazyn kończą pracę jednocześnie po otrzymaniu polecenia_4 od dyrektora.
 - Aktualny stan magazynu zapisany w pliku, po ponownym uruchomieniu stan magazynu jest odtwarzany z pliku.

### Cel: 

Napisz programy dla procesów: dyrektor, dostawca i pracownik reprezentujących odpowiednio:
dyrektora, dostawców produktów A, B, C i D oraz pracowników na stanowiskach 1 i 2. 

Raport z przebiegu symulacji zapisać w pliku (plikach) tekstowym.

### Cel (dodatkowy):
 - Polecenie 5, 6, 7, 8 - uruchomienie fabryki, magazynu i dostawców przez dyrektora
 - Pracownik jest generyczny (ten sam kod działa jako pracownik 1 i 2)
 - Dostawca jest generyczny (ten sam kod działa jako dostawca A--D)
 - Dynamicznie przydzielana pamięć dla symulacji magazynu

### TESTY

   1. Dostawa składnika: A, B, C, D -> oczekujemy zwiększenia stanu magazynowego o 1 i zwiększenia zajętości magazynu o X
   2. Pobranie składnika z magazynu A, B, C, D -> oczekujemy zmniejszenia stanu magazynowego o 1 i zmniejszenia zajętości magazynu o X
   3. Pobranie składnika z pustego magazynu -> błąd bez zamknięcia symulacji -- pracownik czeka
   4. Dostawa składnika do pełnego magazynu -> błąd bez zamknięcia symulacji -- dostawca czeka
   5. Wyprodukowanie czekolady 1, 2 (bez pobrania z magazynu; pracownik pobiera z inf. magazynu)  
   6. Polecenie 1: Próba wyprodukowania czekolady kończy się błędem
   7. Polecenie 2: Próba dostawy/pobrania kończy się błędem
   8. Polecenie 3: Próba dostawy kończy się błędem
   9. Polecenie 4: złożenie polecenia 1 i 2




