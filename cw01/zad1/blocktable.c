/*
Created by Mateusz Buta 10/03/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
						
#include "blocktable.h"

//Statically allocated table
char DATA[MAXBLOCKSNUMBER][MAXBLOCKSIZE];

//Rrepresents if table row is occupied
bool OCCUPIED[MAXBLOCKSNUMBER];

//Declared by user blocks number(tale size)
int BLOCKSNUMBER=0;

//Declared by user block size(string length)
int BLOCKSIZE=0;

char* createStaticTab(int blocksNumber, int blockSize){
	if (blocksNumber>MAXBLOCKSNUMBER || blockSize>MAXBLOCKSIZE)
		return NULL;
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
		return EXIT_FAILURE;
	OCCUPIED[index]=true;
	strcpy(DATA[index],content);
	return EXIT_SUCCESS;
}

int deleteBlockStaticTab(int index){
	if(index>=BLOCKSNUMBER)
		return EXIT_FAILURE;
	OCCUPIED[index]=false;
	return EXIT_SUCCESS;
}

void printStaticTab(){
	for(int idx=0;idx<BLOCKSNUMBER;idx++){
		printf("[%d]",idx);
		if(OCCUPIED[idx])
			printf("%s\n",DATA[idx]);
		else
			printf("EMPTY\n");
	}
}

char* searchStaticTab(int asciiSumTemplate){
	int foundDiff=INT_MAX;
	char *foundString=NULL;

	for(int idx=0;idx<BLOCKSNUMBER;idx++){
		if(OCCUPIED[idx]){
			int asciiSum=0;
			for(int strIdx;strIdx<BLOCKSIZE;strIdx++){
				asciiSum+=DATA[idx][strIdx];
			}
			int diff=abs(asciiSumTemplate-asciiSum);
			if(diff<foundDiff){
				foundDiff=diff;
				foundString=&DATA[idx][0];
			}
		}	
	}
	return foundString;
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
		return EXIT_FAILURE;

	blockTab[index]=malloc(strlen(content));
	strcpy(blockTab[index],content);
	return EXIT_SUCCESS;
}

int deleteBlockDynamicTab(char** blockTab,int blocksNumber,int index){
	if(index>=blocksNumber || blockTab[index]==NULL)
		return EXIT_FAILURE;

	free(blockTab[index]);
	blockTab[index]=NULL;
	return EXIT_SUCCESS;
}

char* searchDynamicTab(char** blockTab,int blocksNumber,int asciiSumTemplate){
	int foundDiff=INT_MAX;
	char *foundString=NULL;

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
				foundString=blockTab[idx];
			}
		}	
	}
	return foundString;
}

void printDynamicTab(char** blockTab,int blocksNumber){
	for(int idx=0;idx<blocksNumber;idx++){
		printf("[%d]",idx);
		if(blockTab[idx]!=NULL)
			printf("%s\n",blockTab[idx]);
		else
			printf("EMPTY\n");
	}
}