/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Juraj Sloboda
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file parser.c
 * @author Juraj Sloboda, xslobo07
 * @brief Interface for the syn analyzer for IFJ19
 */

#include <stdio.h>

#include "dynamicString.h"
#include "stack.h"
#include "stringStack.h"
#include "token.h"

#include "parser.h"

//Macros
// GET_TOKEN - macro for getting new token without compare
#define GET_TOKEN do{if(getNextToken(global_var->token, global_var->scannerIndentStack, global_var->input) == 1){ \
            return ERR_LEXICAL; \
        }  }while(0)
                    
                    
                    
// GET_TOKEN_OF_TYPE - macro for getting new token with compare, return ERR_SYNTACTIC in some case :D
#define GET_TOKEN_OF_TYPE(TOKEN_TYPE) do{ \
            GET_TOKEN; \
            if (getTokenType(global_var->token) != TOKEN_TYPE){ \
                return ERR_SYNTACTIC; \
            } }while(0)


// funkcie, premenne, a indent a dedent nova tabulka; 

int FakeExpressionParser(tGlobalVars global_var);
int Return2(tGlobalVars global_var);
int ID2(tGlobalVars global_var, char* scope, char* key );
int ID1(tGlobalVars global_var, char* scope, char* key );
int Term(tGlobalVars global_var, long unsigned int *ParamNum, tStringStack stringStack, bool isPrint);
int TermList(tGlobalVars global_var, long unsigned int *ParamNum, tStringStack stringStack, bool isPrint);
int TermListN(tGlobalVars global_var, long unsigned int *ParamNum, tStringStack stringStack, bool isPrint);
int Stat(tGlobalVars global_var);
int StatList(tGlobalVars global_var);
int StatListDef(tGlobalVars global_var);
int ParList(tGlobalVars global_varm,char *key, long unsigned int *NumOfParams);
int ParListN(tGlobalVars global_var, long unsigned int *NumOfParams );
int FDef(tGlobalVars global_var);
int program(tGlobalVars global_var);



// fake function for testing, replace by expresion parser when done
int FakeExpressionParser(tGlobalVars global_var){
    while (getTokenType(global_var->token) != TOKEN_TYPE_EOL){
        //fprintf(stdout,"dsadwq");
        getNextToken(global_var->token, global_var->scannerIndentStack, global_var->input);
    }
    return SYNTAX_OK;
} 


//<return'>		<exp>EOL	//int, float, string, none,id, (
//		        EOL	                //EOL
int Return2(tGlobalVars global_var){
	fprintf(stderr,"[PARSER]Return2\n");
    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_ID:
        case TOKEN_TYPE_LPAR:
            result = parse_exp(global_var);
            fprintf(stderr,"parserexp result %d",result);
            if (result != SYNTAX_OK){
                return result;
            }
            if(getTokenType(global_var->token) != TOKEN_TYPE_EOL) return ERR_SYNTACTIC;
            //generate instruction return
            instFunctionDefReturnFromStack(&(global_var->generator));
            

            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_EOL:
            
            //generate instruction return
            instFunctionDefReturn(&(global_var->generator), "");
            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;

    }
    return ERR_SYNTACTIC;
}

