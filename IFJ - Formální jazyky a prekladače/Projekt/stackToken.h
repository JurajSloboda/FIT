/*
 * Created on Sat Oct 26 2019
 *
 * Copyright (c) 2019 Jakub Kočalka, Kristina Hostacna
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file stackToken.h
 * @author Jakub Kočalka, Kristina Hostacna
 * @brief 
 */

#ifndef _STACKTOKEN_H_
#define _STACKTOKEN_H_

#define STACK_CHUNK_SIZE 16

#include <stdlib.h>
#include <stdbool.h>
#include "token.h"

typedef tToken tStackTElem;

struct TStack;
typedef struct TStack* tTStack;

/*public methods prototypes*/
void stackTInit ( tTStack* s );

/**
 * @brief Is the Stack Empty?
 * 
 * @param s 
 * @return true if the stack is empty
 * @return false 
 */
bool stackTEmpty ( const tTStack s );
bool stackTFull ( const tTStack s );

/**
 * @brief Returns the top element of stack
 * 
 * @param s 
 * @return tStackTElem 
 */
tStackTElem stackTTop ( const tTStack s);

/**
 * @brief Returns the nth element of stack
 * 
 * @param s 
 * @param n 
 * @return *tStackTElem if possible
 * @return null 
 */
tStackTElem stackGet ( const tTStack s, unsigned int n);

/**
 * @brief Removes the top element of stack and returns it
 * 
 * @param s 
 * @return tStackTElem 
 */
tStackTElem stackTPop ( tTStack s );
void stackTPush ( tTStack s, tStackTElem c );

#endif
