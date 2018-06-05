#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>
#include <pthread.h>
#include <unistd.h>

int W,H,M,c;
unsigned char** I;
unsigned char** J;
float **K;

typedef struct Partition{
    int from,until;
}Partition;

int max(int x,int y){
    return x>y ? x : y; 
}

int min(int x,int y){
    return x>y ? y : x; 
}

char s(int x,int y){
    int s=0;
    for(int i=0;i<c;i++){
        for(int j=0;j<c;j++){
            s+= (int) roundf(I[min(max(0,x-((c+1)/2)+i),H-1)][min(max(0,y-((c+1)/2)+j),W-1)] * K[i][j]);
        }
    }
    return s;
}


void read_picture(char *file_path) {
    FILE *file = fopen(file_path, "r");
    if(file==NULL){
        perror(file_path);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "P2\n");
    fscanf(file, "%d", &W);
    fscanf(file, "%d", &H);
    fscanf(file, "%d", &M);

    I = malloc(H * sizeof(unsigned char *));
    for (int i = 0; i < H; i++)
        I[i] = malloc(W * sizeof(unsigned char));

    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            fscanf(file, "%hhu",&I[i][j]);
    fclose(file);
}

void read_filter(char *file_path) {
    FILE *file = fopen(file_path, "r");
    if(file==NULL){
        perror(file_path);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &c);

    K = malloc(c * sizeof(float *));
    for (int i = 0; i < c; i++)
        K[i] = malloc(c * sizeof(float));

    for (int i = 0; i < c; i++)
        for (int j = 0; j < c; j++)
            fscanf(file, "%f",&K[i][j]);
    fclose(file);
}

void gen_picture(int from,int until){
    for (int i = from; i < until; i++)
        for (int j = 0; j < W; j++)
            J[i][j]=s(i,j);
}

void write_picture(char* file_path){
    FILE *file = fopen(file_path, "w");
    if(file==NULL){
        perror(file_path);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "P2\n");
    fprintf(file, "%d ", W);
    fprintf(file, "%d ", H);
    fprintf(file, "%d ", M);

    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            fprintf(file, "%hhu ",J[i][j]);
    fclose(file);
}
void prepare_gen_picture(){
    J = malloc(H * sizeof(unsigned char *));
    for (int i = 0; i < H; i++)
        J[i] = malloc(W * sizeof(unsigned char));
}



void *routine(void *args) {
    
    Partition* partition=(Partition*) args;
    gen_picture(partition->from,partition->until);
    
    return NULL;
}
void printHelp(char *progName){
  printf("\nUsage help\n");
  printf("%s <threads_num> <input_file> <filter_file> <output_file> \n",progName);
}

double countTime(clock_t start, clock_t end) {
  return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void gen_all_picture(int threads_num){
    struct tms startTime;
    clock_t startRealTime = times(&startTime);

    pthread_t *threads = malloc(threads_num * sizeof(pthread_t));
    for (int i = 0; i < threads_num; i++) {
        Partition *partition=malloc(sizeof(Partition));
        partition->from = H * i / threads_num;
        partition->until = H * (i + 1) / threads_num;
        pthread_create(&threads[i], NULL, routine, partition);
    }
    for (int i = 0; i < threads_num; i++) {
        void *ret;
        pthread_join(threads[i], &ret);
    }

    struct tms endTime;
    clock_t endRealTime = endRealTime = times(&endTime);

    printf("%d threads. Time:\n",threads_num);
    printf("   Real      User      System\n");
    printf("%lf   ", countTime(startRealTime, endRealTime));
    printf("%lf   ", countTime(startTime.tms_utime, endTime.tms_utime));
    printf("%lf ", countTime(startTime.tms_stime, endTime.tms_stime));
    printf("\n");
}

int main (int argc, char **argv){

    /*
    Args:
        liczbę wątków,
        nazwę pliku z wejściowym obrazem,
        nazwę pliku z definicją filtru,
        nazwę pliku wynikowego.
    */
    if (argc!=5) {
        printf("Ilegal arguments number");
        printHelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    int threads_num=atoi(argv[1]);

    read_picture(argv[2]);
    read_filter(argv[3]);
    prepare_gen_picture();
    gen_all_picture(threads_num);
    write_picture(argv[4]);


    return EXIT_SUCCESS;
}


