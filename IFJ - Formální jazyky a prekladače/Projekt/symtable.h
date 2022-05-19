/*
 * Created on Sun Oct 27 2019
 *
 * Copyright (c) 2019 Kristina Hostacna, Andrej Ryska
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file symtable.h
 * @author Kristina Hostacna, Andrej Ryska
 * @brief 
 */

#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <error.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "dynamicString.h"

#define HTSIZE 101

/**
* @brief Function for hash calculation.
*
* @param key Key for calculating hash
* @return Returns hash
*/
unsigned hashCode(char *key);

// ---------------------- FUNCTIONS -----------------------

/**
 * @struct Structure for function calls.
 */
typedef struct funct_call{
	tDynamicString     funct_name;
	struct funct_call* ptrnext;
} tFunct_call;

/**
 * @struct Structure for function data representation.
 */
typedef struct HTfunct_item_data{
	tFunct_call*    funct_call_list;
	tDynamicString  funct_name;
	int             num_of_params;
    bool            defined;
} tHTfunct_item_data;

/**
 * @struct Symbol table for function representation.
 */
typedef struct HTfunct_item{
	tDynamicString         key;
	tHTfunct_item_data*    data;
	struct HTfunct_item*   ptrnext;
} tHTfunct_item;

typedef tHTfunct_item* tHTfunct[HTSIZE];


/**
* @brief Function initializes symbol table
*
* @param HTfunct Pointer to hash table for functions
*/
void HTfunct_init(tHTfunct* HTfunct);

/**
* @brief Function finds symbol(function) in symbol table and returns it.
*
* @param HTfunct Pointer to hash table for functions
* @param key Key for function identification. (= its name)
* @return Returns found symbol or NULL when symbol wasnt found.
*/
tHTfunct_item* HTfunct_get_item(tHTfunct* HTfunct, char *key);

/**
* @brief Function finds symbol(function) in symbol table and returns its data.
*
* @param HTfunct Pointer to hash table for functions
* @param key Key for function identification. (= its name)
* @return Returns data of found symbol or NULL when symbol wasnt found.
*/
tHTfunct_item_data* HTfunct_get_item_data(tHTfunct* HTfunct, char *key);

/**
* @brief Function inserts item to hash table for functions
*
* @param HTfunct Pointer to hash table for functions
* @param key Key for function identification. (= its name)
* @param defined Holds bool whether function was declared or not
* @param num_of_params Number of function parameters
* @return Returns 0 on success, 1 if item of given key is already present, 99 on malloc failure
*/
int HTfunct_insert_item(tHTfunct* HTfunct, char *key, bool defined, int num_of_params);

/**
* @brief Function inserts name of the next called function to the function_calls structure
* 
* @param HTfunct Pointer to hash table for functions
* @param key Key for function identification (= its name)
* @param name Name of the inserting function(call)
*/
int HTfunct_insert_funct_call(tHTfunct* HTfunct, char *key, char* name);

/**
* @brief Function removes item from hash table for functions
*
* @param HTfunct Pointer to hash table for functions
* @param key Key for function identification (= its name)
*/
void HTfunct_remove_item(tHTfunct* HTfunct, char *key);		

/**
* @brief Function deletes hash table for functions
*
* @param HTfunct Pointer to hash table for functions
*/
void HTfunct_delete(tHTfunct* HTfunct); 

// ------------------ VARIABLES ---------------------

typedef struct HTvar_item_data{
    tDynamicString    name; // == key
    bool              defined;
} tHTvar_item_data;

typedef struct HTvar_item{
	tDynamicString     key; // == name
	tHTvar_item_data*  data;
	struct HTvar_item* ptrnext;
} tHTvar_item;

typedef tHTvar_item* tHTvar[HTSIZE];


void HTvar_init(tHTvar* HTvar);
tHTvar_item* HTvar_get_item(tHTvar* HTvar, char *key);					
tHTvar_item_data* HTvar_get_item_data(tHTvar* HTvar, char *key);			 
int HTvar_insert_item(tHTvar* HTvar, char *key, bool defined);
void HTvar_remove_item(tHTvar* HTvar, char *key);
void HTvar_delete(tHTvar* HTvar); 			

#endif
	