//<id2>		id(<termlist>EOL	//id
//		    <exp> EOL	        //int, float, string, none, (
int ID2(tGlobalVars global_var, char* scope, char* key){
	fprintf(stderr,"[PARSER]ID2\n");

    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_ID:
            if (HTfunct_get_item(global_var->HTfunct, getStrData(getTokenAttribute(global_var->token))) != NULL){
                char* tmpFunctName = malloc(strlen(getStrData(getTokenAttribute(global_var->token))));
                strcpy(tmpFunctName, getStrData(getTokenAttribute(global_var->token)));
                GET_TOKEN_OF_TYPE(TOKEN_TYPE_LPAR);

                instCallFunctionStart(&(global_var->generator), tmpFunctName);
                long unsigned int ParamNum = 1;
                tStringStack stringStack;
                stringStackInit(&stringStack);

                //print function has a different calling convention
                if(strcmp("print", tmpFunctName) == 0){
                    GET_TOKEN;
                    result = TermList(global_var, &ParamNum, stringStack, true);
                    ParamNum--; //Ugly Hack

                    while(!stringStackEmpty(stringStack)){
                        char* prefix = stringStackPop(stringStack);
                        char* value = stringStackPop(stringStack);

                        instPush(&(global_var->generator), prefix, value);

                        //free(prefix);
                        //free(value);
                    }

                    char *fstr = calloc(GetOrderOfInteger(ParamNum), 1);
                    sprintf(fstr, "%ld", ParamNum);

                    instCallFunctionParam(&(global_var->generator),"int",fstr,1);

                    //free(fstr);

                    if ( result != SYNTAX_OK) return result;
                }else{
                    GET_TOKEN;
                    result = TermList(global_var, &ParamNum, stringStack, false);
                    if ( result != SYNTAX_OK) return result;
                    ParamNum--; //Ugly Hack
                    tHTfunct_item_data* data; // = (tHTfunct_item_data*)malloc(sizeof(struct HTfunct_item_data));
                    data = HTfunct_get_item_data(global_var->HTfunct, tmpFunctName);
                    if (ParamNum != data->num_of_params){
                        return ERR_SEMANTIC_WRONG_PARAM_NUM;
                    }
                }
                instCallFunctionEnd(&(global_var->generator), tmpFunctName);
                //free(tmpFunctName);

                instGetReturnVal(&(global_var->generator), scope, key);

                GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);

                return SYNTAX_OK;
                
            } else {
                result = parse_exp(global_var);
                if (result != SYNTAX_OK) return result;

                instAssignFromStack(&(global_var->generator), scope, key);

                fprintf(stderr,"parserexp result %d \n",result);

                if (getTokenType(global_var->token) != TOKEN_TYPE_EOL){
                    return ERR_SYNTACTIC;
                }
                
                return SYNTAX_OK;
            }
            
            
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_LPAR:
            result = parse_exp(global_var);
            fprintf(stderr,"parserexp result %d \n",result);
            if (result != SYNTAX_OK) return result;
            instAssignFromStack(&(global_var->generator), scope, key);
           
            if (getTokenType(global_var->token) != TOKEN_TYPE_EOL){
                return ERR_SYNTACTIC;
            }

            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;

    }
    return ERR_SYNTACTIC;
}

//<id1>		\=<id''>	    //=
//		EOL	                //EOL
//		(<termlist>EOL	    //(
int ID1(tGlobalVars global_var, char* scope, char* key ){
	fprintf(stderr,"[PARSER]ID1\n");

    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_ASSIGN:
            GET_TOKEN;
            result = ID2(global_var, scope, key);
            if (result != SYNTAX_OK) return result;
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_EOL:
            if((HTvar_get_item(global_var->HTglobalVar, key) == NULL) && (HTvar_get_item(global_var->HTlocalVar, key) == NULL) ){
                return ERR_SEMANTIC_NONDEFINED;
            } 
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_LPAR:

            if(HTfunct_get_item(global_var->HTfunct, key) == NULL){
                fprintf(stderr,"%s\n",key);
                return ERR_SEMANTIC_NONDEFINED;
            }   
            instCallFunctionStart(&(global_var->generator), key);
            long unsigned int ParamNum = 1;
            tStringStack stringStack;
            stringStackInit(&stringStack);

            //print function has a different calling convention
            if(strcmp("print", key) == 0){
                GET_TOKEN;
                result = TermList(global_var, &ParamNum, stringStack, true);
                ParamNum--; //Ugly Hack

                while(!stringStackEmpty(stringStack)){
                    char* prefix = stringStackPop(stringStack);
                    char* value = stringStackPop(stringStack);

                    instPush(&(global_var->generator), prefix, value);

                    //free(prefix);
                    //free(value);
                }

                char *fstr = calloc(GetOrderOfInteger(ParamNum), 1);
                sprintf(fstr, "%ld", ParamNum);

                instCallFunctionParam(&(global_var->generator),"int",fstr,1);

                //free(fstr);

                if ( result != SYNTAX_OK) return result;
            }else{
                GET_TOKEN;
                result = TermList(global_var, &ParamNum, stringStack, false);
                ParamNum--; //Ugly Hack
                tHTfunct_item_data* data; //= (tHTfunct_item_data*)malloc(sizeof(struct HTfunct_item_data));
                data = HTfunct_get_item_data(global_var->HTfunct, key);
                if (ParamNum != data->num_of_params){
                    return ERR_SEMANTIC_WRONG_PARAM_NUM;
                }

                if ( result != SYNTAX_OK) return result;
            }
            instCallFunctionEnd(&(global_var->generator),key);


            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;

    }
    return ERR_SYNTACTIC;
}

