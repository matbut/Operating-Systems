#include <stdlib.h>
#include <stdio.h>

int ackermann(int m,int n) {
    if (m==0) 
        return n+1;
    if (m>0 && n==0) 
        return ackermann(m-1, 1);
    if (m>0 && n>0) 
        return ackermann(m-1, ackermann(m, n-1)); 
    return 0;
}

int main() {
    ackermann(5,5);
    perror("Ackermann");
    return 0;
}

