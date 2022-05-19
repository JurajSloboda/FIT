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


#include "stackToken.h"
#include "error.h"

struct TStack
{
    tStackTElem* elem;
    size_t top;
    size_t size;
};

void stackTInit ( tTStack* s ){
    *s = malloc(sizeof(struct TStack));
    if(*s == NULL){
        ourError(ERR_INTERNAL, "error allocating stack structure");
    }

    (*s)->elem = malloc(sizeof(tStackTElem) * STACK_CHUNK_SIZE);
    if((*s)->elem == NULL){
        ourError(ERR_INTERNAL, "error allocating stack structure");
    }

    (*s)->top = -1;
    (*s)->size = STACK_CHUNK_SIZE;
}

bool stackTEmpty ( const tTStack s ){
    return (s->top+1 == 0);
}

bool stackTFull ( const tTStack s ){
    return (s->top+1 == s->size);
}

//BUG: touches protected memory if stack empty
tStackTElem stackTTop ( const tTStack s){
        return s->elem[s->top];
}

tStackTElem stackTPop ( tTStack s ){
    tStackTElem result = stackTTop(s);
    s->top = s->top - 1;
    return result;
}

tStackTElem stackGet ( const tTStack s, unsigned int n){       ///added
    if (s->size >= n) {
        return s->elem[s->top-n+1];
    }
    return NULL;
}                       

void stackTResize(tTStack s, size_t newSize){
    s = realloc(s, newSize);
    if(s == NULL){
        ourError(ERR_INTERNAL, "error resizing stack structure");
    }
}

void stackTPush ( tTStack s, tStackTElem c ){
    if(stackTFull(s)){
        stackTResize(s, s->size+STACK_CHUNK_SIZE);
    }

    s->top++;
    s->elem[s->top] = c;
}