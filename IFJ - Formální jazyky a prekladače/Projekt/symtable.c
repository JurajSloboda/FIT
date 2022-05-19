/*
 * Created on Sun Oct 27 2019
 *
 * Copyright (c) 2019 Kristina Hostacna, Andrej Ryska
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file symtable.c
 * @author Kristina Hostacna, Andrej Ryska
 * @brief 
 */

#include "symtable.h"

// -------------- FUNCTIONS -------------------

unsigned hashCode(char *key){

    int retVal = 1;
    int keylen = strlen(key);

    for (int i = 0; i < keylen; i++)
        retVal += key[i];

    return (retVal % HTSIZE);
}

void HTfunct_init(tHTfunct* HTfunct){

    // INIT
    for (int i = 0; i < HTSIZE; i++){
        (*HTfunct)[i] = NULL;
    }

    // pridam vestevene funkce do tabulky
    HTfunct_insert_item(HTfunct, "inputs", true, 0);
    HTfunct_insert_item(HTfunct, "inputi", true, 0);
    HTfunct_insert_item(HTfunct, "inputf", true, 0);
    HTfunct_insert_item(HTfunct, "print" , true, 0);
    HTfunct_insert_item(HTfunct, "len"   , true, 1);
    HTfunct_insert_item(HTfunct, "substr", true, 3);
    HTfunct_insert_item(HTfunct, "ord"   , true, 2);
    HTfunct_insert_item(HTfunct, "chr"   , true, 1);

}

tHTfunct_item* HTfunct_get_item(tHTfunct* HTfunct, char *key){

    tHTfunct_item *item = (*HTfunct)[hashCode(key)];

    while (item != NULL){

        if (strcmp(key, getStrData(item->key)) == 0)
            return item;

        item = item->ptrnext;
    }

    return NULL;
}

tHTfunct_item_data* HTfunct_get_item_data(tHTfunct* HTfunct, char *key){

    tHTfunct_item *item = HTfunct_get_item(HTfunct, key);

    if (item == NULL)
        return NULL;
    else
        return (item->data);
}

int HTfunct_insert_item(tHTfunct* HTfunct, char *key, bool defined, int num_of_params){

    tHTfunct_item *item = HTfunct_get_item(HTfunct, key);

    // Ak sa uz item nachadza
    if (item != NULL){
        return 1;
    }

    item = (tHTfunct_item*)malloc(sizeof(struct HTfunct_item));
    if (item == NULL)
        return 99; // malloc error

    tHTfunct_item_data* data = (tHTfunct_item_data*)malloc(sizeof(struct HTfunct_item_data));
    if (data == NULL)
        return 99;

    item->data = data;

    /* adding values */

    // KEY
    item->key = initDynamicString(strlen(key)+1);
    strcpy(getStrData(item->key), key);

    // ptrnext
    unsigned hash = hashCode(key);
    item->ptrnext = (*HTfunct)[hash];
    
    // funct_name
    item->data->funct_name = initDynamicString(strlen(key)+1); 
    strcpy(getStrData(item->data->funct_name), key);

    // num_of_params
    item->data->num_of_params = num_of_params;

    // defined
    item->data->defined = defined;

    // funct_call_list
    item->data->funct_call_list = NULL;
    

    (*HTfunct)[hash] = item;

    return 0;
}

int HTfunct_insert_funct_call(tHTfunct* HTfunct, char *key, char* name){

    tHTfunct_item *item = HTfunct_get_item(HTfunct, key);

    // Ak sa item nenachadza
    if (item == NULL){
        return 1;
    }

    tFunct_call* new_call = (tFunct_call*)malloc(sizeof(struct funct_call));
    if (new_call == NULL)
        return 99; // Malloc failed

    new_call->funct_name = initDynamicString(strlen(name)+1);
    strcpy(getStrData(new_call->funct_name), key);
    new_call->ptrnext = NULL;


    tFunct_call* callList = item->data->funct_call_list;

    // callList je prazdny
    if (callList == NULL){

        item->data->funct_call_list = new_call;
        return 0;
    }
    // callList nie je prazdny
    else{

        while (callList != NULL){

            // nasiel som posledny
            if (callList->ptrnext == NULL){

                callList->ptrnext = new_call;
                return 0;
            }

            callList = callList->ptrnext;
        }
    }
    return 0;
}

