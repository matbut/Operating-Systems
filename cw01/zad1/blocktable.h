/*
Created by Mateusz Buta 10/03/2018
*/

#ifndef BLOCK_TABLE_H
#define BLOCK_TABLE_H

#define MAXBLOCKSNUMBER 10000005
#define MAXBLOCKSIZE 100

//Statically allocated table

char* createStaticTab(int , int );
void deleteStaticTab();
int addBlockStaticTab(int ,char *);
int deleteBlockStaticTab(int );
char* searchStaticTab(int );
void printStaticTab();

//Dynamically allocated table

char** createDynamicTab(int ,int );
void deleteDynamicTab(char** ,int );
int addBlockDynamicTab(char** ,int ,int ,char *);
int deleteBlockDynamicTab(char** ,int ,int );
char* searchDynamicTab(char** ,int ,int );
void printDynamicTab(char** ,int );

#endif //BLOCK_TABLE_H
