#define _XOPEN_SOURCE 500
#include <ftw.h>
#include "./../../errorlib/errorlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>

const char data_format[] = "%Y-%m-%d";
int display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf);

static int system_func_flag;
static char operand;
static struct tm *user_date;

void print_file_info(const char *, const struct stat *);
void printHelp(char *);
char* file_access_right(const struct stat *);
int date_compare(struct tm*);

int main (int argc, char **argv){

    if (argc!=5) {
        err_ret("Ilegal arguments number");
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }


    if(strcmp(argv[1],"--lib"))
        system_func_flag=1;
    else if(strcmp(argv[1],"--sys"))
        system_func_flag=0;
    else
        err_ret("Ilegal first argument");

    char *path = argv[2];
    operand = argv[3][0];

    user_date = malloc(sizeof(struct tm));

    strptime(argv[4], data_format, user_date);
   
    if (nftw(realpath(path, NULL), display_info, 20, FTW_PHYS) == -1) {
        err_sys_exit("nftw");
    }


    return EXIT_SUCCESS;
}

int display_info(const char *fpath, const struct stat *file_stat, int tflag, struct FTW *ftwbuf){

    struct tm timeinfo;
    if(localtime_r(&file_stat->st_mtime,&timeinfo)==NULL)
        err_sys_exit("Can't convert file stat to tm struct");

    if((tflag == FTW_D || tflag == FTW_F) && date_compare(&timeinfo))
        print_file_info(fpath,file_stat);

    return 0;           /* To tell nftw() to continue */
}

void print_file_info(const char *path, const struct stat *file_stat){
    char buff[20]; 

    struct tm timeinfo;
    if(localtime_r(&file_stat->st_mtime, &timeinfo)==NULL)
        err_sys_exit("Can't convert file stat to tm struct");
    strftime(buff, 20, data_format, &timeinfo); 

    printf("%10s %-7jd %s  %s\n",
        file_access_right(file_stat),
        (intmax_t) file_stat->st_size,
        buff,
        path);
}

int date_compare(struct tm *date) {

    switch (operand){
        case '=':
            return user_date->tm_year == date->tm_year
                && user_date->tm_mon == date->tm_mon 
                && user_date->tm_mday == date->tm_mday;
        break;
        case '>':
            return user_date->tm_year < date->tm_year ||
                (user_date->tm_year == date->tm_year && user_date->tm_mon < date->tm_mon) ||
                (user_date->tm_year == date->tm_year && user_date->tm_mon == date->tm_mon && user_date->tm_mday < date->tm_mday);
        break;
        case '<':
            return user_date->tm_year > date->tm_year ||
                (user_date->tm_year == date->tm_year && user_date->tm_mon > date->tm_mon) ||
                (user_date->tm_year == date->tm_year && user_date->tm_mon == date->tm_mon && user_date->tm_mday > date->tm_mday);
               break;
        default:
            err_exit("Not suported operation.");
        
    }
    
    return 1;
}

char* file_access_right(const struct stat *file_stat){
    char* access_rights=calloc(10,sizeof(char));
    access_rights[0]=file_stat->st_mode & S_IRUSR ? 'r' : '-';
    access_rights[1]=file_stat->st_mode & S_IWUSR ? 'w' : '-';
    access_rights[2]=file_stat->st_mode & S_IXUSR ? 'x' : '-';
    access_rights[3]=file_stat->st_mode & S_IRGRP ? 'r' : '-';
    access_rights[4]=file_stat->st_mode & S_IWGRP ? 'w' : '-';
    access_rights[5]=file_stat->st_mode & S_IXGRP ? 'x' : '-';
    access_rights[6]=file_stat->st_mode & S_IROTH ? 'r' : '-';
    access_rights[7]=file_stat->st_mode & S_IWOTH ? 'w' : '-';
    access_rights[8]=file_stat->st_mode & S_IXOTH ? 'x' : '-';
    access_rights[9]='\0';

    return access_rights;
}

void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s [--lib or --sys] <path> <operator> <date %s format> \n",progName,data_format);
}
