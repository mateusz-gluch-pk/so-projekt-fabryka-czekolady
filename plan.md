# Taski

## Procesy (osoby)

### Dyrektor
 - zna parametry fabryki:
   - listę przepisów
   - listę obiektów *magazyn* -- <we/wy>
   - listę obiektów *stanowisko*
   - listę aktywnych dostawców
   - listę aktywnych pracowników

### Pracownik
 
    Atrybuty:
        - Stanowisko *Station
        - Magazyn *Warehouse

    Metody:
        - `work()`
        - `create()`
        - `stop()`

### Dostawca

  Atrybuty:
    - Składnik *Item
    - Intervał *DeliveryInterval
    - Magazyn docelowy *Warehouse

  Metody:
    - Pętla główna (work)
    - `deliver()`
    - `stop()`

 - Inicjalizacja - wybór składnika, wybór interwału i seeda dostarczania (delay = base_delay + random(-1, 1)*random_factor) 
 - Pętla główna (dostarczaj): work(*Warehouse)
 - Dostarcz składnik do magazynu na żądanie: ->deliver(*Warehouse)
 - Przerwij dostarczanie: stop()

## Stanowiska - klasy, pliki z danymi 

### Magazyn - najlepiej sproxować jakimś CRUD iface

Atrybuty:
 - shared memory
 - semafor
 - plik ze stanem magazynu

Metody (public):
 - `int put(char* ingredient)`
 - `int pick(char* ingredient)`
 - `int get_state(char* ingredient)`
 - `int get_all(char* ingredient)`
 - `int stats()`

Metody (private):
 - `int sync()`
 - `int wait()`

*magazyn wyjściowy -- zwykły magazyn o pojemnośći infinite* 

Shared memory proxowany semaforem *na wątkach to to jest mapa z mutexem*


### Stanowisko - najlepiej (również) sproxować CRUDem...
Atrybuty:
 - semafor
 - `Recipe* przepis`

Metody:
 - `int take()`
 - `int leave()`
 - `char* create(char** ingredients)`
 - `char* recipe()`
 - ``

### Obiekty w symulacji
 - Przepis:
   - inputs: char**
   - outputs: Item*

 - Item:
   - name:
   - size:
   - (optional: value)

 - Składnik == Czekolada -> char*


