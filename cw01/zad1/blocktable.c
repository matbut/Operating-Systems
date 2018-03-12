/*
Created by Mateusz Buta 10/03/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define MAXBLOCKSNUMBER 100
#define MAXBLOCKSIZE 100

//Statically allocated table

char DATA[MAXBLOCKSNUMBER][MAXBLOCKSIZE];
bool OCCUPIED[MAXBLOCKSNUMBER];
int BLOCKSNUMBER=0;
int BLOCKSIZE=0;

char* createStaticTab(int blocksNumber, int blockSize){
	BLOCKSNUMBER=blocksNumber;
	BLOCKSIZE=blockSize;
	for(int idx=0;idx<BLOCKSNUMBER;idx++){
		OCCUPIED[idx]=false;
	}
	return &DATA[0][0];
}

void deleteStaticTab(){
	BLOCKSNUMBER=0;
	BLOCKSIZE=0;
}

int addBlockStaticTab(int index,char *content){
	if(OCCUPIED[index] || index>=BLOCKSNUMBER || strlen(content)>BLOCKSIZE)
		return -1;
	OCCUPIED[index]=true;
	strcpy(DATA[index],content);
	return 0;
}

int deleteBlockStaticTab(int index){
	if(index>=BLOCKSNUMBER)
		return -1;
	OCCUPIED[index]=false;
	return 0;
}

void printStaticTab(){
	printf("Static table\n");
	for(int idx=0;idx<BLOCKSNUMBER;idx++){
		printf("[%2.1d]",idx);
		if(OCCUPIED[idx])
			printf("%s\n",DATA[idx]);
		else
			printf("EMPTY\n");
	}
}

char* searchStaticTab(int asciiSumTemplate){

	int foundDiff=INT_MAX;
	int foundIdx=-1;

	for(int idx=0;idx<BLOCKSNUMBER;idx++){
		if(OCCUPIED[idx]){
			int asciiSum=0;
			for(int strIdx;strIdx<BLOCKSIZE;strIdx++){
				asciiSum+=DATA[idx][strIdx];
			}
			int diff=abs(asciiSumTemplate-asciiSum);
			if(diff<foundDiff){
				foundDiff=diff;
				foundIdx=idx;
			}
		}	
	}
	if(foundIdx==-1)
		return NULL;
	return &DATA[foundIdx][0];
}

//Dynamically allocated table

char** createDynamicTab(int blocksNumber,int blockSize){
	char**blockTab = calloc(blocksNumber,sizeof(char*));

	for(int idx=0;idx<blocksNumber;idx++){
		blockTab[idx]=NULL;
	}
	return blockTab;
}

void deleteDynamicTab(char** blockTab,int blocksNumber){


	for(int i=0;i<blocksNumber;i++){
		free(blockTab[i]);
	}	
	free(blockTab);
}

int addBlockDynamicTab(char** blockTab,int blocksNumber,int index,char *content){

	if(index>=blocksNumber || blockTab[index]!=NULL)
		return -1;

	blockTab[index]=malloc(strlen(content));
	strcpy(blockTab[index],content);
	return 0;
}

int deleteBlockDynamicTab(char** blockTab,int blocksNumber,int index){

	if(index>=blocksNumber)
		return -1;
	free(blockTab[index]);
	blockTab[index]=NULL;
	return 0;
}

char* searchDynamicTab(char** blockTab,int blocksNumber,int asciiSumTemplate){

	int foundDiff=INT_MAX;
	int foundIdx=-1;

	for(int idx=0;idx<blocksNumber;idx++){
		if(blockTab[idx]!=NULL){
			int asciiSum=0;
			int strLen=strlen(blockTab[idx]);
			for(int strIdx;strIdx<strLen;strIdx++){
				asciiSum+=blockTab[idx][strIdx];
			}
			int diff=abs(asciiSumTemplate-asciiSum);
			if(diff<foundDiff){
				foundDiff=diff;
				foundIdx=idx;
			}
		}	
	}
	if(foundIdx==-1)
		return NULL;
	return blockTab[foundIdx];
}

void printDynamicTab(char** blockTab,int blocksNumber){
	printf("Dynamic table\n");
	for(int idx=0;idx<blocksNumber;idx++){
		printf("[%2.1d]",idx);
		if(blockTab[idx]!=NULL)
			printf("%s\n",blockTab[idx]);
		else
			printf("EMPTY\n");
	}
}