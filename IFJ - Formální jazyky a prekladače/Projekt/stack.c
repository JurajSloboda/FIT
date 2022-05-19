/*
 * Created on Sat Oct 26 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file stack.c
 * @author Jakub Kočalka
 * @brief 
 */


#include "stack.h"
#include "error.h"

struct Stack
{
    tStackElem* elem;
    size_t top;
    size_t size;
};

void stackInit ( tStack* s ){
    *s = malloc(sizeof(struct Stack));
    if(*s == NULL){
        ourError(ERR_INTERNAL, "error allocating stack structure");
    }

    (*s)->elem = malloc(sizeof(tStackElem) * STACK_CHUNK_SIZE);
    if((*s)->elem == NULL){
        ourError(ERR_INTERNAL, "error allocating stack structure");
    }

    (*s)->top = -1;
    (*s)->size = STACK_CHUNK_SIZE;
}

bool stackEmpty ( const tStack s ){
    return (s->top+1 == 0);
}

bool stackFull ( const tStack s ){
    return (s->top+1 == s->size);
}

//BUG: touches protected memory if stack empty
tStackElem stackTop ( const tStack s){
        return s->elem[s->top];
}

tStackElem stackPop ( tStack s ){
    tStackElem result = stackTop(s);
    s->top = s->top - 1;
    return result;
}

void stackResize(tStack s, size_t newSize){
    s = realloc(s, newSize);
    if(s == NULL){
        ourError(ERR_INTERNAL, "error resizing stack structure");
    }
}

void stackPush ( tStack s, tStackElem c ){
    if(stackFull(s)){
        stackResize(s, s->size+STACK_CHUNK_SIZE);
    }

    s->top++;
    s->elem[s->top] = c;
}