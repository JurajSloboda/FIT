/*
 * Created on Sun Oct 27 2019
 *
 * Copyright (c) 2019 Kristina Hostacna
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file codeGenerator.c
 * @author Jakub Kočalka
 * @brief 
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codeGenerator.h"

struct instruction {                            //variables table lists
    tDynamicString op_code;                     //keyword- intruction name (case insensitive)
    tDynamicString op1;                    //case sensitive (var/const/label)
    tDynamicString op2;                    //case sensitive (var/const/label)
    tDynamicString op3;
};
typedef struct instruction* tInstruction;

struct instQueueElem{
    tInstruction inst;
    struct instQueueElem *next;
};
typedef struct instQueueElem* tInstQueueElem;

struct instructionQueue{
    tInstQueueElem head;
    tInstQueueElem tail;

    size_t count;
};


struct codeGenerator
{
    tInstQueue instQueue;
    tInstQueue funcQueue;

    bool generatingFunc;

    size_t ifCounter; //counts conditional statements for the purposes of generating labels
    size_t whileCounter; //counts cycles for the purposes of generating labels
};

///////////////////////////////// MACROS //////////////////////////////////////////////////////////////
//can only be used inside generator instance functions, must be stored in a tInstQueue variable
#define CHOOSE_QUEUE (*generator)->generatingFunc?((*generator)->funcQueue):((*generator)->instQueue)

///////////////////////////////// Private prototypes //////////////////////////////////////////////////////////////
//Queue
tInstQueue queueInit();
tInstQueueElem newQueueElem(tInstruction inst);
tInstruction initInst(char* op_code, char* op1, char* op2, char* op3);
bool queueEmpty(tInstQueue q);
void enqueInst(tInstQueue q, char* op_code, char* op1, char* op2, char* op3);
tInstruction dequeInst(tInstQueue q);
void enqueComment(tInstQueue q, char* commentText);
void enqueLF(tInstQueue q);
void deleteInst(tInstruction *inst);

//Library Function Definitions
void libAbstractInput(tCodeGenerator *generator, char* inputType);
void libInputi(tCodeGenerator *generator);
void libInputf(tCodeGenerator *generator);
void libInputs(tCodeGenerator *generator);
void libPrint(tCodeGenerator *generator);
void libOrd(tCodeGenerator *generator);
void libChr(tCodeGenerator *generator);
void libLen(tCodeGenerator *generator);


//Helper
void helpAdd(tCodeGenerator *generator);
void helpSub(tCodeGenerator *generator);
void helpMul(tCodeGenerator *generator);
void helpDiv(tCodeGenerator *generator);
void helpIdiv(tCodeGenerator *generator);

void instClearStack(tCodeGenerator *generator);
void instAssignFullName(tCodeGenerator *generator, char* dest, char* source);


//////////////////////////////////    Code generator    //////////////////////////////////////////////////////////////

#pragma region generator

tCodeGenerator codeGeneratorInit(){
    tCodeGenerator generator = malloc(sizeof(struct codeGenerator));

    generator->instQueue = queueInit();
    generator->funcQueue = queueInit();

    generator->generatingFunc = false;
    generator->ifCounter = 0;
    generator->whileCounter = 0;

    enqueInst(generator->instQueue, ".IFJCode19", "", "", "");
    /*generate helper instructions*/
    enqueComment(generator->instQueue, "Helper Variables");
    enqueLF(generator->instQueue);
    //Unconditional jump that skips function definition code.
    //The program should end with an %end label.
    enqueLF(generator->funcQueue);
    enqueComment(generator->funcQueue, "Function definitions");
    enqueInst((generator->funcQueue), "JUMP", "$end", "", "");
    enqueLF(generator->funcQueue);
    //variables to store values on stack for the purposes of type control
    enqueInst(generator->instQueue, "DEFVAR", "GF@%stack_type_control_value_1", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%stack_type_control_value_2", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%stack_type_control_type_1", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%stack_type_control_type_2", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%stack_type_control_bool", "", "");
    enqueLF(generator->instQueue);
    //variables for concatenation (there is no stack version of concat)
    enqueInst(generator->instQueue, "DEFVAR", "GF@%concat_result", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%concat_op1", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%concat_op2", "", "");
    //variables for popping expression operands
    enqueInst(generator->instQueue, "DEFVAR", "GF@%exp_op1", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%exp_op2", "", "");
    enqueLF(generator->instQueue);
    //variables for LTE, GTE
    enqueInst(generator->instQueue, "DEFVAR", "GF@%compare_equal_value_1", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%compare_equal_value_2", "", "");
    enqueInst(generator->instQueue, "DEFVAR", "GF@%compare_equal_result", "", "");
    enqueLF(generator->instQueue);
    //variable for popping for the purposes of returning value and assigning
    enqueInst(generator->instQueue, "DEFVAR", "GF@%tmp_pop", "", "");
    enqueLF(generator->instQueue);

    /*generate library functions*/
    enqueComment(generator->funcQueue, "Library Functions");
    libInputi(&generator);
    libInputf(&generator);
    libInputs(&generator);
    libPrint(&generator);
    libOrd(&generator);
    libChr(&generator);
    libLen(&generator);

    helpAdd(&generator);
    helpSub(&generator);
    helpMul(&generator);
    helpDiv(&generator);
    helpIdiv(&generator);

    return generator;
}

void generateInstructions(tCodeGenerator *generator){
    while (!queueEmpty((*generator)->instQueue))
    {
        tInstruction inst = dequeInst(((*generator)->instQueue));
        printf("%s %s %s %s\n", getStrData(inst->op_code), getStrData(inst->op1), getStrData(inst->op2), getStrData(inst->op3));
    }
    
    enqueInst(((*generator)->funcQueue), "LABEL", "$type_err", "", "");
    enqueInst(((*generator)->funcQueue), "EXIT", "int@4", "", "");
    enqueInst(((*generator)->funcQueue), "LABEL", "$end", "", "");

    while (!queueEmpty((*generator)->funcQueue))
    {
        tInstruction inst = dequeInst(((*generator)->funcQueue));
        printf("%s %s %s %s\n", getStrData(inst->op_code), getStrData(inst->op1), getStrData(inst->op2), getStrData(inst->op3));
    }

}

#pragma endregion

#pragma region queue

tInstQueue queueInit(){
    tInstQueue q = malloc(sizeof(struct instructionQueue));
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
    return q;
}

tInstQueueElem newQueueElem(tInstruction inst){
    tInstQueueElem temp = malloc(sizeof(struct instQueueElem));
    temp->inst = inst;
    temp->next = NULL;
    return temp;
}

tInstruction initInst(char* op_code, char* op1, char* op2, char* op3){
    tInstruction inst = malloc(sizeof(struct instruction));
    inst->op_code = initDynamicString(STRING_CHUNK_SIZE);
    setDynamicString(&(inst->op_code), op_code);
    inst->op1 = initDynamicString(STRING_CHUNK_SIZE);
    setDynamicString(&(inst->op1), op1);
    inst->op2 = initDynamicString(STRING_CHUNK_SIZE);
    setDynamicString(&(inst->op2), op2);
    inst->op3 = initDynamicString(STRING_CHUNK_SIZE);
    setDynamicString(&(inst->op3), op3);
    return inst;
}

bool queueEmpty(tInstQueue q){
    //return (q->count == 0)?true:false;
    return (q->tail == NULL)?true:false;
}


void enqueInst(tInstQueue q, char* op_code, char* op1, char* op2, char* op3){
    tInstruction inst = initInst(op_code, op1, op2, op3);
    tInstQueueElem tmpElem = newQueueElem(inst);

    if(queueEmpty(q)){
        q->head = tmpElem;
        q->tail = tmpElem;
    }else{
        q->tail->next = tmpElem;
        q->tail = tmpElem;
    }
    (q->count)++;
}

tInstruction dequeInst(tInstQueue q){
    if(queueEmpty(q)){
        return NULL;
    }else{
        tInstQueueElem temp = q->head;
        q->head = q->head->next;

        if(q->head == NULL){
            q->tail = NULL;
        }
        tInstruction tmpInst = temp->inst;
        free(temp);
        return tmpInst;

    }
    q->count = q->count - 1;
}

void enqueComment(tInstQueue q, char* commentText){
    char* formatedComment = malloc(strlen(commentText)+2);
    sprintf(formatedComment, "# %s", commentText);

    enqueInst(q, formatedComment, "", "", "");

    free(formatedComment);
}

void enqueLF(tInstQueue q){
    enqueInst(q, "", "", "", "");
}

void deleteInst(tInstruction *inst){
    deleteDynamicString(&((*inst)->op_code));
    deleteDynamicString(&((*inst)->op1));
    deleteDynamicString(&((*inst)->op2));
    deleteDynamicString(&((*inst)->op3));
    free((*inst));
}

#pragma endregion

#pragma region instructions

#pragma region variables
void instDecVar(tCodeGenerator *generator, char* scope, char* varName){
    tInstQueue q = CHOOSE_QUEUE; 

    char* varf = malloc(strlen(scope) + strlen(varName) + 2);
    sprintf(varf, "%s@%s", scope, varName);

    enqueInst(q, "DEFVAR", varf, "", "");

    free(varf);
}

void instAssign(tCodeGenerator *generator, char* scope, char* varName, char* type, char* value){
    tInstQueue q = CHOOSE_QUEUE;

    char* varf = malloc(strlen(scope) + strlen(varName) + 2);
    sprintf(varf, "%s@%s", scope, varName);
    char* formatedVal = malloc(strlen(type)+strlen(value)+2);
    sprintf(formatedVal, "%s@%s", type, value);

    enqueInst(q, "MOVE", varf, formatedVal, "");

    free(varf);
    free(formatedVal);
}

void instAssignFullName(tCodeGenerator *generator, char* dest, char* source){
    tInstQueue q = CHOOSE_QUEUE;

    enqueInst(q, "MOVE", dest, source, "");
}

void instAssignFromStack(tCodeGenerator *generator, char* scope, char* varName){
    tInstQueue q = CHOOSE_QUEUE;

    char* varf = malloc(strlen(scope) + strlen(varName) + 2);
    sprintf(varf, "%s@%s", scope, varName);

    enqueInst(q, "POPS", varf, "", "");

    free(varf);
}

void instDecVarAndAssign(tCodeGenerator *generator, char* scope, char* varName, char* type, char* value){
    instDecVar(generator, scope, varName);
    instAssign(generator, scope, varName, type, value);
}
#pragma endregion

#pragma region if-else
void instIf(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;
    const int elseLabelPrefixSize = 13; //$if_else_else_

    enqueInst(q, "PUSHS", "bool@true", "", "");
    char *str = calloc(elseLabelPrefixSize + GetOrderOfInteger((*generator)->ifCounter), 1);
    sprintf(str, "$if_else_else_%ld", (*generator)->ifCounter);
    enqueInst(q, "JUMPIFNEQS", str, "", "");

    free(str);
}

void instElse(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;
    const int elseLabelPrefixSize = 13; //$if_else_else_
    const int endLabelPrefixSize = 12; //$if_else_end_

    char *str1 = calloc(endLabelPrefixSize + GetOrderOfInteger((*generator)->ifCounter), 1);
    sprintf(str1, "$if_else_end_%ld", (*generator)->ifCounter);
    enqueInst(q, "JUMP", str1, "", "");

    char *str2 = calloc(elseLabelPrefixSize + GetOrderOfInteger((*generator)->ifCounter), 1);
    sprintf(str2, "$if_else_else_%ld", (*generator)->ifCounter);
    enqueInst(q, "LABEL", str2, "", "");

    free(str1);
    free(str2);
}

void instIfElseEnd(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;
    const int  endLabelPrefixSize = 12; //$if_else_end_

    char *str = calloc(endLabelPrefixSize + GetOrderOfInteger((*generator)->ifCounter), 1);
    sprintf(str, "$if_else_end_%ld", (*generator)->ifCounter);
    enqueInst(q, "LABEL", str, "", "");

    free(str);
    ((*generator)->ifCounter)++;
}
#pragma endregion

#pragma region while
void instWhileStart(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;

    const int  startLabelPrefixSize = 12; //$while_start_
    char *strLabelStart = calloc(startLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelStart, "$while_start_%ld", (*generator)->whileCounter);

    const int endLabelPrefixSize = 10; //$while_end_
    char *strLabelEnd = calloc(endLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelEnd, "$while_end_%ld", (*generator)->whileCounter);

    enqueInst(q, "LABEL", strLabelStart, "", "");

    //check condition
    enqueInst(q, "PUSHS", "bool@true", "", "");
    enqueInst(q, "JUMPIFNEQS", strLabelEnd, "", ""); //while(stackPop){

    free(strLabelStart);
    free(strLabelEnd);
}

void instWhileEnd(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;

    const int  startLabelPrefixSize = 12; //$while_start_
    char *strLabelStart = calloc(startLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelStart, "$while_start_%ld", (*generator)->whileCounter);

    const int endLabelPrefixSize = 10; //$while_end_
    char *strLabelEnd = calloc(endLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelEnd, "$while_end_%ld", (*generator)->whileCounter);

    enqueInst(q, "JUMP", strLabelStart, "", ""); //}
    enqueInst(q, "LABEL", strLabelEnd, "", "");

    ((*generator)->whileCounter)++;

    free(strLabelStart);
    free(strLabelEnd);
}
#pragma endregion

#pragma region expresions
void instExpressionFromPostfix(tCodeGenerator *generator, char* postfix){
    fprintf(stderr, "[GENERATOR] Postfix:%s\n", postfix);
    tInstQueue q = CHOOSE_QUEUE;
    //size_t postfixLen = strlen(postfix);
    tDynamicString str = initDynamicString(STRING_CHUNK_SIZE);
    tDynamicString firstValue = initDynamicString(STRING_CHUNK_SIZE); //for purposes of assign
    bool isFirst = true; //for purposes of assign

    int state = 0;

    int i = 0;
    char c = postfix[i];
    while(c != '\0'){
        switch (state)
        {
        case 0: //reading token
            if (c == '+')
            {
                state = 1;
            }
            else if(c == '-')
            {
                state = 2;
            }
            else if(c == '*')
            {
                state = 3;
            }
            else if (c == '/')
            {
                state = 4;
            }
            else if(c == '<')
            {
                state = 5;
            }
            else if(c == '>')
            {
                state = 6;
            }
            else if(c == '=')
            {
                state = 7;
            }
            else if (isalnum(c) || c == '_' || c == '@')
            {
                appendToString(&str, c);
                state = 0;
            }
            else if(isspace(c))
            {
                enqueInst(q, "PUSHS", getStrData(str), "", "");
                if(isFirst){
                    setDynamicString(&firstValue, getStrData(str));
                    isFirst = false;
                }
                clearDynamicString(&str);
                state = 0;
            }
        break;

        case 1: //ADD OR CONCAT
            if(isspace(c)){
                instCallFunctionStart(generator, "$help_add");
                instCallFunctionEnd(generator, "$help_add");

                state = 0;
            }
        break;

        case 2: //SUB
            if(isspace(c)){
                instCallFunctionStart(generator, "$help_sub");
                instCallFunctionEnd(generator, "$help_sub");

                state = 0;
            }else{
                state = 12;
            }
        break;

        case 3: //MUL
            if(isspace(c)){
                instCallFunctionStart(generator, "$help_mul");
                instCallFunctionEnd(generator, "$help_mul");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 4: //POS_DIV
            if(c == '/'){
                state = 8;
            }else if(isspace(c)){
                instCallFunctionStart(generator, "$help_div");
                instCallFunctionEnd(generator, "$help_div");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 5: //POS_LT
            if(c == '='){
                state = 9;
            }else if(isspace(c)){
                instCallFunctionStart(generator, "$help_lt");
                instCallFunctionEnd(generator, "$help_lt");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 6: //POS_GT
            if(c == '='){
                state = 10;
            }else if(isspace(c)){
                instCallFunctionStart(generator, "$help_gt");
                instCallFunctionEnd(generator, "$help_gt");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 7: //EQ
            if(c == '='){
                state = 11;
            }else if(isspace(c)){
                instAssignFromStack(generator, "GF", "%tmp_pop");
                instAssignFullName(generator, getStrData(firstValue), "GF@%tmp_pop");
                instClearStack(generator);
            }else{
                state = 12;
            }
        break;

        case 8: //IDIV
            if(isspace(c)){
                //check types
                instCallFunctionStart(generator, "$help_idiv");
                instCallFunctionEnd(generator, "$help_idiv");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 9: //LE
            if(isspace(c)){
                instCallFunctionStart(generator, "$help_le");
                instCallFunctionEnd(generator, "$help_le");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 10: //GE
            if(isspace(c)){
                instCallFunctionStart(generator, "$help_ge");
                instCallFunctionEnd(generator, "$help_ge");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 11: //EQ
            if(isspace(c)){
                //check types
                instCallFunctionStart(generator, "$help_eq");
                instCallFunctionEnd(generator, "$help_eq");
                state = 0;
            }else{
                state = 12;
            }
        break;

        case 12: //ERR
            fprintf(stderr, "ERR: HUH, something went wrong while generating instructions from postfix.");
        break;

        default:
            state = 12;
            break;
        }
        i++;
        c = postfix[i];
    }
}

void instPush(tCodeGenerator *generator, char* prefix, char* value){
    tInstQueue q = CHOOSE_QUEUE;

    char* formatedValue = calloc(strlen(prefix)+1+strlen(value), 1);
    sprintf(formatedValue, "%s@%s", prefix, value);

    enqueInst(q, "PUSHS", formatedValue, "", "");

    free(formatedValue);
}

void instClearStack(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;
    enqueInst(q, "CLEARS", "", "", "");
}
#pragma endregion

#pragma region functions
void instFunctionDefStart(tCodeGenerator *generator, char* name){
    (*generator)->generatingFunc = true;
    tInstQueue q = CHOOSE_QUEUE;

    enqueInst(q, "LABEL", name, "", "");
    enqueInst(q, "PUSHFRAME", "", "", "");
    enqueInst(q, "DEFVAR", "LF@%retval", "", "");
    enqueInst(q, "MOVE", "LF@%retval", "nil@nil", "");
}

void instFunctionDefParam(tCodeGenerator *generator, char* paramName, size_t paramNum){
        tInstQueue q = CHOOSE_QUEUE;

        char* fstr1 = calloc(3+strlen(paramName), 1);
        char* fstr2 = calloc(4+3, 1); //no more than 100 params
        sprintf(fstr1, "LF@%s", paramName);
        sprintf(fstr2, "LF@%%%ld", paramNum);
        enqueInst(q, "DEFVAR", fstr1, "", "");
        enqueInst(q, "MOVE", fstr1, fstr2, "");
        free(fstr1);
        free(fstr2);
}

void instFunctionDefReturnFromStack(tCodeGenerator *generator){
    tInstQueue q = CHOOSE_QUEUE;

    //fill return value
    enqueInst(q, "POPS", "GF@%tmp_pop", "", "");
    enqueInst(q, "MOVE", "LF@%retval", "GF@%tmp_pop", "");

    enqueInst(q, "POPFRAME", "", "", "");
    enqueInst(q, "RETURN", "", "", "");
}

void instFunctionDefReturn(tCodeGenerator *generator, char* returnValue){
    tInstQueue q = CHOOSE_QUEUE;

    //fill return value, if applicable
    if(returnValue != NULL && (strcmp(returnValue, "") != 0)){
        enqueInst(q, "MOVE", "LF@%retval", returnValue, "");
    }

    enqueInst(q, "POPFRAME", "", "", "");
    enqueInst(q, "RETURN", "", "", "");
}

void instFunctionDefEnd(tCodeGenerator *generator){
    enqueLF(CHOOSE_QUEUE);
    (*generator)->generatingFunc = false;
}

void instCallFunctionStart(tCodeGenerator *generator, char* name){
    tInstQueue q = CHOOSE_QUEUE;
    enqueLF(q);
    enqueComment(q, name);
    enqueInst(q, "CREATEFRAME", "", "", "");
}

void instCallFunctionParam(tCodeGenerator *generator, char* prefix, char *paramValue, size_t paramNum){
    tInstQueue q = CHOOSE_QUEUE;

    char* formalParam = calloc(4+GetOrderOfInteger(paramNum), 1);
    sprintf(formalParam, "TF@%%%ld", paramNum);

    char* actualParam = calloc(strlen(prefix)+1+GetOrderOfInteger(paramNum), 1);
    sprintf(actualParam, "%s@%s", prefix, paramValue);

    enqueInst(q, "DEFVAR", formalParam, "", "");
    enqueInst(q, "MOVE", formalParam, actualParam, "");

    free(formalParam);
    free(actualParam);
}

//DEPRECATED
/*void instCallFunction(tCodeGenerator *generator, char* name, unsigned paramCount, ...){
    tInstQueue q = CHOOSE_QUEUE;
    va_list valist;
    va_start(valist, paramCount);
    enqueInst(q, "CREATEFRAME", "", "", "");
    for (size_t i = 1; i <= paramCount; i++)
    {
        char* fstr = calloc(4+3, 1); //no more than 100 params
        sprintf(fstr, "LF@%%%ld", i);
        enqueInst(q, "DEFVAR", fstr, "", "");
        enqueInst(q, "MOVE", fstr, va_arg(valist, char*), "");
        free(fstr);
    }
    enqueInst(q, "CALL", name, "", "");
}*/

void instCallFunctionEnd(tCodeGenerator *generator, char* name){
    tInstQueue q = CHOOSE_QUEUE;
    enqueInst(q, "CALL", name, "", "");
}

void instGetReturnVal(tCodeGenerator *generator, char* scope, char* name){
    tInstQueue q = CHOOSE_QUEUE;

    char* var = calloc(strlen(scope)+1+strlen(name), 1);
    sprintf(var, "%s@%s", scope, name);

    enqueInst(q, "MOVE", var, "TF@%retval", "");

    free(var);
}
#pragma endregion

#pragma region libFunctions
//segfault somewhere here (probably)
void libAbstractInput(tCodeGenerator *generator, char* inputType){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    char *str = calloc(7, 1);
    if(strcmp(inputType, "int") == 0){
        sprintf(str, "input%c", 'i');
    }else if(strcmp(inputType, "float") == 0){
        sprintf(str, "input%c", 'f');
    }else if(strcmp(inputType, "string") == 0){
        sprintf(str, "input%c", 's');
    }

    instFunctionDefStart(generator, str);
    enqueInst(q, "READ", "LF@%retval", inputType, "");
    instFunctionDefReturn(generator, ""); //WARNING: requires the function not to overwrite %retval when no return value is specified
    instFunctionDefEnd(generator);

    free(str);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void libInputi(tCodeGenerator *generator){
    libAbstractInput(generator, "int");
}

void libInputf(tCodeGenerator *generator){
    libAbstractInput(generator, "float");
}

void libInputs(tCodeGenerator *generator){
    libAbstractInput(generator, "string");
}

void libLen(tCodeGenerator *generator){
    instFunctionDefStart(generator, "len");
    instFunctionDefParam(generator, "s", 1);

    enqueInst((*generator)->funcQueue, "STRLEN", "LF@%retval", "LF@s", "");
    instFunctionDefReturn(generator, ""); //WARNING: requires the function not to overwrite %retval when no return value is specified

    instFunctionDefEnd(generator);
}

void libSubstr(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "substr");
    instFunctionDefParam(generator, "s", 1);
    instFunctionDefParam(generator, "i", 2);
    instFunctionDefParam(generator, "n", 3);

    //TODO: Check types

    enqueInst(q, "DEFVAR", "LF@result", "", "");
    enqueInst(q, "DEFVAR", "LF@c", "", "");
    enqueInst(q, "DEFVAR", "LF@iter", "", ""); enqueInst(q, "MOVE", "LF@iter", "int@0", "");
    enqueInst(q, "DEFVAR", "LF@strlen", "", ""); enqueInst(q, "STRLEN", "LF@strlen", "LF@s", "");
    enqueInst(q, "SUB", "LF@strlen", "LF@strlen", "int@1"); //decrement strlen to account for zero base indexing
    enqueInst(q, "SUB", "LF@strlen", "LF@strlen", "LF@i"); // strlen = strlen - i;

    //get first char
    enqueInst(q, "GETCHAR", "LF@result", "LF@s", "LF@i");
    enqueInst(q, "ADD", "LF@iter", "LF@iter", "int@1");
    enqueInst(q, "ADD", "LF@i", "LF@i", "int@1");

    const int  startLabelPrefixSize = 12; //$while_start_
    char *strLabelStart = calloc(startLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelStart, "$while_start_%ld", (*generator)->whileCounter);
    const int endLabelPrefixSize = 10; //$while_end_
    char *strLabelEnd = calloc(endLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelEnd, "$while_end_%ld", (*generator)->whileCounter);

    enqueInst(q, "LABEL", strLabelStart, "", "");

    //check condition
    enqueInst(q, "JUMPIFNEQ", strLabelEnd, "LF@iter", "LF@strlen"); //while(iter != strlen
    enqueInst(q, "JUMPIFNEQ", strLabelEnd, "LF@iter", "LF@n"); //      && iter != n){

    enqueInst(q, "GETCHAR", "LF@result", "LF@s", "LF@i");
    enqueInst(q, "ADD", "LF@i", "LF@i", "int@1");
    enqueInst(q, "ADD", "LF@iter", "LF@iter", "int@1");

    enqueInst(q, "JUMP", strLabelStart, "", ""); //}
    enqueInst(q, "LABEL", strLabelEnd, "", "");

    free(strLabelStart);
    free(strLabelEnd);

    ((*generator)->whileCounter)++;

    instFunctionDefReturn(generator, "LF@result"); // return result

    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
} 

/*Calling the print function:
    1. push all the parameters to stack
    2. pass the number of parameters as the first and only formal parameter
    3. call this function
*/
void libPrint(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "print");
    instFunctionDefParam(generator, "%paramCount", 1);

    //while labels
    const int  startLabelPrefixSize = 12; //$while_start_
    char *strLabelStart = calloc(startLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelStart, "$while_start_%ld", (*generator)->whileCounter);
    const int endLabelPrefixSize = 10; //$while_end_
    char *strLabelEnd = calloc(endLabelPrefixSize + GetOrderOfInteger((*generator)->whileCounter), 1);
    sprintf(strLabelEnd, "$while_end_%ld", (*generator)->whileCounter);

    //helper variables
    enqueInst(q, "DEFVAR", "LF@%stackPop", "", "");

    enqueInst(q, "LABEL", strLabelStart, "", "");

    //check condition
    enqueInst(q, "JUMPIFEQ", strLabelEnd, "LF@%paramCount", "int@0"); //while(iter != 0){

    enqueInst(q, "POPS", "LF@%stackPop", "", "");
    enqueInst(q, "WRITE", "LF@%stackPop", "", "");

    enqueInst(q, "SUB", "LF@%paramCount", "LF@%paramCount", "int@1"); //%paramCount--

    enqueInst(q, "JUMPIFEQ", "$printLabelShouldPrintSpaceOrNot", "LF@%paramCount", "int@0"); //if (paramCount != 0) {
    enqueInst(q, "WRITE", "string@\\032", "", ""); //write space
    enqueInst(q, "LABEL", "$printLabelShouldPrintSpaceOrNot", "", ""); // } //end if

    enqueInst(q, "JUMP", strLabelStart, "", ""); //}
    enqueInst(q, "LABEL", strLabelEnd, "", "");

    enqueInst(q, "WRITE", "string@\\010", "", ""); //write newline

    free(strLabelStart);
    free(strLabelEnd);

    ((*generator)->whileCounter)++;

    instFunctionDefReturn(generator, "nil@nil");

    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void libPrintOld(tCodeGenerator *generator, size_t paramCount){
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "print");

    for (size_t i = 1; i <= paramCount; i++)
    {
        //LF@%print
        char* paramNameStrf = calloc(9+GetOrderOfInteger(i), 1);
        sprintf(paramNameStrf, "LF@%%print%ld", i);
        
        instFunctionDefParam(generator, paramNameStrf, i);

        free(paramNameStrf);
    }

    //print function
    for (size_t i = 1; i <= paramCount; i++)
    {
        char* paramNameStrf = calloc(9+GetOrderOfInteger(i), 1);
        sprintf(paramNameStrf, "LF@%%print%ld", i);

        enqueInst(q, "WRITE", paramNameStrf, "", "");
        if(i != paramCount){ //don't print space after last param
            enqueInst(q, "WRITE", "string@\\032", "", ""); //space
        }

        free(paramNameStrf);
    }

    enqueInst(q, "WRITE", "string@\\010", "", ""); //newline
    
    instFunctionDefReturn(generator, "nil@nil");

    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void libOrd(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "ord");
    instFunctionDefParam(generator, "s", 1);
    instFunctionDefParam(generator, "i", 2);
    
    enqueInst(q, "DEFVAR", "LF@bool", "", "");
    enqueInst(q, "DEFVAR", "LF@strlen", "", "");
    enqueInst(q, "DEFVAR", "LF@result", "", "");

    enqueInst(q, "STRLEN", "LF@strlen", "LF@s", "");
    enqueInst(q, "LT", "LF@bool", "LF@i", "LF@strlen");
    enqueInst(q, "JUMPIFEQ", "$ORD_OUT_OF_BOUNDS_LABEL", "LF@bool", "bool@false"); //if((i < strlen) == false) return nil;

    enqueInst(q, "STRI2INT", "LF@result", "LF@s", "LF@i");
    instFunctionDefReturn(generator, "LF@result");

    enqueInst(q, "JUMP", "$ORD_END_LABEL", "", "");
    enqueInst(q, "LABEL", "$ORD_OUT_OF_BOUNDS_LABEL", "", "");

    instFunctionDefReturn(generator, "nil@nil");

    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void libChr(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "chr");
    instFunctionDefParam(generator, "i", 1);

    enqueInst(q, "DEFVAR", "LF@result", "", "");

    enqueInst(q, "INT2CHAR", "LF@result", "LF@i", "");

    instFunctionDefReturn(generator, "LF@result");

    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

#pragma region arithmeticHelpers

void helpAdd(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_add");
    //no formal params, everything is on stack

    //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$add_type_ok", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");
    enqueInst(q, "JUMPIFEQ", "$add_op2_to_float", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFEQ", "$add_op1_to_float", "GF@%stack_type_control_type_2", "string@float");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //type conversion
    enqueInst(q, "LABEL", "$add_op1_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "JUMP", "$add_type_ok", "", "");

    enqueInst(q, "LABEL", "$add_op2_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMP", "$add_type_ok", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$add_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");
    
    //should perform concat?
    enqueInst(q, "JUMPIFEQ", "$add_perform_concat", "GF@%stack_type_control_type_1", "string@string");

    //add
    enqueInst(q, "ADDS", "", "", "");

    //concat
    enqueInst(q, "JUMP", "$add_computation_end", "", "");
    enqueInst(q, "LABEL", "$add_perform_concat", "", "");
    enqueInst(q, "POPS", "GF@%concat_op1", "", "");
    enqueInst(q, "POPS", "GF@%concat_op2", "", "");
    enqueInst(q, "CONCAT", "GF@%concat_result", "GF@%concat_op1", "GF@%concat_op2");
    enqueInst(q, "PUSHS", "GF@%concat_result", "", "");

    enqueInst(q, "LABEL", "$add_computation_end", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpSub(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_sub");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$sub_type_eq", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");
    enqueInst(q, "JUMPIFEQ", "$sub_op2_to_float", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFEQ", "$sub_op1_to_float", "GF@%stack_type_control_type_2", "string@float");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //type conversion
    enqueInst(q, "LABEL", "$sub_op1_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "JUMP", "$sub_type_ok", "", "");

    enqueInst(q, "LABEL", "$sub_op2_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_2", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMP", "$sub_type_ok", "", "");

    //check everything is number
    enqueInst(q, "LABEL", "$sub_type_eq", "", "");
    enqueInst(q, "JUMPIFEQ", "$sub_type_ok", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFEQ", "$sub_type_ok", "GF@%stack_type_control_type_1", "string@int");
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$sub_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "SUBS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpMul(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_mul");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$mul_type_eq", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");
    enqueInst(q, "JUMPIFEQ", "$mul_op2_to_float", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFEQ", "$mul_op1_to_float", "GF@%stack_type_control_type_2", "string@float");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //type conversion
    enqueInst(q, "LABEL", "$mul_op1_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "JUMP", "$mul_type_ok", "", "");

    enqueInst(q, "LABEL", "$mul_op2_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMP", "$mul_type_ok", "", "");

    //check everything is number
    enqueInst(q, "LABEL", "$mul_type_eq", "", "");
    enqueInst(q, "JUMPIFEQ", "$mul_type_ok", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFEQ", "$mul_type_ok", "GF@%stack_type_control_type_1", "string@int");
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$mul_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "MUlS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpDiv(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_div");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$div_type_eq", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");
    enqueInst(q, "JUMPIFEQ", "$div_op2_to_float", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFEQ", "$div_op1_to_float", "GF@%stack_type_control_type_2", "string@float");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //type conversion
    enqueInst(q, "LABEL", "$div_op1_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "JUMP", "$div_type_ok", "", "");

    enqueInst(q, "LABEL", "$div_op2_to_float", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_2", "string@int"); //check if can be converted
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMP", "$div_type_ok", "", "");

    //check everything is float
    enqueInst(q, "LABEL", "$div_type_eq", "", "");
    enqueInst(q, "JUMPIFEQ", "$div_type_ok", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@int");
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    //enqueInst(q, "JUMP", "$div_type_ok", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$div_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "DIVS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpIdiv(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_idiv");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$idiv_type_eq", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");
    enqueInst(q, "JUMPIFEQ", "$idiv_op2_to_int", "GF@%stack_type_control_type_1", "string@int");
    enqueInst(q, "JUMPIFEQ", "$idiv_op1_to_int", "GF@%stack_type_control_type_2", "string@int");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //type conversion
    enqueInst(q, "LABEL", "$idiv_op1_to_int", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@float"); //check if can be converted
    enqueInst(q, "FLOAT2INT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "JUMP", "$idiv_type_ok", "", "");

    enqueInst(q, "LABEL", "$idiv_op2_to_int", "", "");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_2", "string@float"); //check if can be converted
    enqueInst(q, "FLOAT2INT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMP", "$idiv_type_ok", "", "");

    //check everything is int
    enqueInst(q, "LABEL", "$idiv_type_eq", "", "");
    enqueInst(q, "JUMPIFEQ", "$idiv_type_ok", "GF@%stack_type_control_type_1", "string@int");
    enqueInst(q, "JUMPIFNEQ", "$type_err", "GF@%stack_type_control_type_1", "string@float");
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "INT2FLOAT", "GF@%stack_type_control_value_2", "GF@%stack_type_control_value_2", "");
    //enqueInst(q, "JUMP", "$idiv_type_ok", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$idiv_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "IDIVS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpLt(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_lt");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$lt_type_ok", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$lt_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "LTS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpGt(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_gt");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$gt_type_ok", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$gt_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "GTS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpEq(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_eq");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$eq_type_ok", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$eq_type_ok", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%stack_type_control_value_2", "", "");

    enqueInst(q, "EQS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpLe(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_le");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$le_type_ok", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$le_type_ok", "", "");

    enqueInst(q, "MOVE", "GF@%compare_equal_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "MOVE", "GF@%compare_equal_value_2", "GF@%stack_type_control_value_2", "");

    enqueInst(q, "LE", "GF@%compare_equal_result", "GF@%compare_equal_value_1", "GF@%compare_equal_value_2");
    enqueInst(q, "PUSHS", "GF@%compare_equal_result", "", "");
    enqueInst(q, "PUSHS", "GF@%compare_equal_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%compare_equal_value_2", "", "");
    enqueInst(q, "EQS", "", "", "");
    enqueInst(q, "ORS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

void helpGe(tCodeGenerator *generator){
    //save generator state
    bool tmpGeneratorState = (*generator)->generatingFunc;
    (*generator)->generatingFunc = true;

    tInstQueue q = CHOOSE_QUEUE;

    instFunctionDefStart(generator, "$help_ge");
    //no formal params, everything is on stack

     //check types
    enqueInst(q, "POPS", "GF@%stack_type_control_value_1", "", "");
    enqueInst(q, "POPS", "GF@%stack_type_control_value_2", "", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "TYPE", "GF@%stack_type_control_type_2", "GF@%stack_type_control_value_2", "");
    enqueInst(q, "JUMPIFEQ", "$ge_type_ok", "GF@%stack_type_control_type_1", "GF@%stack_type_control_type_2");

    //type err
    enqueInst(q, "JUMP", "$type_err", "", "");

    //perform operation
    enqueInst(q, "LABEL", "$ge_type_ok", "", "");

    enqueInst(q, "MOVE", "GF@%compare_equal_value_1", "GF@%stack_type_control_value_1", "");
    enqueInst(q, "MOVE", "GF@%compare_equal_value_2", "GF@%stack_type_control_value_2", "");

    enqueInst(q, "GE", "GF@%compare_equal_result", "GF@%compare_equal_value_1", "GF@%compare_equal_value_2");
    enqueInst(q, "PUSHS", "GF@%compare_equal_result", "", "");
    enqueInst(q, "PUSHS", "GF@%compare_equal_value_1", "", "");
    enqueInst(q, "PUSHS", "GF@%compare_equal_value_2", "", "");
    enqueInst(q, "EQS", "", "", "");
    enqueInst(q, "ORS", "", "", "");

    //don't return anything, leave return value on stack
    instFunctionDefReturn(generator, NULL);
    instFunctionDefEnd(generator);

    //restore generator state
    (*generator)->generatingFunc = tmpGeneratorState;
}

#pragma endregion
#pragma endregion

#pragma endregion

#pragma region helperFunctions
unsigned GetOrderOfInteger(int a){
    unsigned result = 0;
    while (a != 0)
    {
        a = a / (int)10;
        result++;
    }
    return result;
}
#pragma endregion