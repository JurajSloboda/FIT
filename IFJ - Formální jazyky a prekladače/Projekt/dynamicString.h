/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file dynamicString.h
 * @author Jakub Kočalka
 * @brief 
 */


#ifndef __DYNAMIC_STRING_H__
#define __DYNAMIC_STRING_H__

#include "error.h"

#define STRING_CHUNK_SIZE 16

struct dynamicString;
typedef struct dynamicString* tDynamicString;

/*public methods prototypes*/

/**
 * @brief Allocates and initializes an empty string of given size. The string will be filled with zeroes.
 * 
 * @param size how many symbols should fit into the new string
 * @return tDynamicString pointer to the new string
 */
tDynamicString initDynamicString(size_t size);

/**
 * @brief Deletes a dynamic string
 * 
 * @param str_ptr pointer to the string to be deleted
 */
void deleteDynamicString(tDynamicString *strPtr);

/**
 * @brief Resizes the given string
 * 
 * @param strPtr Pointer to the string to be resized
 * @param newSize New number of symbols that should fit into the string
 */
void resizeDynamicString(tDynamicString *strPtr, size_t newSize);

/**
 * @brief Appends symbol to string.
 * 
 * @param strPtr pointer to the string
 * @param c symbol to be appended
 */
void appendToString(tDynamicString *strPtr, char c);

void setDynamicString(tDynamicString *strPtr, char *c);

/**
 * @brief Get the string itself
 * 
 * @param strPtr Pointer to the dynamic string structure
 * @return char* Pointer to the string proper
 */
char* getStrData(tDynamicString strPtr);

/**
 * @brief Get the Str Len object
 * 
 * @param strPtr 
 * @return size_t 
 */
size_t getStrLen(tDynamicString strPtr);

/**
 * @brief Get the Str Size object
 * 
 * @param strPtr 
 * @return size_t 
 */
size_t getStrSize(tDynamicString strPtr);

/**
 * @brief Copies a string, resizing the destination if necesarry.
 * 
 * @param dest 
 * @param origin 
 */
void copyDynamicString(tDynamicString *dest, tDynamicString origin);

void clearDynamicString(tDynamicString *strPtr);


/**
 * @brief Deletes a string and replaces it with a new one
 * 
 * @param oldPtr Pointer to the string to be replaced
 * @param newPtr Pointer to the new string. It must already be initialized
 */
void replaceDynamicString(tDynamicString *oldPtr, tDynamicString newPtr);

#endif //__DYNAMIC_STRING_H__