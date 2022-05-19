/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file error.h
 * @author Jakub Kočalka, Kristina Hostacna
 * @brief 
 */


#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "error.h"

enum error_code{
    ERR_LEXICAL = 1,
    ERR_SYNTACTIC = 2,
    ERR_SEMANTIC_NONDEFINED = 3,
    ERR_SEMANTIC_INCOMPATIBLE_OPERATORS = 4,
    ERR_SEMANTIC_WRONG_PARAM_NUM = 5,
    ERR_SEMANTIC_OTHER = 6,
    ERR_ZERO_DIV = 7,

    ERR_INTERNAL = 99,
};

typedef enum error_code ERR_CODE;

	void warning_msg(const char *fmt, ...);

	void error_exit(const char *fmt, ...);

	/**
	 * @brief Prints message to stderr
	 * 
	 * @param str message to be printed
	 */
	void errMsg(char* str);

	void ourError(ERR_CODE errCode, char* optMsg);

#endif //__ERROR_H__
