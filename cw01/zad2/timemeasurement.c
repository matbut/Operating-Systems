#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>


#ifndef DLL
#include "./../zad1/blocktable.h"
#endif

#ifdef DLL
void * dll;
#endif

#define DATATEMPLATESIZE 20

char *generate_data(int dataSize);

//A static global variable or a function is "seen" only in the file it's declared in
static int staticAlocationFlag;
static int blockSize;
static int blocksNumber;

static char* staticTable;
static char** dynamicTable;

int createTable(int blocksNumber,int blockSize);
int insertBlocks(int number);
int deleteBlocks(int number);
int alterBlocks(int number);
int searchTable(int asciiSum);
char *generate_data(int dataSize);
void printTable();

void printHelp(char *progName);
double countTime(clock_t start, clock_t end);
int measureOperationTime(char c);
char* alocationType(){
  return staticAlocationFlag ? "static" : "dynamic";
}

int main (int argc, char **argv){
  srand(time(0));
  #ifdef DLL
  dll = dlopen("./../zad1/libblocktableshared.so", RTLD_LAZY);
  #endif

	
  int c;
  while (true){    
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"static",    no_argument, &staticAlocationFlag, 1},
          {"dynamic",   no_argument, &staticAlocationFlag, 0},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"blocksNum", required_argument, 0, 'n'},
          {"blockLen",  required_argument, 0, 'l'},
          {"create",    no_argument      , 0, 'c'},
          {"insert",    required_argument, 0, 'i'},
          {"delete",    required_argument, 0, 'd'},
          {"alter",     required_argument, 0, 'a'},
          {"search",    required_argument, 0, 's'},
          {"print",     no_argument, 0, 'p'},
          {"help",      no_argument, 0, 'h'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "cn:l:i:d:a:s:ph",long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch(c){
        case 0:
        //Flags
        break;
        case 'n':
          //Specify blocks number
          blocksNumber=atoi(optarg);
          break;
        case 'l':
          //Specify blocks length
          blockSize=atoi(optarg);
          break;
        case '?':
          /* getopt_long already printed an error message. */
          break;
        case 'c':
        case 'i':
        case 'd':
        case 'a':
        case 'p':
        case 's':
          measureOperationTime(c);
          break;
        case 'h':
          printHelp(argv[0]);
        break;
        default:
          abort();

      }
  }

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    } 

  #ifdef DLL
  dlclose(dll);
  #endif

	return EXIT_SUCCESS;
}

int measureOperationTime(char c){
  
      struct tms startTime;
      clock_t startRealTime;
      startRealTime = times(&startTime);

      switch (c){
        case 'c':
          printf("---- Create %s table with %d bocks ----\n",alocationType(),blocksNumber);
          if(createTable(blocksNumber,blockSize)==EXIT_FAILURE){
            printf("Can't create table.\n");
            return EXIT_FAILURE;
          }
          break;
        case 'i':
          printf("---- Insert %s blocks into %s table ----\n",optarg,alocationType());
          if(insertBlocks(atoi(optarg))==EXIT_FAILURE){
            printf("Can't insert blocks.\n");
            return EXIT_FAILURE;            
          }
          break;
        case 'd':
          printf("---- Delete %s blocks from %s table ----\n",optarg,alocationType());
          if(deleteBlocks(atoi(optarg))==EXIT_FAILURE){
            printf("Can't delete blocks.\n");
            return EXIT_FAILURE;            
          }
          break;
        case 'a':
          printf("---- Alterly insert and delete %s blocks from %s table----\n",optarg,alocationType());
          
          if(alterBlocks(atoi(optarg))==EXIT_FAILURE){
            printf("Can't alterly insert and delete blocks.\n");
            return EXIT_FAILURE;            
          }
          break;
        case 's':
          printf("---- Search match block with %s ascii sum in %s table ----\n",optarg,alocationType());
          searchTable(atoi(optarg));
          break;
        case 'p':
          printf("---- Print %s table ----\n",alocationType());
          printTable();
          break;
      }

      struct tms endTime;
      clock_t endRealTime;
      endRealTime = times(&endTime);

      printf("   Real      User      System\n");
      printf("%lf   ", countTime(startRealTime, endRealTime));
      printf("%lf   ", countTime(startTime.tms_utime, endTime.tms_utime));
      printf("%lf ", countTime(startTime.tms_stime, endTime.tms_stime));
      printf("\n");

      return EXIT_SUCCESS;
}