//<term>          	-->     	int	    //int
//                	-->     	float 	//float
//                	-->     	string 	//string
//                	-->     	none 	//none
//                	-->     	id 	    //id
int Term(tGlobalVars global_var, long unsigned int *ParamNum, tStringStack stringStack, bool isPrint){
	fprintf(stderr,"[PARSER]Term\n");
    
    if(isPrint){
        switch(getTokenType(global_var->token)){
            case TOKEN_TYPE_INT:
                stringStackCopyAndPush(stringStack, getStrData(getTokenAttribute(global_var->token)));
                stringStackCopyAndPush(stringStack, "int");
                //instPush(&(global_var->generator), "float", getStrData(getTokenAttribute(global_var->token)));
                
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_FLOAT:
                stringStackCopyAndPush(stringStack, getStrData(getTokenAttribute(global_var->token)));
                stringStackCopyAndPush(stringStack, "float");
                //instPush(&(global_var->generator), "float", getStrData(getTokenAttribute(global_var->token)));
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_STR:
                stringStackCopyAndPush(stringStack, getStrData(getTokenAttribute(global_var->token)));
                stringStackCopyAndPush(stringStack, "string");
                //instPush(&(global_var->generator), "string", getStrData(getTokenAttribute(global_var->token)));
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_KEYWORD_NONE:
                stringStackCopyAndPush(stringStack, "nil");
                stringStackCopyAndPush(stringStack, "nil");
                //instPush(&(global_var->generator), "nil", "nil");
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_ID:
                //yostenie či je id v tabulkách
                if(HTvar_get_item(global_var->HTlocalVar, getStrData(getTokenAttribute(global_var->token))) != NULL){
                    stringStackCopyAndPush(stringStack, getStrData(getTokenAttribute(global_var->token)));
                    stringStackCopyAndPush(stringStack, "LF");
                    //instPush(&(global_var->generator), "LF", getStrData(getTokenAttribute(global_var->token)));
                }else if(HTvar_get_item(global_var->HTglobalVar, getStrData(getTokenAttribute(global_var->token))) != NULL){
                    stringStackCopyAndPush(stringStack, getStrData(getTokenAttribute(global_var->token)));
                    stringStackCopyAndPush(stringStack, "GF");
                    //instPush(&(global_var->generator), "GF", getStrData(getTokenAttribute(global_var->token)));
                }else{
                    return ERR_SEMANTIC_NONDEFINED;
                }
                
                return SYNTAX_OK;
                break;
            default:
                return ERR_SYNTACTIC;

        }
    }else{
        switch(getTokenType(global_var->token)){
            case TOKEN_TYPE_INT:
                instCallFunctionParam(&(global_var->generator),"int",getStrData(getTokenAttribute(global_var->token)),*ParamNum);
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_FLOAT:
                instCallFunctionParam(&(global_var->generator),"float", getStrData(getTokenAttribute(global_var->token)),*ParamNum);
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_STR:
                instCallFunctionParam(&(global_var->generator), "string", getStrData(getTokenAttribute(global_var->token)),*ParamNum);
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_KEYWORD_NONE:
                instCallFunctionParam(&(global_var->generator), "nil", "nil",*ParamNum);
                
                return SYNTAX_OK;
                break;
            case TOKEN_TYPE_ID:
                //yostenie či je id v tabulkách
                if(HTvar_get_item(global_var->HTlocalVar, getStrData(getTokenAttribute(global_var->token))) != NULL){
                    instCallFunctionParam(&(global_var->generator),"LF", getStrData(getTokenAttribute(global_var->token)),*ParamNum);
                }else if(HTvar_get_item(global_var->HTglobalVar, getStrData(getTokenAttribute(global_var->token))) != NULL){
                    instCallFunctionParam(&(global_var->generator),"GF", getStrData(getTokenAttribute(global_var->token)),*ParamNum);
                }else{
                    return ERR_SEMANTIC_NONDEFINED;
                }
                
                return SYNTAX_OK;
                break;
            default:
                return ERR_SYNTACTIC;

        }
    }
    return ERR_SYNTACTIC;
}

