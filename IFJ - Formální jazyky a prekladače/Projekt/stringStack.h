/*
 * Created on Sat Oct 26 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file stringStack.h
 * @author Jakub Kočalka
 * @brief 
 */

#ifndef _STRING_STACK_H_
#define _STRING_STACK_H_

#define STACK_CHUNK_SIZE 16

#include <stdlib.h>
#include <stdbool.h>

typedef char* tStringStackElem;

struct StringStack;
typedef struct StringStack* tStringStack;

/*public methods prototypes*/
void stringStackInit ( tStringStack* s );

/**
 * @brief Is the Stack Empty?
 * 
 * @param s 
 * @return true if the stack is empty
 * @return false 
 */
bool stringStackEmpty ( const tStringStack s );
bool stringStackFull ( const tStringStack s );

/**
 * @brief Returns the top element of stack
 * 
 * @param s 
 * @return tStringStackElem 
 */
tStringStackElem stringStackTop ( const tStringStack s);

/**
 * @brief Removes the top element of stack and returns it
 * 
 * @param s 
 * @return tStringStackElem 
 */
tStringStackElem stringStackPop ( tStringStack s );
void stringStackPush ( tStringStack s, tStringStackElem c );
void stringStackCopyAndPush(tStringStack s, tStringStackElem c);

#endif
