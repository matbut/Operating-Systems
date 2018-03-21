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

const char data_format[] = "%Y-%m-%d %H:%M:%S";
int display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf);

static int system_func_flag;

void printHelp(char *);

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
    char *operant = argv[3];
    char *usr_date = argv[4];

    struct tm *tm = malloc(sizeof(struct tm));

    int const buff_size = PATH_MAX;
    char buffer[buff_size];

    strptime(usr_date, data_format, tm);
    strftime(buffer, buff_size, data_format, tm);
    time_t date = mktime(tm);



    if (nftw(realpath(path, NULL), display_info, 20, FTW_PHYS) == -1) {
        err_sys_exit("nftw");
    }


    return EXIT_SUCCESS;
}

int display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf){


    char buff[20]; 
    struct tm timeinfo;

    (void) localtime_r(&sb->st_mtime, &timeinfo);

    strftime(buff, 20, data_format, &timeinfo); 



    if(tflag == FTW_D || tflag == FTW_F)
    printf("%-80s %-7jd %s\n",
        fpath,
        (intmax_t) sb->st_size,
        buff);
    return 0;           /* To tell nftw() to continue */
}

void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s [--lib or --sys] <path> <operator> <date %s format> \n",progName,data_format);
}