//<term list -n>  	-->     	,<term><term list -n>	    //,
//                	-->     	)	                        //)
int TermListN(tGlobalVars global_var, long unsigned int *ParamNum, tStringStack stringStack, bool isPrint){
	fprintf(stderr,"[PARSER]TermListN\n");
    //(*ParamNum)++;
    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_COMMA:
            GET_TOKEN;
            result = Term(global_var,ParamNum, stringStack, isPrint);
            if (result != SYNTAX_OK) return result;
            (*ParamNum)++;

            GET_TOKEN;
            result = TermListN(global_var,ParamNum, stringStack, isPrint);
            if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_RPAR:
            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;
    
    }
    return ERR_SYNTACTIC;
}

//<term list>     	-->     	<term><term list -n>	    //int, float, string, none, id
//                	-->     	<termlist -n>	            //,)
int TermList(tGlobalVars global_var, long unsigned int *ParamNum, tStringStack stringStack, bool isPrint){
	fprintf(stderr,"[PARSER]TermList\n");

    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR: 
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_ID:
            result = Term(global_var, ParamNum, stringStack, isPrint);
            if (result != SYNTAX_OK) return result;
            (*ParamNum)++;
            GET_TOKEN;
            result = TermListN(global_var, ParamNum, stringStack, isPrint);
            if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_RPAR:
            result = TermListN(global_var, ParamNum, stringStack, isPrint);
            if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;
    }
    return ERR_SYNTACTIC;
}