void HTfunct_remove_item(tHTfunct* HTfunct, char *key){	
    
    unsigned hash = hashCode(key);
    tHTfunct_item *item = (*HTfunct)[hash];
    tHTfunct_item *prevItem = NULL;

    while(item != NULL){

        if (strcmp(key, getStrData(item->key)) == 0){

            if (prevItem == NULL)
                (*HTfunct)[hash] = item->ptrnext;

            if (prevItem != NULL)
                prevItem->ptrnext = item->ptrnext;

            deleteDynamicString(&(item->key));
            deleteDynamicString(&(item->data->funct_name));

            tFunct_call *callList = item->data->funct_call_list;
            tFunct_call *itemToDelete = NULL;

            while(callList != NULL){

                itemToDelete = callList;
                callList = callList->ptrnext;

                deleteDynamicString(&(itemToDelete->funct_name));
                free(itemToDelete);
            }

            free(item);
            return;
        }

        prevItem = item;
        item = item->ptrnext;
    }
}

void HTfunct_delete(tHTfunct* HTfunct){
    
    tHTfunct_item *item = NULL;
    tHTfunct_item *itemToDelete = NULL;

    tFunct_call *callList = NULL;
    tFunct_call *callToDelete = NULL;

    for (int i = 0; i < HTSIZE; i++){

        item = (*HTfunct)[i];

        while (item != NULL){

            itemToDelete = item;
            item = item->ptrnext;

            deleteDynamicString(&(itemToDelete->key));
            deleteDynamicString(&(itemToDelete->data->funct_name));

            callList = itemToDelete->data->funct_call_list;
            callToDelete = NULL;

            while(callList != NULL){

                callToDelete = callList;
                callList = callList->ptrnext;

                deleteDynamicString(&(callToDelete->funct_name));
                free(callToDelete);
            }

            free(itemToDelete);
        }

        (*HTfunct)[i] = NULL;
    }
}

// ---------------- VARIABLES ---------------------

void HTvar_init(tHTvar* HTvar){

    for (int i = 0; i < HTSIZE; i++)
        (*HTvar)[i] = NULL;
}

tHTvar_item* HTvar_get_item(tHTvar* HTvar, char *key){

    tHTvar_item *item = (*HTvar)[hashCode(key)];

    while (item != NULL){

        if (strcmp(key, getStrData(item->key)) == 0)
            return item;

        item = item->ptrnext;
    }

    return NULL;
}					

tHTvar_item_data* HTvar_get_item_data(tHTvar* HTvar, char *key){

    tHTvar_item *item = HTvar_get_item(HTvar, key);

    if (item == NULL)
        return NULL;
    else
        return (item->data);
}

int HTvar_insert_item(tHTvar* HTvar, char *key, bool defined){
    
    tHTvar_item *item = HTvar_get_item(HTvar, key);

    // Ak sa uz key nachadza
    if (item != NULL){
        return 1;
    }

    item = (tHTvar_item*)malloc(sizeof(struct HTvar_item));
    if (item == NULL)
        return 99; // malloc error

    tHTvar_item_data* data = (tHTvar_item_data*)malloc(sizeof(struct HTvar_item_data));
    if (data == NULL)
        return 99;

    item->data = data;

    // KEY
    item->key = initDynamicString(strlen(key)+1);
    strcpy(getStrData(item->key), key);

    // ptrnext
    unsigned hash = hashCode(key);
    item->ptrnext = (*HTvar)[hash];
    
    // name
    item->data->name = initDynamicString(strlen(key)+1); 
    strcpy(getStrData(item->data->name), key);

    // defined
    item->data->defined = defined;
    

    (*HTvar)[hash] = item;

    return 0;
}

void HTvar_remove_item(tHTvar* HTvar, char *key){
    
    tHTvar_item *item = (*HTvar)[hashCode(key)];
    tHTvar_item *prevItem = NULL;

    while(item != NULL){

        if (strcmp(getStrData(item->key), key) == 0){

            if (prevItem == NULL)
                (*HTvar)[hashCode(key)] = item->ptrnext;

            if (prevItem != NULL)
                prevItem->ptrnext = item->ptrnext;

            deleteDynamicString(&((item->key)));
            deleteDynamicString(&(item->data->name));
            free(item->data);
            free(item);
            return;
        }

        prevItem = item;
        item = item->ptrnext;
    }
}

void HTvar_delete(tHTvar* HTvar){
    
    tHTvar_item *item = NULL;
    tHTvar_item *itemToDelete = NULL;

    for (int i = 0; i < HTSIZE; i++){

        item = (*HTvar)[i];

        while(item != NULL){

            itemToDelete = item;
            item = item->ptrnext;

            deleteDynamicString(&(itemToDelete->key));
            deleteDynamicString(&(itemToDelete->data->name));
            free(itemToDelete->data);
            free(itemToDelete);
        }

        (*HTvar)[i] = NULL;
    } 
}

	
