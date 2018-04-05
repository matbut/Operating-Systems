#include <stdlib.h>
#include <stdio.h>

#define ARRAY_SIZE 10000
#define STRING_LEN 10000

int main() {
    char**tab = calloc(ARRAY_SIZE,sizeof(char*));
    for (int i = 0; i< ARRAY_SIZE; i++) {
        tab[i] = calloc(STRING_LEN,sizeof(char));
        for(int j=0;j<STRING_LEN;j++)
            tab[i][j]='M';
    }
    return 0;
}
