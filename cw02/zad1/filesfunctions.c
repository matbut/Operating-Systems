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

void generate_file();
void copy_sys_file();
void sort_sys_file();
void copy_lib_file();
void sort_lib_file();

static int system_func_flag=0;
static int recors_number=0;
static int record_length=0;
static char* file_name;
static char* copy_file_name;

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
            generate_file();
        break;
        case 's':
        case 'c':
            copy_file_name=optarg;
            measureOperationTime(c);
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
        case 's':
            if(system_func_flag){
                printf("---- Sort file with system functions----\n");
                sort_sys_file();
            }else{
                printf("---- Sort file with library functions----\n");
                sort_lib_file();
            }
        break;
        case 'c':
            if(system_func_flag){
                printf("---- Copy file with system functions----\n");
                copy_sys_file();
            }else{
                printf("---- Copy file with library functions----\n");
                copy_lib_file();
            }
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
        free(buf);
    }
    if(close(file_descriptor)!=0)
        err_sys_exit("Can't close file");
}

void copy_sys_file(){
    int file_source;
    int file_copy;

    file_source=open(file_name,O_RDONLY);
    if(file_source<0)
        err_sys_exit("Can't open file ");

    file_copy=creat(copy_file_name,FILE_MODE);
    if(file_copy<0)
        err_sys_exit("Can't create file ");

    char* buf=calloc(record_length,sizeof(char));
    int read_num=read(file_source,buf,record_length);
    while(read_num>0){
        if(write(file_copy,buf,read_num)!=read_num)
            err_sys_exit("Can't write to file");
        read_num=read(file_source,buf,record_length);
    }

    if(read_num<0)
        err_sys_exit("Can't read file");

    free(buf);
    if(close(file_source)!=0)
        err_sys_exit("Can't close source file");
    if(close(file_copy)!=0)
        err_sys_exit("Can't close copy file");
}

void sort_sys_file(){
    int file_descriptor=open(file_name,O_RDWR);
    int offset=sizeof(char)*record_length;

    char* insert_buf=calloc(record_length,sizeof(char));
    char* swap_buf=calloc(record_length,sizeof(char));

    if(file_descriptor<0)
        err_sys_exit("Can't open file ");    

    for(int i=1;i<recors_number;i++){

        if(lseek(file_descriptor,i*offset,SEEK_SET)==-1)
            err_sys_exit("Can't seek");
        
        if(read(file_descriptor,insert_buf,record_length)!=record_length)
            err_sys_exit("Can't read file");

        int j;
        for(j=i-1;j>=0;j--){

            if(lseek(file_descriptor,j*offset,SEEK_SET)==-1)
                err_sys_exit("Can't seek"); 

            if(read(file_descriptor,swap_buf,record_length)!=record_length)
                err_sys_exit("Can't read file");

            if(insert_buf[0] >= swap_buf[0]) break;

            if(write(file_descriptor,swap_buf,record_length)!=record_length)
                err_sys_exit("Can't write to file"); 
                 
        }
        if(lseek(file_descriptor,(j+1)*offset,SEEK_SET)==-1)
            err_sys_exit("Can't seek"); 

        if(write(file_descriptor,insert_buf,record_length)!=record_length)
            err_sys_exit("Can't write to file"); 

    }

    free(insert_buf);
    free(swap_buf);
    if(close(file_descriptor)!=0)
        err_sys_exit("Can't close file");
}

void copy_lib_file(){
    FILE *file_source = fopen(file_name, "r");
    if(file_source==NULL)
        err_sys_exit("Can't open file "); 

    FILE *file_copy = fopen(copy_file_name, "w+");
    if(file_copy==NULL)
        err_sys_exit("Can't open file "); 

    char* buf=calloc(record_length,sizeof(char));

    int read_num=fread(buf, sizeof(char), (size_t) record_length, file_source);
    while(read_num>0){
        if(fwrite(buf, sizeof(char), (size_t) read_num, file_copy) != read_num) 
            err_sys_exit("Can't write to file");
        read_num=fread(buf, sizeof(char), (size_t) record_length, file_source);    
    }

    if(read_num<0)
        err_sys_exit("Can't read file");
    
    free(buf);
    if(fclose(file_source)!=0)
        err_sys_exit("Can't close source file");
    if(fclose(file_copy)!=0)
        err_sys_exit("Can't close copy file");
}

void sort_lib_file(){
    FILE *file_source = fopen(file_name, "rw+");
    if(file_source==NULL)
        err_sys_exit("Can't open file "); 
    int offset=sizeof(char)*record_length;

    char* insert_buf=calloc(record_length,sizeof(char));
    char* swap_buf=calloc(record_length,sizeof(char));

    if(file_source==NULL)
        err_sys_exit("Can't open file ");    

    for(int i=1;i<recors_number;i++){

        if(fseek(file_source,i*offset,0)!=0)
            err_sys_exit("Can't seek");
        
        if(fread(insert_buf,sizeof(char),(size_t) record_length,file_source)!=record_length)
            err_sys_exit("Can't read file");

        int j;
        for(j=i-1;j>=0;j--){

            if(fseek(file_source,j*offset,0)!=0)
                err_sys_exit("Can't seek"); 
                
            if(fread(swap_buf,sizeof(char),(size_t) record_length,file_source)!=record_length)
                err_sys_exit("Can't read file");

            if(insert_buf[0] >= swap_buf[0]) break;

            if(fwrite(swap_buf, sizeof(char), (size_t) record_length, file_source) != record_length)
                err_sys_exit("Can't write swap buffer to file"); 
                 
        }
        if(fseek(file_source,(j+1)*offset,0)!=0)
            err_sys_exit("Can't seek"); 

        if(fwrite(insert_buf, sizeof(char), (size_t) record_length, file_source) != record_length)
            err_sys_exit("Can't write insert buffer to file");

    }

    free(insert_buf);
    free(swap_buf);
    if(fclose(file_source)!=0)
        err_sys_exit("Can't close file");
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