double countTime(clock_t start, clock_t end) {
  return (double) (end - start) / sysconf(_SC_CLK_TCK);
}
  
int createTable(int blocksNumber,int blockSize){

  #ifdef DLL
  char* (*createStaticTab)(int blocksNumber, int blockSize) = dlsym(dll, "createStaticTab");
  char** (*createDynamicTab)(int blocksNumber, int blockSize) = dlsym(dll, "createDynamicTab");
  #endif

  if(staticAlocationFlag){
    staticTable=createStaticTab(blocksNumber,blockSize);
      return staticTable==NULL ? EXIT_FAILURE : EXIT_SUCCESS;
  //Code copy-paste, but it works 2 times faster 
  }else{ 
    dynamicTable=createDynamicTab(blocksNumber,blockSize);
      return dynamicTable==NULL ? EXIT_FAILURE : EXIT_SUCCESS;
  }
}
int insertBlocks(int number){

  #ifdef DLL
  int (*addBlockStaticTab)(int index,char *content) = dlsym(dll, "addBlockStaticTab");
  int (*addBlockDynamicTab)(char** blockTab,int blocksNumber,int index,char *content) = dlsym(dll, "addBlockDynamicTab");
  #endif

  if(staticAlocationFlag){
    for(int idx=0;idx<number;idx++)
      if(addBlockStaticTab(idx,generate_data(blockSize))==EXIT_FAILURE)
        return EXIT_FAILURE;
  //Code copy-paste, but it works 2 times faster 
  }else{ 
    for(int idx=0;idx<number;idx++)
      if(addBlockDynamicTab(dynamicTable,blocksNumber,idx,generate_data(blockSize))==EXIT_FAILURE)
        return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int deleteBlocks(int number){

  #ifdef DLL
  int (*deleteBlockStaticTab)(int index) = dlsym(dll, "deleteBlockStaticTab");
  int (*deleteBlockDynamicTab)(char** blockTab,int blocksNumber,int index) = dlsym(dll, "deleteBlockDynamicTab");
  #endif

  if(staticAlocationFlag){
    for(int idx=0;idx<number;idx++)
      if(deleteBlockStaticTab(idx)==EXIT_FAILURE)
        return EXIT_FAILURE;
  //Code copy-paste, but it works 2 times faster  
  }else{
    for(int idx=0;idx<number;idx++)
      if(deleteBlockDynamicTab(dynamicTable,blocksNumber,idx)==EXIT_FAILURE)
        return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int alterBlocks(int number){

  #ifdef DLL
  int (*addBlockStaticTab)(int index,char *content) = dlsym(dll, "addBlockStaticTab");
  int (*addBlockDynamicTab)(char** blockTab,int blocksNumber,int index,char *content) = dlsym(dll, "addBlockDynamicTab");
  int (*deleteBlockStaticTab)(int index) = dlsym(dll, "deleteBlockStaticTab");
  int (*deleteBlockDynamicTab)(char** blockTab,int blocksNumber,int index) = dlsym(dll, "deleteBlockDynamicTab");
  #endif

  if(staticAlocationFlag){
    for(int idx=0;idx<number;idx++)
      if(addBlockStaticTab(idx,generate_data(blockSize))==EXIT_FAILURE || deleteBlockStaticTab(idx)==EXIT_FAILURE)
        return EXIT_FAILURE;
  //Code copy-paste, but it works 2 times faster  
  }else{
    for(int idx=0;idx<number;idx++)
      if(addBlockDynamicTab(dynamicTable,blocksNumber,idx,generate_data(blockSize))==EXIT_FAILURE || deleteBlockDynamicTab(dynamicTable,blocksNumber,idx)==EXIT_FAILURE)
        return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;

}
int searchTable(int asciiSum){

  #ifdef DLL
  char* (*searchStaticTab)(int asciiSumTemplate) = dlsym(dll, "searchStaticTab");
  char* (*searchDynamicTab)(char** blockTab,int blocksNumber,int asciiSumTemplate) = dlsym(dll, "searchDynamicTab");
  #endif

  if(staticAlocationFlag)
    searchStaticTab(asciiSum);
  else
    searchDynamicTab(dynamicTable,blocksNumber,asciiSum);
  return EXIT_SUCCESS;
}

void printTable(){

  #ifdef DLL
  int (*printStaticTab)() = dlsym(dll, "printStaticTab");
  int (*printDynamicTab)() = dlsym(dll, "printDynamicTab");
  #endif
    
  if(staticAlocationFlag)
    printStaticTab();
  else
    printDynamicTab(dynamicTable,blocksNumber);

}

char *generate_data(int dataSize) {
	const char* dataTemplate[DATATEMPLATESIZE] = {
		"lectus justo eu arcu. Morbi",
		"vitae velit egestas lacinia. Sed congue, elit sed consequat auctor, nunc nulla vulputate dui, nec tempus",
		"aliquam arcu. Aliquam ultrices iaculis odio. Nam interdum enim non nisi. Aenean",
		"tristique senectus et netus et malesuada fames ac turpis egestas. Aliquam",
		"eu lacus. Quisque imperdiet, erat nonummy ultricies ornare",
		"arcu. Sed eu nibh vulputate mauris sagittis placerat. Cras dictum ultricies ligula. Nullam enim. Sed nulla ante",
		"eu tellus eu augue porttitor interdum. Sed auctor odio a purus. Duis elementum, dui",
		"nibh dolor",
		"consequat enim diam vel arcu. Curabitur ut",
		"euismod",
		"ornare. In faucibus. Morbi vehicula. Pellentesque tincidunt tempus risus. Donec egestas. Duis ac arcu. Nunc mauris. Morbi",
		"Integer aliquam adipiscing lacus. Ut nec urna et arcu imperdiet ullamcorper. Duis at lacus. Quisque purus",
		"Duis risus odio, auctor vitae, aliquet nec, imperdiet nec, leo. Morbi neque tellus, imperdiet",
		"ligula tortor, dictum eu, placerat eget, venenatis a, magna. Lorem ipsum dolor sit amet, consectetuer",
		"placerat. Cras dictum ultricies ligula. Nullam enim. Sed nulla ante, iaculis nec, eleifend non, dapibus rutrum, justo. Praesent luctus.",
		"Suspendisse sagittis. Nullam vitae diam. Proin dolor. Nulla semper tellus id nunc interdum feugiat. Sed nec metus",
		"luctus ut, pellentesque eget, dictum placerat, augue.",
		"dui. Fusce aliquam, enim",
		"sed, est. Nunc laoreet lectus quis massa. Mauris",
		"nec ligula consectetuer rhoncus. Nullam velit dui, semper et, lacinia vitae, sodales at, velit. Pellentesque ultricies dignissim lacus. Aliquam rutrum"
	};

	char* resultData=calloc(dataSize,sizeof(char));
	strncpy(resultData,dataTemplate[rand()%DATATEMPLATESIZE],dataSize-1);
  resultData[dataSize-1]='\0';
	return resultData; 
}
void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s [-sdblcidasph]\n",progName);
  printf("--static        -s        no argument\n");
  printf("--dynamic       -d        no argument\n");
  printf("--blocksNum     -n        required_argument\n");
  printf("--blockLen      -l        required_argument\n");
  printf("--create        -c        no_argument\n");
  printf("--insert        -i        required_argument\n");
  printf("--delete        -d        required_argument\n");
  printf("--alter         -a        required_argument\n");
  printf("--search        -s        required_argument\n");
  printf("--print         -p        no_argument\n");
  printf("--help          -h        no_argument\n");
}
