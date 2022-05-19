/*
 * Created on Sat Oct 26 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file stringStack.c
 * @author Jakub Kočalka
 * @brief 
 */


#include "stringStack.h"
#include "error.h"

struct StringStack
{
    tStringStackElem* elem;
    size_t top;
    size_t size;
};

void stringStackInit ( tStringStack* s ){
    *s = malloc(sizeof(struct StringStack));
    if(*s == NULL){
        ourError(ERR_INTERNAL, "error allocating stringStack structure");
    }

    (*s)->elem = malloc(sizeof(tStringStackElem) * STACK_CHUNK_SIZE);
    if((*s)->elem == NULL){
        ourError(ERR_INTERNAL, "error allocating stringStack structure");
    }

    (*s)->top = -1;
    (*s)->size = STACK_CHUNK_SIZE;
}

bool stringStackEmpty ( const tStringStack s ){
    return (s->top+1 == 0);
}

bool stringStackFull ( const tStringStack s ){
    return (s->top+1 == s->size);
}

//BUG: touches protected memory if stack empty
tStringStackElem stringStackTop ( const tStringStack s){
        return s->elem[s->top];
}

tStringStackElem stringStackPop ( tStringStack s ){
    tStringStackElem result = malloc(strlen(stringStackTop(s)));
    strcpy(result, stringStackTop(s));
    //free(stringStackTop(s));
    s->top = s->top - 1;
    return result;
}

void stringStackResize(tStringStack s, size_t newSize){
    s = realloc(s, newSize);
    if(s == NULL){
        ourError(ERR_INTERNAL, "error resizing stringStack structure");
    }
}

void stringStackPush ( tStringStack s, tStringStackElem c ){
    if(stringStackFull(s)){
        stringStackResize(s, s->size+STACK_CHUNK_SIZE);
    }

    s->top++;
    s->elem[s->top] = c;
}

void stringStackCopyAndPush(tStringStack s, tStringStackElem c){
    char* newStr = malloc(strlen(c));
    strcpy(newStr, c);
    stringStackPush(s, newStr);
}

void stringStackDelete(tStringStack s){
    while(!stringStackEmpty(s)){
        stringStackPop(s);
    }
    free(s);
}