//<stat def>    -->     	id<id'>	                                                //id
//    ???       -->     	<exp2>EOL	                                            //int, float, string, none, (
//              -->     	if <exp> : EOL {<stat list>} else : EOL {<stat list>	//if
//              -->     	while <exp> : EOL {<stat list>	                        //while
//              -->     	return<return'>	                                        //return
//              -->     	pass EOL	                                            //pass
int StatDef(tGlobalVars global_var){ ///todooooo see stat
	fprintf(stderr,"[PARSER]StatDef\n");

    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_ID:
            // hľadanie v tabulke a zistenie či expr alebo nie 
            ;
            char *key = calloc(strlen(getStrData(getTokenAttribute(global_var->token))), 1);
            strcpy(key, getStrData(getTokenAttribute(global_var->token)));
            if (HTfunct_get_item(global_var->HTfunct, getStrData(getTokenAttribute(global_var->token))) != NULL){
                GET_TOKEN;
                
                result = ID1(global_var, "LF", key );
                if (result != SYNTAX_OK) return result;
                return SYNTAX_OK;
            } else {
                int Tempresult = 0;
                Tempresult = parse_exp(global_var);
                fprintf(stderr,"parserepx result %d \n", Tempresult);
                if ((Tempresult != 0)&&(Tempresult != 3) ){
                    return Tempresult;
                } 
                if (Tempresult != 0){
                    fprintf(stderr,"dasdas1\n");
                    if (HTfunct_get_item(global_var->HTfunct, getStrData(getTokenAttribute(global_var->token))) != NULL){
                        fprintf(stderr,"%s\n",getStrData(getTokenAttribute(global_var->token)));
                        char *key2 = calloc(strlen(getStrData(getTokenAttribute(global_var->token))), 1);
                        strcpy(key2, getStrData(getTokenAttribute(global_var->token)));
                        GET_TOKEN;
                        result = ID1(global_var, "LF", key2 );
                        if (result != SYNTAX_OK) return result;
                        return SYNTAX_OK;
                    }
                    GET_TOKEN;
                    if ((getTokenType(global_var->token) == TOKEN_TYPE_ASSIGN) && (Tempresult == 3)){
                        if(HTfunct_get_item(global_var->HTfunct, key) != NULL){
                            return ERR_SEMANTIC_NONDEFINED;
                        }
                        HTvar_insert_item(global_var->HTlocalVar, key, true);
                        instDecVar(&(global_var)->generator,"LF",key);
                        
                        
                    } 
                    
                    result = ID1(global_var, "LF", key );
                    if (result != SYNTAX_OK) return result;
                }
            }
            return SYNTAX_OK;
            break;

           
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_LPAR:
            result = parse_exp(global_var);
            return result;
        case TOKEN_TYPE_KEYWORD_IF:
            GET_TOKEN;
            // volanie exp parseru
            result = parse_exp(global_var);
            if (result != SYNTAX_OK) return result;
            if(getTokenType(global_var->token) != TOKEN_TYPE_COLON){
                return SYNTAX_OK;
            }
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);
            instIf(&(global_var->generator));
            GET_TOKEN;
            result = StatListDef(global_var);
            if (result != SYNTAX_OK) return result;
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_KEYWORD_ELSE);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_COLON);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);
            instElse(&(global_var->generator));
            GET_TOKEN;
            result = StatListDef(global_var);
            if (result != SYNTAX_OK) return result;
           
            instIfElseEnd(&(global_var->generator));
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_KEYWORD_WHILE: 
            GET_TOKEN;
            result = parse_exp(global_var);
            if ( result != SYNTAX_OK) return result;
            if (getTokenType(global_var->token) != TOKEN_TYPE_COLON){
                return ERR_SYNTACTIC;
            }
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);
            instWhileStart(&(global_var->generator));
            GET_TOKEN;
            result = StatListDef(global_var);
            if (result != SYNTAX_OK) return result;
            instWhileEnd(&(global_var->generator));
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_KEYWORD_RETURN:
            
            GET_TOKEN;
            result = Return2(global_var);
            if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_KEYWORD_PASS:
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;

    }
    return ERR_SYNTACTIC;
}

