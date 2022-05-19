/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file token.c
 * @author Jakub Kočalka
 * @brief 
 */

#include "token.h"
#include <stdlib.h>

struct token{
    int type;
    tDynamicString attribute;
};

tToken initToken(){
    tToken token = malloc(sizeof(struct token));
    if(token == NULL){
        ourError(ERR_INTERNAL, "INFO: Errory while initializing token.");
    }
    token->type=TOKEN_TYPE_EMPTY;
    token->attribute=initDynamicString(STRING_CHUNK_SIZE);
    return token;
}

void deleteToken(tToken token){
    deleteDynamicString(&(token->attribute));
    free(token);
}

tTokenType getTokenType(tToken token){
    return token->type;
}

void setTokenType(tToken token, tTokenType type){
    token->type = type;
}

tDynamicString getTokenAttribute(tToken token){
    return token->attribute;
}

tDynamicString* getTokenAttributePtr(tToken token){
    return &(token->attribute);
}

void setTokenAttribute(tToken token, tDynamicString attribute){
    copyDynamicString(&(token->attribute), attribute);
}
