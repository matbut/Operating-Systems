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
#include <dirent.h>
#include <wait.h>

const char data_format[] = "%Y-%m-%d";
static char operand;
static struct tm *user_date;

void print_file_info(const char *, const struct stat *);
void printHelp(char *);
char* file_access_right(const struct stat *);
int date_compare(struct tm*);
int display_dir_sys(char * );

int main (int argc, char **argv){

    if (argc!=4) {
        err_ret("Ilegal arguments number");
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }

    char *path = argv[1];
    operand = argv[2][0];

    user_date = malloc(sizeof(struct tm));

    if(strptime(argv[3], data_format, user_date)==NULL)
        err_exit("Ilegal data format in argument: %s",argv[3]);
   
  
    display_dir_sys(realpath(path, NULL));

    return EXIT_SUCCESS;
}

int display_dir_sys(char * path){

    DIR *dp;
    struct dirent *dirp;
    struct stat file_stat;
    char new_path[PATH_MAX];

    dp=opendir(path);
    if(dp==NULL)
        err_sys_exit("Can't open %s",path);

    dirp=readdir(dp);

    while (dirp != NULL) {
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, dirp->d_name);

        lstat(new_path, &file_stat);
        
        if (S_ISREG(file_stat.st_mode)) 
            print_file_info(new_path,&file_stat);
        else if (S_ISDIR(file_stat.st_mode)){
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
                dirp = readdir(dp);
                continue;
            }
            pid_t pid=fork();
            //printf("new process %d\n",pid);
            if(pid<0)
                err_sys_exit("Fork error");
            else if (pid>0){
                display_dir_sys(new_path);
                //printf("end process %d\n",pid);
                exit(0);
            }
        }    
        dirp = readdir(dp);
    }

    if(closedir(dp)!=0)
        err_sys_exit("Can't close dir");

    return 0;
}

void print_file_info(const char *path, const struct stat *file_stat){
    char buff[20]; 

    struct tm timeinfo;
    if(localtime_r(&file_stat->st_mtime, &timeinfo)==NULL)
        err_sys_exit("Can't convert file stat to tm struct");
    strftime(buff, 20, data_format, &timeinfo); 

    if(date_compare(&timeinfo)){
        printf("%10s %-7jd %s  %s\n",
            file_access_right(file_stat),
            (intmax_t) file_stat->st_size,
            buff,
            path);
    } 
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
            err_exit("Not suported %c operation.",operand);
        
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
  printf("%s <path> <operator> <date %s format> \n",progName,data_format);
}
