#include "./../../errorlib/errorlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

double count_time(clock_t , clock_t );
void print_time(clock_t ,clock_t ,struct tms,struct tms);
void printHelp(char *);
int measureOperationTime(char);
char *generate_data(int);

void copy_file();
void generate_file();
void sort_file();

static int system_func_flag=0;
static int recors_number=0;
static int record_length=0;
static char* file_name;

int main (int argc, char **argv){
  srand(time(0));

  int c;
  while (true){    
        static struct option long_options[] ={
            /* These options set a flag. */
            {"sys",           no_argument, &system_func_flag, 1},
            {"lib",           no_argument, &system_func_flag, 0},
            /* These options don’t set a flag.
                We distinguish them by their indices. */
            {"generate",      no_argument,        0, 'g'},
            {"sort",          no_argument,        0, 's'},
            {"copy",          required_argument,  0, 'c'},
            {"recordnum",     required_argument,  0, 'n'},
            {"recordlen",     required_argument,  0, 'l'},
            {"file",          required_argument,  0, 'f'},
            {"help",          no_argument,        0, 'h'},
            {0, 0, 0, 0}
        };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "gsc:n:l:f:h",long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
        break;

    switch(c){
        case 0:
            //Flags
            break;
        case 'n':
            //Specify record number
            recors_number=atoi(optarg);
            break;
        case 'l':
            //Specify record length
            record_length=atoi(optarg);
            break;
        case 'f':
            file_name=optarg;
        case '?':
            /* getopt_long already printed an error message. */
            break;
        case 'g':
        case 's':
        case 'c':
            measureOperationTime(c);
            //TODO
            break;
        case 'h':
            printHelp(argv[0]);
            break;
        default:
            err_exit("illegal argument: "+c);

        }
    }
    if (optind < argc){
        char error_message[]="non-option ARGV-elements: ";
        while (optind < argc)
            strcat(error_message,argv[optind++]);
            strcat(error_message," ");
        strcat(error_message,"\n");
        err_exit(error_message);
    } 

return EXIT_SUCCESS;
}

int measureOperationTime(char c){

    struct tms start_time;
    clock_t start_real_time= times(&start_time);

    if(start_real_time == -1)
        err_sys_exit("times error");

    switch (c){
        case 'g':
            printf("---- Generate file  ----\n");
            generate_file();
        break;
        case 's':
            printf("---- Sort file  ----\n");
            sort_file();
        break;
        case 'c':
            printf("---- Copy file  ----\n");
            copy_file();
        break;
    }

    struct tms end_time;
    clock_t end_real_time = times(&end_time);

    if(end_real_time == -1)
        err_sys_exit("times error");

    print_time(start_real_time,end_real_time,start_time,end_time);
 
    return EXIT_SUCCESS;
}
void generate_file(){
    int file_descriptor;

    file_descriptor=creat(file_name,FILE_MODE);
    if(file_descriptor<0)
        err_sys_exit("Can't create file");

    for(int i=0;i<recors_number;i++){
        char *buf=generate_data(record_length);
        if(write(file_descriptor,buf,record_length)!=record_length)
            err_sys_exit("Can't write to file");
    }
}
void copy_file(){
 
}
void sort_file(){

}

double count_time(clock_t start, clock_t end) {
  return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void print_time(clock_t start_real_time,clock_t end_real_time,struct tms start_time,struct tms end_time){
      printf("   Real      User      System\n");
      printf("%lf   ", count_time(start_real_time, end_real_time));
      printf("%lf   ", count_time(start_time.tms_utime, end_time.tms_utime));
      printf("%lf ", count_time(start_time.tms_stime, end_time.tms_stime));
      printf("\n");
}

char *generate_data(int data_size) {
    if(data_size<1)
        err_exit("Record length is to short, can't generate data. ");

    char* data = calloc(data_size,sizeof(char));
    if(data==NULL)
        err_sys_exit("Can't generate data. ");

    for(int i=0;i<data_size-1;i++){
        data[i]=rand()%25+65;
    }
    data[data_size-1]='\n';
    return data;
}

void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s [-nlgscf]\n",progName);
  printf("--sys                       no argument\n");
  printf("--lib                       no argument\n");
  printf("--blocksNum       -n        required_argument\n");
  printf("--blockLen        -l        required_argument\n");
  printf("--generate        -g        no_argument\n");
  printf("--sort            -s        required_argument\n");
  printf("--copy            -c        required_argument\n");
  printf("--file            -f        required_argument\n");
}
