/*
Created by Mateusz Buta 17/03/2018
*/

#ifndef ERRORLIB_H
#define ERRORLIB_H

void err_sys_ret(const char *, ...);
void err_sys_exit(const char *, ...);
void err_ret(const char *, ...);
void err_exit(const char *args, ...);


#endif //ERRORLIB_H