//<stat>        -->     	id<id'>	                                                //id
//    ???       -->     	<exp>EOL	                                            //int, float, string, none, (
//              -->     	if <exp> : EOL {<stat list>} else : EOL {<stat list>	//if
//              -->     	while <exp> : EOL {<stat list>	                        //while
//              -->     	pass EOL	                                            //pass
int Stat(tGlobalVars global_var){
	fprintf(stderr,"[PARSER]Stat\n");

    int result;
    switch(getTokenType(global_var->token)){
        case TOKEN_TYPE_ID:
            
            // 1. hladanie v tabulke, ak je to už axistujuca funkcia tak pokračuje, ak nie dáva sa to aprseru podla ktorého pokračuje ďalej
            //kontrola ci premenn
            ;
            char *key = calloc(strlen(getStrData(getTokenAttribute(global_var->token))), 1);
            strcpy(key, getStrData(getTokenAttribute(global_var->token)));
            if (HTfunct_get_item(global_var->HTfunct, getStrData(getTokenAttribute(global_var->token))) != NULL){
                GET_TOKEN;
                
                result = ID1(global_var, "GF", key );
                if (result != SYNTAX_OK) return result;
                return SYNTAX_OK;
            } else {
                int Tempresult = 0;
                Tempresult = parse_exp(global_var);
                fprintf(stderr,"parserepx result %d \n", Tempresult);
                if ((Tempresult != 0)&&(Tempresult != 3) ){
                    return Tempresult;
                } 
                if (Tempresult != 0){
                     if (HTfunct_get_item(global_var->HTfunct, getStrData(getTokenAttribute(global_var->token))) != NULL){
                         
                        fprintf(stderr,"%s\n",getStrData(getTokenAttribute(global_var->token)));
                        char *key2 = calloc(strlen(getStrData(getTokenAttribute(global_var->token))), 1);
                        
                        strcpy(key2, getStrData(getTokenAttribute(global_var->token)));
                        GET_TOKEN;
                        result = ID1(global_var, "GF", key2 );
                        if (result != SYNTAX_OK) return result;
                        return SYNTAX_OK;
                    }
                    GET_TOKEN;
                    if ((getTokenType(global_var->token) == TOKEN_TYPE_ASSIGN) && (Tempresult == 3)){
                        if(HTfunct_get_item(global_var->HTfunct, key) != NULL){
                            return ERR_SEMANTIC_NONDEFINED;
                        }
                        HTvar_insert_item(global_var->HTglobalVar, key, true);
                        instDecVar(&(global_var)->generator,"GF",key);
                        
                    } 
                    
                    result = ID1(global_var, "GF", key);
                    if (result != SYNTAX_OK) return result;
                }
            }
            return SYNTAX_OK;
            break;
            
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_LPAR:
            result = parse_exp(global_var);
            return result;
        case TOKEN_TYPE_KEYWORD_IF:

            GET_TOKEN;

            result = parse_exp(global_var);
            if (result != SYNTAX_OK) return result;
            if(getTokenType(global_var->token) != TOKEN_TYPE_COLON){
                return ERR_SYNTACTIC;
            }

            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);
            
            instIf(&(global_var->generator));
            GET_TOKEN;
            result = StatList(global_var);
            if (result != SYNTAX_OK) return result;

            GET_TOKEN_OF_TYPE(TOKEN_TYPE_KEYWORD_ELSE);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_COLON);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);

            instElse(&(global_var->generator));
            GET_TOKEN;
            result = StatList(global_var);
            if (result != SYNTAX_OK) return result;
            
            instIfElseEnd(&(global_var->generator));
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_KEYWORD_WHILE: 

            GET_TOKEN;

            result = parse_exp(global_var);
            if ( result != SYNTAX_OK) return result;
            if (getTokenType(global_var->token) != TOKEN_TYPE_COLON){
                return ERR_SYNTACTIC;
            }
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);

            instWhileStart(&(global_var->generator));

            GET_TOKEN;

            result = StatList(global_var);
            if (result != SYNTAX_OK) return result;
            instWhileEnd(&(global_var->generator));

            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_KEYWORD_PASS:

            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);

            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;

    }
    return ERR_SYNTACTIC;
}

//<par list -n>   	-->     	,id <par list -n>	    //,
//                	-->     	)	                    //)
int ParListN(tGlobalVars global_var, long unsigned int *NumOfParams ){
	fprintf(stderr,"[PARSER]ParListN\n");

    int result;
    switch(getTokenType(global_var->token)){
        //,id <par list -n>	    //,
        case TOKEN_TYPE_COMMA:
            // zistenie či je id func alebo var
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_ID);
            
            instFunctionDefParam(&(global_var->generator),getStrData(getTokenAttribute(global_var->token)), *NumOfParams);
            
            if (HTvar_get_item(global_var->HTlocalVar,getStrData(getTokenAttribute(global_var->token))) == NULL){
                HTvar_insert_item(global_var->HTlocalVar,getStrData(getTokenAttribute(global_var->token)), true); 
            } else {
                return ERR_SEMANTIC_NONDEFINED;
            }
            GET_TOKEN;
            (*NumOfParams)++;
            result = ParListN(global_var, NumOfParams);
            if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;
            break;
        //)	                    //)
        case TOKEN_TYPE_RPAR:
             
            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;
    }
    return ERR_SYNTACTIC;
}

