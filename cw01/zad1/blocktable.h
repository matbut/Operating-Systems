/*
Created by Mateusz Buta 10/03/2018
*/

//Statically allocated table


char* createStaticTab(int blocksNumber, int blockSize);
void deleteStaticTab();
int addBlockStaticTab(int index,char *content);
int deleteBlockStaticTab(int index);
char* searchStaticTab(int asciiSumTemplate);
void printStaticTab();

//Dynamically allocated table

char** createDynamicTab(int blocksNumber,int blockSize);
void deleteDynamicTab(char** blockTab,int blocksNumber);
int addBlockDynamicTab(char** blockTab,int blocksNumber,int index,char *content);
int deleteBlockDynamicTab(char** blockTab,int blocksNumber,int index);
char* searchDynamicTab(char** blockTab,int blocksNumber,int asciiSumTemplate);
void printDynamicTab(char** blockTab,int blocksNumber);
