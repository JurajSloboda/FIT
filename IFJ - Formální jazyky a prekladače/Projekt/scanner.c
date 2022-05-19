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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"

//private prototypes
bool isCutSymbol(int c, tState state, FILE* input);
bool isOperator(int c);
bool isPar(int c);
int convertHexStrToDec(char* hex, unsigned len);

int generateToken(tToken token, tState state);

 
/*int main(int argc, char* argv[]){
    FILE* input;

    if(argc > 1){
        input = fopen(argv[1], "r");
    }else{
        input = stdin;
    }

    //bool gl_wasEOL = false; //indicates whether we should be generating an INDENT/DEDENT token

    printf("\nScanner Starting\n====\n");

    while(!feof(input)){
        tToken token = initToken();
        tStack indentStack = StackInit();
        stackPush(indentStack, 0);
        getNextToken(token, indentStack, input);
        printf("TOKEN_TYPE: %d, TOKEN_ATTRIBUTE: %s\n", getTokenType(token), getStrData(getTokenAttribute(token)));
        deleteToken(token);
    }
}*/

int getNextToken(tToken token, tStack indentStack, FILE* input){
    static bool firstToken = true; //initialize to true, to find indent of first line
    static bool endOfFile = false;

    int result = 0;
    int c; //the current symbol
    char hex[3] = {0}; //for reading hexadecimal escape sequences
    int curSpaceCount = 0;
    static int dedentsToGenerate = 0; //how many dedents should we force onto parser before reading any new symbols
    tState state = STATE_START;
    bool isCut = false; //we've read an entire token
    bool dontAppend = false;

    //clear string
    clearDynamicString(getTokenAttributePtr(token));

    while(!isCut){
        //generate all missing dedent tokens
        if(endOfFile){
            if(stackPop(indentStack) != 0){
                state = STATE_DEDENT;
                dedentsToGenerate--;
                isCut = true;    
            }else{
                state = STATE_EOF;
                isCut = true;
            }
        }else if(dedentsToGenerate > 0){
            state = STATE_DEDENT;
            dedentsToGenerate--;
            isCut = true;
            continue;
        }else{

            //read input
            c = fgetc(input);

           fprintf(stderr,"[SCANER]State: %d, Symbol: %c(%d), Str: %s\n", state, c, c, getStrData(getTokenAttribute(token)));

            //perform computational step
            switch (state)
            {
                case STATE_INDENT_COUNTING:
                   fprintf(stderr,"[SCANNER]{indenting}stackTop: %d\n", stackTop(indentStack));
                    if(c == '\n')
                    {
                        curSpaceCount = 0;
                        state = STATE_START;
                    }
                    else if (c == '#')
                    {
                        curSpaceCount = 0;
                        state = STATE_COMMENT;
                    }
                    else if(isspace(c))
                    {
                        curSpaceCount++;
                        state = STATE_INDENT_COUNTING;
                    }
                    else
                    {
                        ungetc(c, input);
                        firstToken = false;
                       fprintf(stderr,"[SCANNER]{indenting}stackTop: %d, curSpaceCount: %d\n", stackTop(indentStack), curSpaceCount);
                        if(curSpaceCount == stackTop(indentStack)){
                            firstToken = false;
                            state = STATE_START;
                            continue;
                        }else if(curSpaceCount > stackTop(indentStack)){
                            stackPush(indentStack, curSpaceCount);
                            state = STATE_INDENT;
                            isCut = true;
                            continue;
                        }else{
                            stackPop(indentStack);
                            while(curSpaceCount != stackTop(indentStack)){
                                if(curSpaceCount < stackTop(indentStack)){
                                    stackPop(indentStack);
                                    dedentsToGenerate++;
                                }else{
                                    state = STATE_FALSE;
                                }
                            }
                            state = STATE_DEDENT;
                            isCut = true;
                            continue;
                        }
                    }
                    
                break;

                case STATE_START:
                    if(c == '\n')
                    {
                        if(firstToken){
                            state = STATE_START;
                        }else{
                            state = STATE_EOL;
                        }
                    }
                    else if(isspace(c))
                    {
                        if(firstToken){
                            curSpaceCount++;
                            state = STATE_INDENT_COUNTING;
                        }else{
                            state = STATE_START;
                        }
                    }
                    else if (isalpha(c) || c == '_')
                    {
                        if(firstToken){
                            ungetc(c, input);
                            curSpaceCount = 0;
                            state = STATE_INDENT_COUNTING;
                            continue;
                        }else{
                            state = STATE_ID;
                        }
                    }
                    else if(isdigit(c))
                    {
                        if(firstToken){
                            ungetc(c, input);
                            curSpaceCount = 0;
                            state = STATE_INDENT_COUNTING;
                            continue;
                        }else{
                            state = STATE_NUMBER;
                        }
                    }
                    else if(c == '"')
                    {
                        if(firstToken){
                            ungetc(c, input);
                            curSpaceCount = 0;
                            state = STATE_INDENT_COUNTING;
                            continue;
                        }else{
                            state = STATE_POS_STRING_QUOTE;
                            dontAppend = true;
                        }
                    }
                    else if(c == '\'')
                    {
                        if(firstToken){
                            ungetc(c, input);
                            curSpaceCount = 0;
                            state = STATE_INDENT_COUNTING;
                            continue;
                        }else{
                            state = STATE_POS_STRING_APOSTROPHE;
                            dontAppend = true;
                        }
                    }
                    else if(c == '+')
                    {
                        state = STATE_ADD;
                    }
                    else if(c == '-')
                    {
                        state = STATE_SUB;
                    }
                    else if(c == '*')
                    {
                        state = STATE_MUL;
                    }
                    else if(c == '/')
                    {
                        state = STATE_DIV;
                    }
                    else if(c == '<')
                    {
                        state = STATE_LTN;
                    }
                    else if(c == '>')
                    {
                        state = STATE_GTN;
                    }
                    else if(c == '=')
                    {
                        state = STATE_ASSIGN;
                    }
                    else if(c == '!')
                    {
                        state = STATE_POS_NEQ;
                    }
                    else if(c == '(')
                    {
                        if(firstToken){
                            ungetc(c, input);
                            curSpaceCount = 0;
                            state = STATE_INDENT_COUNTING;
                            continue;
                        }else{
                            state = STATE_LPAR;
                        }
                    }
                    else if(c == ')')
                    {
                        state = STATE_RPAR;
                    }
                    else if(c == ',')
                    {
                        state = STATE_COMMA;
                    }
                    else if(c == ':')
                    {
                        state = STATE_COLLON;
                    }
                    else if(c == '#')
                    {
                        state = STATE_COMMENT;
                    }
                    else if(c == EOF)
                    {
                        endOfFile = true;
                        continue;
                    }
                    else
                    {
                        state = STATE_FALSE;
                    }
                break;

                case STATE_COMMENT:
                    if(c == '\n')
                    {
                        state = STATE_EOL;
                    }
                    else
                    {
                        state = STATE_COMMENT;
                    }
                break; 

                case STATE_EOL:
                    //ungetc(c, input);
                    firstToken = true;
                    isCut = true;
                break;

                case STATE_ID:
                    if(isalnum(c) || c == '_')
                    {
                        state = STATE_ID;
                    }
                    else
                    {
                        isCut = true;
                    }
                break;   

                //ERROR: Doesn't handle multiple exponenets
                #pragma region NUMBER_STATES
                case STATE_NUMBER:
                    if(isdigit(c))
                    {
                        state = STATE_NUMBER;
                    }
                    else if(c == '.')
                    {
                        state = STATE_NUMBER_POS_FLOAT_POINT;
                    }
                    else if(c == 'e' || c == 'E')
                    {
                        state = STATE_NUMBER_POS_FLOAT_EXP;
                    }
                    else
                    {
                        isCut = true;
                        //generateToken(token, str, state);
                    }
                break;
                
                case STATE_NUMBER_POS_FLOAT_POINT:
                    if(isdigit(c))
                    {
                        state = STATE_NUMBER_FLOAT;
                    }
                    else
                    {
                        state = STATE_FALSE;
                    }
                break;
                
                case STATE_NUMBER_POS_FLOAT_EXP:
                    if(isdigit(c))
                    {
                        state = STATE_NUMBER_FLOAT;
                    }
                    else if(c == '+' || c == '-')
                    {
                        state = STATE_NUMBER_POS_FLOAT_POINT;
                    }
                    else
                    {
                        state = STATE_FALSE;
                    }
                break;

                case STATE_NUMBER_FLOAT:
                    if(isdigit(c))
                    {
                        state = STATE_NUMBER_FLOAT;
                    }
                    else if(c == 'e' || c == 'E')
                    {
                        state = STATE_NUMBER_POS_FLOAT_EXP;
                    }
                    else
                    {
                        isCut = true;
                    }
                break;
                #pragma endregion

                #pragma region STRING_STATES
                case STATE_POS_STRING_QUOTE:
                    if(c != '"' && c != '\\')
                    {
                        if(c == 32){
                            appendToString(getTokenAttributePtr(token), '\\');
                            appendToString(getTokenAttributePtr(token), '0');
                            appendToString(getTokenAttributePtr(token), '3');
                            appendToString(getTokenAttributePtr(token), '2');
                            dontAppend = true;
                        }else if(c == 35){
                            appendToString(getTokenAttributePtr(token), '\\');
                            appendToString(getTokenAttributePtr(token), '0');
                            appendToString(getTokenAttributePtr(token), '3');
                            appendToString(getTokenAttributePtr(token), '5');
                            dontAppend = true;
                        }
                        state = STATE_POS_STRING_QUOTE;
                    }
                    else if (c == '\\')
                    {
                        state = STATE_POS_STRING_QUOTE_ESCAPE_SEQUENCE;
                        dontAppend = true;
                    }
                    else
                    {
                        state = STATE_STRING;
                        dontAppend = true;
                    }
                break;

                case STATE_POS_STRING_APOSTROPHE:
                    if(c != '\'' && c != '\\')
                    {
                        if(c == ' '){
                            appendToString(getTokenAttributePtr(token), '\\');
                            appendToString(getTokenAttributePtr(token), '0');
                            appendToString(getTokenAttributePtr(token), '3');
                            appendToString(getTokenAttributePtr(token), '2');
                            dontAppend = true;
                        }else if(c == '#'){
                            appendToString(getTokenAttributePtr(token), '\\');
                            appendToString(getTokenAttributePtr(token), '0');
                            appendToString(getTokenAttributePtr(token), '3');
                            appendToString(getTokenAttributePtr(token), '5');
                            dontAppend = true;
                        }
                        state = STATE_POS_STRING_APOSTROPHE;
                    }
                    else if (c == '\\')
                    {
                        state = STATE_POS_STRING_APOSTROPHE_ESCAPE_SEQUENCE;
                        dontAppend = true;
                    }
                    else
                    {
                        state = STATE_STRING;
                        dontAppend = true;
                    }
                break;

                case STATE_POS_STRING_QUOTE_ESCAPE_SEQUENCE:
                    if(c == 'x'){
                        appendToString(getTokenAttributePtr(token), '\\');
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE;
                    }
                    else if(c == '"')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '3');
                        appendToString(getTokenAttributePtr(token), '4');
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE;
                    }
                    else if(c == '\'')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '3');
                        appendToString(getTokenAttributePtr(token), '9');
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE;
                    }
                    else if(c == 'n')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '1');
                        appendToString(getTokenAttributePtr(token), '0');
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE;
                    }
                    else if(c == 't')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '9');
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE;
                    }
                    else
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        state = STATE_POS_STRING_QUOTE;
                    }
                break;

                case STATE_POS_STRING_APOSTROPHE_ESCAPE_SEQUENCE:
                    if(c == 'x'){
                        appendToString(getTokenAttributePtr(token), '\\');
                        state = STATE_POS_STRING_APOSTROPHE_HEXADECIMAL_SEQUENCE;
                    }
                    else if(c == '"')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '3');
                        appendToString(getTokenAttributePtr(token), '4');
                        dontAppend = true;
                        state = STATE_POS_STRING_APOSTROPHE;
                    }
                    else if(c == '\'')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '3');
                        appendToString(getTokenAttributePtr(token), '9');
                        dontAppend = true;
                        state = STATE_POS_STRING_APOSTROPHE;
                    }
                    else if(c == 'n')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '1');
                        appendToString(getTokenAttributePtr(token), '0');
                        dontAppend = true;
                        state = STATE_POS_STRING_APOSTROPHE;
                    }
                    else if(c == 't')
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '0');
                        appendToString(getTokenAttributePtr(token), '9');
                        dontAppend = true;
                        state = STATE_POS_STRING_APOSTROPHE;
                    }
                    else
                    {
                        appendToString(getTokenAttributePtr(token), '\\');
                        state = STATE_POS_STRING_APOSTROPHE;
                    }
                break;

                case STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE:
                    if(isxdigit(c)){
                        hex[0] = c;
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE_2;
                    }else{
                        appendToString(getTokenAttributePtr(token), 'x');
                        state = STATE_POS_STRING_QUOTE;
                    }
                break;

                case STATE_POS_STRING_APOSTROPHE_HEXADECIMAL_SEQUENCE:
                    if(isxdigit(c)){
                        hex[0] = c;
                        dontAppend = true;
                        state = STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE_2;
                    }else{
                        appendToString(getTokenAttributePtr(token), 'x');
                        state = STATE_POS_STRING_QUOTE;
                    }
                break;

                case STATE_POS_STRING_QUOTE_HEXADECIMAL_SEQUENCE_2:
                    if(isxdigit(c)){
                        hex[1] = c;

                        unsigned tmp = convertHexStrToDec(hex, 2);
                        char dec[4] = {0};

                        sprintf(dec, "%03d", tmp);

                        appendToString(getTokenAttributePtr(token), dec[0]);
                        appendToString(getTokenAttributePtr(token), dec[1]);
                        appendToString(getTokenAttributePtr(token), dec[2]);

                        dontAppend = true;
                    }else{
                        appendToString(getTokenAttributePtr(token), hex[0]);
                    }
                    state = STATE_POS_STRING_QUOTE;
                break;

                case STATE_POS_STRING_APOSTROPHE_HEXADECIMAL_SEQUENCE_2:
                    if(isxdigit(c)){
                        hex[1] = c;

                        unsigned tmp = convertHexStrToDec(hex, 2);
                        char dec[5] = {0};

                        snprintf(dec, 4, "%03d", tmp);

                        appendToString(getTokenAttributePtr(token), dec[0]);
                        appendToString(getTokenAttributePtr(token), dec[1]);
                        appendToString(getTokenAttributePtr(token), dec[2]);

                        dontAppend = true;
                    }else{
                        appendToString(getTokenAttributePtr(token), hex[0]);
                    }
                    state = STATE_POS_STRING_APOSTROPHE;
                break;

                case STATE_STRING:
                    isCut = true;
                break;
                #pragma endregion

                #pragma region OPERATOR_STATES_AND_ASSIGN
                case STATE_ADD:
                    isCut = true;
                break;

                case STATE_SUB:
                    isCut = true;
                break;

                case STATE_MUL:
                    isCut = true;
                break;

                case STATE_DIV:
                    if(c == '/')
                    {
                        state = STATE_IDIV;
                    }
                    else
                    {
                        isCut = true;
                    }
                break;

                case STATE_IDIV:
                    isCut = true;
                break;

                case STATE_LTN:
                    if(c == '=')
                    {
                        state = STATE_LEQ;
                    }
                    else
                    {
                        isCut = true;
                    }
                break;

                case STATE_LEQ:
                    isCut = true;
                break;

                case STATE_GTN:
                    if(c == '=')
                    {
                        state = STATE_GEQ;
                    }
                    else
                    {
                        isCut = true;
                    }
                break;

                case STATE_GEQ:
                    isCut = true;
                break;

                case STATE_ASSIGN:
                    if(c == '=')
                    {
                        state = STATE_EQ;
                    }
                    else
                    {
                        isCut = true;
                    }
                break;

                case STATE_EQ:
                    isCut = true;
                break;

                case STATE_POS_NEQ:
                    if(c == '=')
                    {
                        state = STATE_NEQ;
                    }
                    else
                    {
                        state = STATE_FALSE;
                    }
                break;

                case STATE_NEQ:
                    isCut = true;
                break;

                #pragma endregion

                case STATE_LPAR:
                    isCut = true;
                break;

                case STATE_RPAR:
                    isCut = true;
                break;

                case STATE_COMMA:
                    isCut = true;
                break;

                case STATE_COLLON:
                    isCut = true;
                break;

                case STATE_FALSE:
                    isCut = true;
                break;

                default:
                    fprintf(stderr, "DEFAULT CASE, undefined state\n");
                    return -1;
                break;
            }
        }

        //add symbol to str
        if(!isCut){
            bool isStringState = (state == STATE_POS_STRING_QUOTE || state == STATE_POS_STRING_APOSTROPHE || state == STATE_POS_STRING_APOSTROPHE_ESCAPE_SEQUENCE || state == STATE_POS_STRING_QUOTE_ESCAPE_SEQUENCE);
            if((!isspace(c) || isStringState) && !dontAppend){ //don't append whitespace
                appendToString(getTokenAttributePtr(token), c);
            }
            dontAppend = false;
        }else{
            ungetc(c, input);
            //printf("Generating State %d\n", state);
        }
    }
    result = generateToken(token, state);
    fprintf(stderr, "[SCANER]SCANNER RESULT: %d\n", isCut?1:0);
    if(result == 1){
        return result;
    }else{
        return isCut?0:-1;
    }
}