//<par list>      	-->     	id <par list -n>    	//id
//                	-->     	<par list -n>       	//,)
int ParList(tGlobalVars global_var,char *key, long unsigned int *NumOfParams){
	fprintf(stderr,"[PARSER]ParList\n");

    int result;

    switch(getTokenType(global_var->token)){
        //id <par list -n>    	//id
        case TOKEN_TYPE_ID:
            if (HTvar_get_item(global_var->HTlocalVar,key) == NULL){
                HTvar_insert_item(global_var->HTlocalVar,key, true); 
            } else {
                return ERR_SEMANTIC_NONDEFINED;
            }
                 
            instFunctionDefParam(&(global_var->generator), key , *NumOfParams);
            GET_TOKEN;
            (*NumOfParams)++;
            result = ParListN(global_var, NumOfParams);
            if (result != SYNTAX_OK) return result;
            
            return SYNTAX_OK;
            break;
        //<par list -n>       	//,)
        //case TOKEN_TYPE_COMMA:
        case TOKEN_TYPE_RPAR:

            result = ParListN(global_var, NumOfParams);
            if (result != SYNTAX_OK) return result;
            return SYNTAX_OK;
            break; 
        default:
            return ERR_SYNTACTIC;
        
    }
    return ERR_SYNTACTIC;
}

//<stat list>     	-->     	<stat><stat list>	  //int, float, string, none, id, (, if, while, pass
//              	-->     	}EOL	              //}
int StatList(tGlobalVars global_var){
	fprintf(stderr,"[PARSER]StatList\n");

    int result;
    switch(getTokenType(global_var->token)){
        //<stat><stat list>	  //int, float, string, none, id, (, if, while, pass
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_ID:
        case TOKEN_TYPE_LPAR: 
        case TOKEN_TYPE_KEYWORD_IF:
        case TOKEN_TYPE_KEYWORD_WHILE:
        case TOKEN_TYPE_KEYWORD_PASS:
       
            result = Stat(global_var);
            if (result != SYNTAX_OK) return result;
            GET_TOKEN;
            result = StatList(global_var);
            if (result != SYNTAX_OK) return result;
   
            return SYNTAX_OK;
            break;

        //}EOL	              //}
        case TOKEN_TYPE_DEDENT:
            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;
    }
    return ERR_SYNTACTIC;
}

//<stat list def>     	-->     	<stat><stat list>	  //int, float, string, none, id, (, if, while, return, pass
//              	-->     	}EOL	              //}
int StatListDef(tGlobalVars global_var){
	fprintf(stderr,"[PARSER]StatListDef\n");

    int result;
    switch(getTokenType(global_var->token)){
        //<stat><stat list>	  //int, float, string, none, id, (, if, while, return, pass
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_ID:
        case TOKEN_TYPE_LPAR: 
        case TOKEN_TYPE_KEYWORD_IF:
        case TOKEN_TYPE_KEYWORD_WHILE:
        case TOKEN_TYPE_KEYWORD_RETURN:
        case TOKEN_TYPE_KEYWORD_PASS:
       
            result = StatDef(global_var);
            if (result != SYNTAX_OK) return result;
            GET_TOKEN;
            result = StatListDef(global_var);
            if (result != SYNTAX_OK) return result;
   
            return SYNTAX_OK;
            break;

        //}EOL	              //}
        case TOKEN_TYPE_DEDENT:

            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;
    }
    return ERR_SYNTACTIC;
}

