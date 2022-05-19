/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file error.c
 * @author Jakub Kočalka, Kristina Hostacna
 * @brief 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "error.h"

#define MAX_ERROR_MSG_LEN 128

void warning_msg(const char *fmt, ...){
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, arg);
    va_end(arg);
}


void error_exit(const char *fmt, ...){
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    exit(1);
}


void errMsg(char *str){
    fprintf(stderr, "ERROR: %s\n" ,str);
}

void ourError(ERR_CODE errCode, char* optMsg){
    bool isFatal = false;
    char msg[MAX_ERROR_MSG_LEN];
    switch (errCode)
    {
    case ERR_INTERNAL:
        strcpy(msg, "Memory Allocation Failed. INFO:");
        break;
    
    default:
        break;
    }

    char* finalMsg = malloc(strlen(msg)+strlen(optMsg));
    strcat(finalMsg, msg);
    strcat(finalMsg, optMsg);
    errMsg(finalMsg);
    free(finalMsg);

    if(isFatal){
        exit(errCode);
    }
}
