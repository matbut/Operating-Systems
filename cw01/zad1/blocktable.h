/*
example include file
*/

/*
Mateusz Buta

Zadanie 1. Alokacja tablicy z wskaźnikami na bloki pamięci zawierające znaki (25%)
Zaprojektuj i przygotuj zestaw funkcji (bibliotekę) do zarządzania tablicą bloków zawierającą znaki.

Biblioteka powinna umozliwiać:

- tworzenie i usuwanie tablicy

- dodanie i usunięcie bloków na które wskazują wybrane indeksy elementów tablicy

- wyszukiwanie bloku w tablicy, którego suma znaków (kodów Ascii) w bloku jest najbliższa elementowi o zadanym numerze,

Tablice i bloki alokowane powinny być przy pomocy funkcji calloc (alokacja dynamiczna) jak również powinny wykorzystywać tablicę dwuwymiarowa (statyczny przydział pamięci).

Przygotuj plik Makefile, zawierający polecenia kompilujące pliki źródłowe biblioteki oraz tworzące biblioteki w dwóch wersjach: statyczną i dzieloną.
*/

//Statically allocated table
char* makeStaticTab(int blockNumber);
void deleteStaticTab();
void addBlockStaticTab(int index,char *content);
void deleteBlockStaticTab(int index);
//Dynamically allocated table

char** makeDynamicTab(int blockNumber,int blockSize);
void deleteDynamicTab(char** blockTab);
void addBlockDynamicTab(char** blockTab,int index,char *content);
void deleteBlockDynamicTab(char** blockTab,int index);
