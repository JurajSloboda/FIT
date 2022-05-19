/*
 * Created on Sat Oct 26 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file stack.h
 * @author Jakub Kočalka
 * @brief 
 */

#ifndef _STACK_H_
#define _STACK_H_

#define STACK_CHUNK_SIZE 16

#include <stdlib.h>
#include <stdbool.h>

typedef int tStackElem;

struct Stack;
typedef struct Stack* tStack;

/*public methods prototypes*/
void stackInit ( tStack* s );

/**
 * @brief Is the Stack Empty?
 * 
 * @param s 
 * @return true if the stack is empty
 * @return false 
 */
bool stackEmpty ( const tStack s );
bool stackFull ( const tStack s );

/**
 * @brief Returns the top element of stack
 * 
 * @param s 
 * @return tStackElem 
 */
tStackElem stackTop ( const tStack s);

/**
 * @brief Removes the top element of stack and returns it
 * 
 * @param s 
 * @return tStackElem 
 */
tStackElem stackPop ( tStack s );
void stackPush ( tStack s, tStackElem c );

#endif
