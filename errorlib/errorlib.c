

#include <stdarg.h> //listy zmiennych argumentów
#include "errorlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 4096

static void err_serv(int, const char*, va_list);

//Errors related to the system call (So there is standard error message related to error number)

//Non-critical (prints message and return)

void err_sys_ret(const char *args, ...){
    va_list ap; 
    va_start(ap,args);
    err_serv(1,args,ap);
    return;
}

//Critical (prints message and exit)

void err_sys_exit(const char *args, ...){
    va_list ap; 
    va_start(ap,args);
    err_serv(1,args,ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

//Errors not related to the system call (without standard error number)

//Non-critical (prints message and return)

void err_ret(const char *args, ...){
    va_list ap; 
    va_start(ap,args);
    err_serv(0,args,ap);
    va_end(ap);
    return;
}

//Critical (prints message and exit)

void err_exit(const char *args, ...){
    va_list ap; 
    va_start(ap,args);
    err_serv(0,args,ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}



static void err_serv(int errno_flag, const char* args, va_list ap){
    int errno_save = errno; 
    char buf[MAXLINE];

    vsprintf(buf,args,ap);

    if(errno_flag)
        sprintf(buf+strlen(buf),": %s",strerror(errno_save));
    strcat(buf,"\n");
    fflush(stdout);
    fputs(buf,stderr);
    fflush(NULL);
    return;
}