int generateToken(tToken token, tState state){
    switch (state)
    {
        
        #pragma region ID_AND_KEYWORD_STATES
        case STATE_ID:
            if (strcmp(getStrData(getTokenAttribute(token)), "def" ) == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_DEF);
            }
            else if(strcmp(getStrData(getTokenAttribute(token)), "if" ) == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_IF);
            }
            else if(strcmp(getStrData(getTokenAttribute(token)), "else" ) == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_ELSE);
            }
            else if(strcmp(getStrData(getTokenAttribute(token)), "while" ) == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_WHILE);
            }
            else if(strcmp(getStrData(getTokenAttribute(token)), "None" ) == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_NONE);
            }
            else if(strcmp(getStrData(getTokenAttribute(token)), "pass" ) == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_PASS);
            }
            else if(strcmp(getStrData(getTokenAttribute(token)), "return") == 0)
            {
                setTokenType(token, TOKEN_TYPE_KEYWORD_RETURN);
            }
            else
            {
                setTokenType(token, TOKEN_TYPE_ID);
            }
        break;
        #pragma endregion

        #pragma region NUMBER_STATES
        case STATE_NUMBER:
            setTokenType(token, TOKEN_TYPE_INT);
        break;

        case STATE_NUMBER_FLOAT:
            setTokenType(token, TOKEN_TYPE_FLOAT);
        break;

        #pragma endregion

        #pragma region OPERATOR_STATES
        case STATE_ADD:
            setTokenType(token, TOKEN_TYPE_ADD);
        break;
        
        case STATE_SUB:
            setTokenType(token, TOKEN_TYPE_SUB);
        break;

        case STATE_MUL:
            setTokenType(token, TOKEN_TYPE_MUL);
        break;

        case STATE_DIV:
            setTokenType(token, TOKEN_TYPE_DIV);
        break;

        case STATE_IDIV:
            setTokenType(token, TOKEN_TYPE_IDIV);
        break;

        case STATE_LTN:
            setTokenType(token, TOKEN_TYPE_LTN);
        break;

        case STATE_LEQ:
            setTokenType(token, TOKEN_TYPE_LEQ);
        break;

        case STATE_GTN:
            setTokenType(token, TOKEN_TYPE_GTN);
        break;

        case STATE_GEQ:
            setTokenType(token, TOKEN_TYPE_GEQ);
        break;

        case STATE_EQ:
            setTokenType(token, TOKEN_TYPE_EQ);
        break;

        case STATE_NEQ:
            setTokenType(token, TOKEN_TYPE_NEQ);
        break;

        #pragma endregion

        case STATE_ASSIGN:
            setTokenType(token, TOKEN_TYPE_ASSIGN);
        break;

        case STATE_LPAR:
            setTokenType(token, TOKEN_TYPE_LPAR);
        break;

        case STATE_RPAR:
            setTokenType(token, TOKEN_TYPE_RPAR);
        break;

        case STATE_COMMA:
            setTokenType(token, TOKEN_TYPE_COMMA);
        break;

        case STATE_COLLON:
            setTokenType(token, TOKEN_TYPE_COLON);
        break;

        case STATE_INDENT:
            setTokenType(token, TOKEN_TYPE_INDENT);
        break;

        case STATE_DEDENT:
            setTokenType(token, TOKEN_TYPE_DEDENT);
        break;

        case STATE_EOL:
            setTokenType(token, TOKEN_TYPE_EOL);
        break;

        case STATE_EOF:
            setTokenType(token, TOKEN_TYPE_EOF);
        break;
        
        case STATE_STRING:
            setTokenType(token, TOKEN_TYPE_STR);
        break;

        case STATE_FALSE:
            setTokenType(token, TOKEN_TYPE_FALSE);
            return 1;
        break;

        
        default:
            fprintf(stderr, "ERROR: Token type for this state was not defined.\n");
            exit(-1);
        break;
    }
   fprintf(stderr,"[SCANER]Generating Token: %d\n", getTokenType(token));
    return 0;
}

bool isOperator(int c){
    bool result = false;
    
    switch (c)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '<':
    case '>':
    case '=':
    case '!':
        result = true;
    break;
    
    default:
        result = false;
    break;
    }

    return result;
}

bool isPar(int c){
    return (c == '(' || c == ')')?true:false;
}

unsigned myPow(unsigned a, unsigned b){
    unsigned result = a;
    if(b == 0){
        result = 1;
    }else{
        for (size_t i = 0; i < b-1; i++)
        {
            result *= a;
        }
    }
    return result;
}

int convertHexStrToDec(char* hex, unsigned len){
    int result = 0;
    for (size_t i = 0; i < len; i++)
    {
        if(hex[i] <= '9'){
            result += (hex[i] - '0') * myPow(16, (len-i-1));
        }else if(hex[i] >= 'A' && hex[i] <= 'Z'){
            result += (hex[i] - 'A') * myPow(16, (len-i-1));
        }else if(hex[i] >= 'a' && hex[i] <= 'z'){
            result += (hex[i] - 'a') * myPow(16, (len-i-1));
        }
    }
    return result;
}