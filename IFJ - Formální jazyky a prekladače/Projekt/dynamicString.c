/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file dynamicString.c
 * @author Jakub Kočalka
 * @brief 
 */

#include <stdlib.h>
#include <string.h>
#include "dynamicString.h"

struct dynamicString{
    char *data;
    size_t size; //Size of the allocated structure
    size_t len; //Lenght of the string. Must not be greater than size-1
};

tDynamicString initDynamicString(size_t size){
    tDynamicString newString = (tDynamicString) malloc(sizeof(struct dynamicString));
    if(newString == NULL){
        ourError(ERR_INTERNAL, "error while allocationg dynamic string structure.");
    }

    size_t numOfChunks = (size+1) / (int) STRING_CHUNK_SIZE + 1; //calculate how many chunks we need
    newString->data = (char*)calloc(numOfChunks * STRING_CHUNK_SIZE, sizeof(char)); //allocates and clears the data
    if(newString->data == NULL){
        ourError(ERR_INTERNAL, "error while allocationg dynamic string data.");
    }
    newString->size = numOfChunks*STRING_CHUNK_SIZE;
    newString->len = 0;

    return newString;
}

void deleteDynamicString(tDynamicString *strPtr){
    free((*strPtr)->data);
    free(*strPtr);
}

void resizeDynamicString(tDynamicString *strPtr, size_t newSize){
    //allocate new string
    //TODO: Figure out why the init function messes with the old structure
    //size_t strLen = strPtr->len; //this is weird, but after the init structure, the old len becomes zero
    tDynamicString newString = initDynamicString(newSize);

    //Copy data
    strcpy(newString->data, (*strPtr)->data);
    //newString->size = newSize; //will be done by init
    newString->len = (*strPtr)->len;

    //delete the old string
    deleteDynamicString(strPtr);

    //assing the new string to the old pointer
    *strPtr = newString;
}

void appendToString(tDynamicString *strPtr, char c){
    if((*strPtr)->len+1 >= (*strPtr)->size){
        resizeDynamicString(strPtr, (*strPtr)->size+STRING_CHUNK_SIZE);
    }
    (*strPtr)->data[(*strPtr)->len++] = c;
}

void setDynamicString(tDynamicString *strPtr, char* c){
    clearDynamicString(strPtr);
    size_t strLen = strlen(c);
    for (size_t i = 0; i < strLen; i++)
    {
        appendToString(strPtr, c[i]);
    }
}

char* getStrData(tDynamicString strPtr){
    return strPtr->data;
}

size_t getStrLen(tDynamicString strPtr){
    return strPtr->len;
}

size_t getStrSize(tDynamicString strPtr){
    return strPtr->size;
}

void copyDynamicString(tDynamicString *dest, tDynamicString origin){
    //resize the string if necessary
    if((*dest)->size <= origin->len){
        resizeDynamicString(dest, origin->len);
    }

    strcpy((*dest)->data, origin->data);
    (*dest)->len = origin->len;
    
}

void clearDynamicString(tDynamicString *strPtr){
    size_t tmpSize = (*strPtr)->size;
    deleteDynamicString(strPtr);
    *strPtr = initDynamicString(tmpSize);
}

void replaceDynamicString(tDynamicString *oldPtr, tDynamicString newPtr){
    deleteDynamicString(oldPtr);
    *oldPtr = newPtr;
}