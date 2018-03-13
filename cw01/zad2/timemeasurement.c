#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "./../zad1/blocktable.h"

#define DATATEMPLATESIZE 20

char *generate_data(int dataSize);

//A static global variable or a function is "seen" only in the file it's declared in
static int staticAlocationFlag;
static int blockSize;
static int blocksNumber;

int main (int argc, char **argv){
	srand(time(0));
  int c;
  while (1){    
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
          {"add",       required_argument, 0, 'a'},
          {"delete",    required_argument, 0, 'd'},
          {"alter",     required_argument, 0, 'r'},
          {"search",    required_argument, 0, 's'},
          {"help",      required_argument, 0, 'h'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "cn:l:a:d:r:s:",long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c){
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

        case 'c':
          //Create table
          break;

        case 'a':
          //Add [n] blocks
          break;

        case 'd':
          //Delete [n] blocks
          break;

        case 'r':
          puts("ala\n");
          //Alterly add and delete [n] blocks
          break;

        case 's':
          //Search match block with [n] ascii sum
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort();
          //printf("Illegal statement: %s");
		    	//exit(EXIT_FAILURE);	
      }
  }

    if(staticAlocationFlag)
      puts("Statis");
  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }

  createStaticTab(10,10);
	
	createStaticTab(blocksNumber,blockSize);

	printStaticTab();

	char*test=searchStaticTab(5);
	if(test!=NULL)
		printf("%s\n",test);
	else
		printf("BRAK");

	addBlockStaticTab(1,generate_data(blockSize));

	printStaticTab();

	test=searchStaticTab(5);
	if(test!=NULL)
		printf("%s\n",test);
	else
		printf("BRAK");

	deleteBlockStaticTab(1);

	printStaticTab();
	
	test=searchStaticTab(5);
	if(test!=NULL)
		printf("%s\n",test);
	else
		printf("BRAK");

	return EXIT_SUCCESS;
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
	strncpy(resultData,dataTemplate[rand()%DATATEMPLATESIZE],dataSize);
	return resultData; 
}