//<f.def>      -->     	def id (<par list> : EOL {<stat list>	 //def
int FDef(tGlobalVars global_var){
	fprintf(stderr,"[PARSER]FDef\n");

    int result = 0;

    switch(getTokenType(global_var->token)){
        //def id (<par list> : EOL {<stat list>	 //def
        case TOKEN_TYPE_KEYWORD_DEF:

            GET_TOKEN_OF_TYPE(TOKEN_TYPE_ID);        

            if( HTfunct_get_item(global_var->HTfunct, getStrData(getTokenAttribute(global_var->token))) != NULL ){
                return ERR_SEMANTIC_NONDEFINED;
            }

            char *key = calloc(strlen(getStrData(getTokenAttribute(global_var->token))), 1);
            long unsigned int NumOfParams = 1;
            strcpy(key, getStrData(getTokenAttribute(global_var->token)));
                    
            
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_LPAR);
            
            instFunctionDefStart(&(global_var->generator), key);                    
            GET_TOKEN;
            result = ParList(global_var,getStrData(getTokenAttribute(global_var->token)), &NumOfParams );
            if (result != SYNTAX_OK){
                return result;
            }
            NumOfParams--;
            result = HTfunct_insert_item( global_var->HTfunct, key, true, NumOfParams);
            if (result != SYNTAX_OK){
                return result;
            }

            GET_TOKEN_OF_TYPE(TOKEN_TYPE_COLON);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_EOL);
            GET_TOKEN_OF_TYPE(TOKEN_TYPE_INDENT);
            GET_TOKEN;

            result = StatListDef(global_var);
            if (result != SYNTAX_OK){
                return result;
            }

            //Generate function end
            instFunctionDefReturn(&(global_var->generator),"");
            instFunctionDefEnd(&(global_var->generator));
            HTvar_delete(global_var->HTlocalVar);

            return SYNTAX_OK;
            break;
        default:
            return ERR_SYNTACTIC;

    }
    return ERR_SYNTACTIC;

}


//<prog>     -->     	<f.def><prog>	    //def
//           -->     	<stat><prog>    	//int, float, string, none, id, (, if, while, return, pass
int program(tGlobalVars global_var){
	fprintf(stderr,"[PARSER]program\n");

    int result;
    fprintf(stderr,"%d+++++++++++\n",getTokenType(global_var->token));
    switch(getTokenType(global_var->token)){
        //<f.def><prog>	//def
        case TOKEN_TYPE_KEYWORD_DEF:
            result = FDef(global_var);
            if (result != 0){
                return result;
            }
            fprintf(stderr,"fdefok\n");
            fprintf(stderr,"%d+++++++++++\n",getTokenType(global_var->token));
            GET_TOKEN;
            result = program(global_var);
                if (result != SYNTAX_OK) return result;
      
        return SYNTAX_OK;
        break;
        //<stat><prog>	//int, float, string, none, id, (, if, while, return, pass
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STR:
        case TOKEN_TYPE_KEYWORD_NONE:
        case TOKEN_TYPE_ID:
        case TOKEN_TYPE_LPAR: 
        case TOKEN_TYPE_KEYWORD_IF:
        case TOKEN_TYPE_KEYWORD_WHILE:
        case TOKEN_TYPE_KEYWORD_RETURN:
        case TOKEN_TYPE_KEYWORD_PASS:
            result = Stat(global_var);
                if (result != SYNTAX_OK) return result;
            
            GET_TOKEN;
            
            result = program(global_var);
                if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_EOF:
            return SYNTAX_OK;
            break;
        case TOKEN_TYPE_EOL:
            GET_TOKEN;
            result = program(global_var);
            if (result != SYNTAX_OK) return result;
            return SYNTAX_OK;

        default:
            return ERR_SYNTACTIC;
    

    }
    return ERR_SYNTACTIC;
}

int parse(tGlobalVars global_var){
    int result;

    GET_TOKEN;
    result = program(global_var);

    return result;
}
