
#include <stdio.h>

#include "parser.h"
#include "global_var.h"
#include "codeGenerator.h"
#include "stack.h"

int main(){
    
    //fprintf(stderr,"inithttable var\n");
    tGlobalVars globalVars = malloc(sizeof(struct global_var));
    globalVars->HTfunct = (tHTfunct*)malloc(sizeof( tHTfunct));
    globalVars->HTglobalVar = (tHTvar*)malloc(sizeof( tHTvar));
    globalVars->HTlocalVar = (tHTvar*)malloc(sizeof( tHTvar));
    //fprintf(stderr,"inittoken\n");
    globalVars->token = initToken();
    //fprintf(stderr,"initstack\n");
    stackInit(&(globalVars->scannerIndentStack));
    //fprintf(stderr,"initstack\n");
    stackPush(globalVars->scannerIndentStack, 0);
    //fprintf(stderr,"inithttablefunct\n");
    HTfunct_init(globalVars->HTfunct);
    //fprintf(stderr,"inithttable var\n");
    HTvar_init(globalVars->HTglobalVar);
    //fprintf(stderr,"inithttable var\n");
    HTvar_init(globalVars->HTlocalVar);
    //fprintf(stderr,"initgenerator\n");
    globalVars->generator = codeGeneratorInit();

    //FILE* fin = fopen("test.test", "r");
    //if(fin == NULL){
    //    ourError(ERR_INTERNAL, "FILENNIDOSNFKJASDBFLIHB");
    //}
    globalVars->input = stdin;

    int syntacticResult = parse(globalVars);
    fprintf(stderr, "FINAL RESULT: %d\n", syntacticResult);

    if(syntacticResult == 0){
        generateInstructions(&(globalVars->generator));
    }
    return syntacticResult;
}