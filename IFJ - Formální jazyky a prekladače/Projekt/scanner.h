/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kočalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file scanner.h
 * @author Jakub Kočalka, xkocal00
 * @brief Interface for the lexical analyzer for IFJ19
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "token.h"
#include "dynamicString.h"
#include "stack.h"

enum state{
    STATE_START,
    STATE_FALSE,

    STATE_ID,
    STATE_EOL,

    //ERROR: Doesn't handle multiple exponenets
    STATE_NUMBER, //Could be int or float, this state IS final
    STATE_NUMBER_POS_FLOAT_POINT, //NAME NOT ACCURATE, Next digit specifies a valid float
    STATE_NUMBER_POS_FLOAT_EXP, //an exponent was added
    STATE_NUMBER_POS_FLOAT_SIGNED_EXP, //An sign was added
    STATE_NUMBER_FLOAT, //Is real number

    STATE_POS_STRING_QUOTE, //A quote was read,
    STATE_POS_STRING_APOSTROPHE, //An apostrophe was read,
    STATE_POS_STRING_QUOTE_ESCAPE_SEQUENCE, //A backslash was read, don't end string at quote
    STATE_POS_STRING_APOSTROPHE_ESCAPE_SEQUENCE, //A backslash was read, don't end string at apostrophe
    STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE,
    STATE_POS_STRING_APOSTROPHE_HEXADECIMAL_SEQUENCE,
    STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE_2,
    STATE_POS_STRING_APOSTROPHE_HEXADECIMAL_SEQUENCE_2,
    STATE_STRING, //A second quote/apostrophe was

    STATE_COMMENT, //A # was read, discarding everything until EOL
    STATE_DOCSTRING,

    //Operator States
    STATE_ADD,
    STATE_SUB,
    STATE_MUL,
    STATE_DIV,
    STATE_IDIV,

    STATE_EQ,
    STATE_POS_NEQ, //An exclamation mark was read
    STATE_NEQ,
    STATE_LTN,
    STATE_LEQ,
    STATE_GTN,
    STATE_GEQ,

    STATE_ASSIGN,

    STATE_LPAR,
    STATE_RPAR,
    STATE_COMMA,
    STATE_COLLON,

    STATE_INDENT_COUNTING,
    STATE_INDENT, //helper state telling token generator to generate indent
    STATE_DEDENT, //helper state telling token generator to generate dedent

    STATE_EOF,
};
typedef enum state tState;

//Prototypes of public functions

/**
 * @brief Get the Next Token object
 * 
 * @param token pointer to an initialized token structure
 * @param str pointer to an initialized dynamicString structure
 * @param input file to be scanned for token
 * @return -1 if EOF was encountered, 1 if lexical error, 0 otherwise
 */
int getNextToken(tToken token, tStack indentStack, FILE* input);

#endif //__SCANNER_H__