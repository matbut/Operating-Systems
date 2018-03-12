#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <../zad1/blocktable.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define DATATEMPLATESIZE 20

char *generate_data(int dataSize);

int main(int argsNumber, char **args) {
	srand(time(0)); 
	/*
		argv[0]-prog name
		argv[1]-blocks number
		argv[2]-block size
		argv[3]-alocation 
		argv[4]-operations
	*/

	if(argsNumber < 5){
		printf("Illegal arguments number\n");
		return 1;
	}

    int blocksNumber = (int) strtol(args[1], NULL, 10);
    int blockSize = (int) strtol(args[2], NULL, 10);
		
	bool isStaticAlocation;
    if (strcmp(args[3], "static"))
		isStaticAlocation=true;
	else if (strcmp(args[3], "dynamic"))
        isStaticAlocation = false;
	else{
        printf("Illegal allocation type\n");
        return 1;
    }


	
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

	return 0;
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

	
