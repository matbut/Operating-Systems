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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <blocktable.h>
#include <string.h>

#define MAXBLOCKNUMBER 100
#define MAXBLOCKSIZE 100

//Statically allocated table

char data[MAXBLOCKNUMBER][MAXBLOCKSIZE];
bool occupied[MAXBLOCKNUMBER];

char* makeStaticTab(int blockNumber){
	for(int i=0;i<blockNumber;i++){
		occupied[i]=false;
	}
	return &data[0][0];
}
void deleteStaticTab();



void addBlockStaticTab(int index,char content[]){
	occupied[index]=true;
	strcpy(data[index],content);
}
void deleteBlockStaticTab(int index){
	occupied[index]=false;
}

//Dynamically allocated table

char** makeDynamicTab(int blockNumber,int blockSize){
	char**blockTab = calloc(blockNumber,sizeof(char*));

	for(int i=0;i<blockNumber;i++){
		blockTab=NULL;
	}
	return blockTab;
}
void deleteDynamicTab(char** blockTab){
	int tableSize=sizeof(blockTab)/sizeof(char*);

	for(int i=0;i<tableSize;i++){
		free(blockTab[i]);
	}	
	free(blockTab);
}

void addBlockDynamicTab(char** blockTab,int index,char content[]){
	blockTab[index]=content;
	blockTab[index]=malloc(sizeof(content));
	strcpy(blockTab[index],content);
	
}
void deleteBlockDynamicTab(char** blockTab,int index){
	free(blockTab[index]);
	blockTab[index]=NULL;
}


