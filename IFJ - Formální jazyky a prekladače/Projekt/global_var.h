#ifndef _GLOBAL_VAR_H_
#define _GLOBAL_VAR_H_

#include "token.h"
#include "stack.h"
#include "symtable.h"


struct global_var{
        tHTfunct* HTfunct;
        tHTvar* HTglobalVar;
        tHTvar* HTlocalVar;
        //instrIFJcode19* instruct_temp_arr;  //memory for instructions (tListOfInstr *list)
        tCodeGenerator generator;
        FILE* input;
        tToken token;        
        tStack scannerIndentStack;
        //int counterVar = 1;
        //tErrQueueElem ErrorQueue;
        //rest of global variables ?
};
typedef struct global_var* tGlobalVars;

#endif