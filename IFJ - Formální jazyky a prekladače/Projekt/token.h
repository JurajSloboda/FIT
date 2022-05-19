/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file token.h
 * @author Jakub Kočalka
 * @brief 
 */


#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "error.h"
#include "dynamicString.h"

enum token_type{
    
    //Arithmetic operators
    TOKEN_TYPE_ADD,
    TOKEN_TYPE_SUB,
    TOKEN_TYPE_MUL,
    TOKEN_TYPE_DIV,
    TOKEN_TYPE_IDIV,

    //Logic operators
    TOKEN_TYPE_EQ,                  //5
    TOKEN_TYPE_NEQ,
    TOKEN_TYPE_LTN,
    TOKEN_TYPE_LEQ,
    TOKEN_TYPE_GTN,
    TOKEN_TYPE_GEQ,                 //10

    TOKEN_TYPE_LPAR,
    TOKEN_TYPE_RPAR,

    //Constants
    TOKEN_TYPE_INT,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_STR,                 //15
    TOKEN_TYPE_ID,
    
    TOKEN_TYPE_EOL,
    TOKEN_TYPE_COLON,

    //KEYWORDS
    TOKEN_TYPE_KEYWORD_NONE,
    TOKEN_TYPE_KEYWORD_DEF,         //20
    TOKEN_TYPE_KEYWORD_IF,
    TOKEN_TYPE_KEYWORD_ELSE,
    TOKEN_TYPE_KEYWORD_WHILE,
    TOKEN_TYPE_KEYWORD_PASS,
    TOKEN_TYPE_KEYWORD_RETURN,      //25

    //indentation
    TOKEN_TYPE_INDENT,
    TOKEN_TYPE_DEDENT,

    
    TOKEN_TYPE_EOF,

    TOKEN_TYPE_ASSIGN,
    TOKEN_TYPE_COMMA,               //30

    //special
    TOKEN_TYPE_FALSE,
    TOKEN_TYPE_EMPTY
};
typedef enum token_type tTokenType;

struct token;
typedef struct token* tToken;

/**
 * @brief Allocates and Initializes token with a single chunk attribute and an EMPTY type
 * 
 * @return tToken pointer to the new token
 */
tToken initToken();

/**
 * @brief Deallocates token
 * 
 * @param token The token to be deleted
 */
void deleteToken(tToken token);

/**
 * @brief Get the Token Type
 * 
 * @param token 
 * @return tTokenType Type of token
 */
tTokenType getTokenType(tToken token);

/**
 * @brief Set the type of token
 * 
 * @param token Pointer to the token
 * @param type The type which should be set
 */
void setTokenType(tToken token, tTokenType type);

/**
 * @brief Get the Attribute of token
 * 
 * @param token 
 * @return tDynamicString pointer to the attribute of token
 */
tDynamicString getTokenAttribute(tToken token);

tDynamicString* getTokenAttributePtr(tToken token);

/**
 * @brief Set the Attribute of token
 * 
 * @param token pointer to the token
 * @param attribute pointer to the string which should be used as an attribute
 */
void setTokenAttribute(tToken token, tDynamicString attribute);

#endif //__TOKEN_H__