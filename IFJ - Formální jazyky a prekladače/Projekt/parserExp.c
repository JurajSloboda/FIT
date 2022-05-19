/*
 * Created on Sun Oct 27 2019
 *
 * Copyright (c) 2019 Kristina Hostacna
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file parserExp.c
 * @author Kristina Hostacna, xhosta05
 * @brief Expression parser-syn. analysis 
 */

#include <string.h>
#include "parser.h"
#include "symtable.h"
#include "dynamicString.h"
#include "stackToken.h"

#define prec_table_size 21		//?
	#define i_plus			TOKEN_TYPE_ADD % prec_table_size
	#define i_minus			TOKEN_TYPE_SUB % prec_table_size
	#define i_mul			TOKEN_TYPE_MUL % prec_table_size
	#define	i_div			TOKEN_TYPE_DIV % prec_table_size
	#define	i_idiv			TOKEN_TYPE_IDIV % prec_table_size
		
	#define	i_equal			TOKEN_TYPE_EQ % prec_table_size
	#define	i_notequal		TOKEN_TYPE_NEQ % prec_table_size
	#define	i_less			TOKEN_TYPE_LTN % prec_table_size
	#define	i_less_equal	TOKEN_TYPE_LEQ % prec_table_size
	#define	i_greater		TOKEN_TYPE_GTN % prec_table_size
	#define	i_greater_equal TOKEN_TYPE_GEQ % prec_table_size
		
	#define	i_c_none		TOKEN_TYPE_KEYWORD_NONE % prec_table_size
	#define	i_c_int			TOKEN_TYPE_INT % prec_table_size
	#define	i_c_float		TOKEN_TYPE_FLOAT % prec_table_size
	#define	i_c_string		TOKEN_TYPE_STR % prec_table_size		
	#define	i_id			TOKEN_TYPE_ID % prec_table_size

	#define	i_lpar			TOKEN_TYPE_LPAR % prec_table_size
	#define	i_rpar			TOKEN_TYPE_RPAR % prec_table_size
	#define	i_eol			TOKEN_TYPE_EOL % prec_table_size
	#define	i_colon			TOKEN_TYPE_COLON % prec_table_size

	#define	i_E				TOKEN_TYPE_KEYWORD_PASS % prec_table_size 	//nonTerminal- random tokenType; not in table

	//i_eof=;
	//merge: logic ops, mul+div+idiv, plus+minus, id and constants, EOL and colon?
	//index=token_type mod prec_table_size

#define is_constant_or_id(myint) (\
	myint==i_c_none || \
	myint==i_c_int || \
	myint==i_c_float || \
	myint==i_c_string || \
	myint==i_id || \
	myint==i_E)	//nonTerminal
#define is_relation_operator(myint) (\
	myint==i_equal || \
	myint==i_notequal || \
	myint==i_less || \
	myint==i_less_equal || \
	myint==i_greater || \
	myint==i_greater_equal )
#define is_arithmetic_operator(myint) (\
	myint==i_plus || \
	myint==i_minus || \
	myint==i_mul || \
	myint==i_div || \
	myint==i_idiv)
#define is_another_operator(myint) (\
	myint==i_lpar || \
	myint==i_rpar || \
	myint==i_eol || \
	myint==i_colon)
#define is_operator(myint) (\
	is_relation_operator(myint) ||\
	is_arithmetic_operator(myint) ||\
	is_another_operator(myint))

/////////////////////////////////////////////////////   Converting rules to postfix   //////////////////////////////////////////////////////
void printTStack(tTStack pushdown);
void print_table(int arr[prec_table_size][prec_table_size]);

int tokenToIndex(tToken token){

	return (getTokenType(token) % prec_table_size);
}
int numOfTokensUntilEmptyToken(tTStack pushdown){
	int i=1;
	tStackTElem temp;
	if(stackTEmpty(pushdown)==0){
		do{
					//fprintf(stderr, "check= %d\n token= %d:\n", i, getTokenType(stackGet(pushdown,i)));
			temp= stackGet (pushdown,i);	
			if(getTokenType(temp) == TOKEN_TYPE_EOL){	//no more empty barriers in stack
				if (i==2){
					return 1;
				}
				else{
					return ERR_SYNTACTIC;
				}
			}
			i++;
		}while(getTokenType(temp) != TOKEN_TYPE_EMPTY);
	}
	return i-2;
}
//-1 return value not used
void appendToOutput(tGlobalVars global_var, tDynamicString* output, tStackTElem token){
	tDynamicString tAttribute=getTokenAttribute(token);
	tTokenType tType=getTokenType(token);

	tDynamicString adding=initDynamicString(100);

  	if(!is_relation_operator(tokenToIndex(token)) && !is_arithmetic_operator(tokenToIndex(token))){
	    switch(tType){											//putting type_of_term@ to adding
	     	case(TOKEN_TYPE_INT):
	     		setDynamicString(&adding, "int@");
	      		break;
	     	case(TOKEN_TYPE_FLOAT):
	     		setDynamicString(&adding, "float@");
	      		break;
	     	case(TOKEN_TYPE_KEYWORD_NONE):
	     		setDynamicString(&adding, "nil@nil");
	      		break;
	      	case (TOKEN_TYPE_STR):
	     		setDynamicString(&adding, "string@");
	      		break;
	     	case (TOKEN_TYPE_ID):
	      		if (HTvar_get_item(global_var->HTlocalVar,getStrData(tAttribute)) != NULL) {
	     			setDynamicString(&adding, "LF@");
	      		} 
	      		else if(HTvar_get_item(global_var->HTglobalVar,getStrData(tAttribute)) != NULL){
	     			setDynamicString(&adding, "GF@");
	      		}
	      		break;

	      	default:
	      		setDynamicString(&adding, "Error_in_appendToOutput");
	    }
	}

	if (tType==TOKEN_TYPE_FLOAT){
		char* tempstr=malloc(300*sizeof(char));	///really? iyeofchar??
	    strcpy(tempstr,getStrData(tAttribute));	///lets hope its not longer

	    float fl = strtof(tempstr, NULL);
		sprintf(tempstr,"%a", fl);
		setDynamicString(&tAttribute, tempstr);
	}

	for (unsigned int i = 0; i < getStrLen(tAttribute); ++i){		//putting attribute to adding
		if(tType==TOKEN_TYPE_STR){							//string formated with special characters
	    	if (getStrData(tAttribute)[i]=='\n'){
	    		//\010='\n'
	    		appendToString(&adding,'\\');
	    		appendToString(&adding,'0');
	    		appendToString(&adding,'1');
	    		appendToString(&adding,'0');
	    	}
	    	else if(getStrData(tAttribute)[i]==' '){
	    		//\032=' '
	    		appendToString(&adding,'\\');
	    		appendToString(&adding,'0');
	    		appendToString(&adding,'3');
	    		appendToString(&adding,'2');
	    	}
	    	else if(getStrData(tAttribute)[i]=='#'){
	    		//\035='#' 
	    		appendToString(&adding,'\\');
	    		appendToString(&adding,'0');
	    		appendToString(&adding,'3');
	    		appendToString(&adding,'5');
	    	}
	    	else if(getStrData(tAttribute)[i]=='\\'){
	    		//\092='\'      
	    		appendToString(&adding,'\\');
	    		appendToString(&adding,'0');
	    		appendToString(&adding,'9');
	    		appendToString(&adding,'2');
	    	}
	    	else{
				appendToString(&adding,getStrData(tAttribute)[i]);
	    	}
		}
		else if(tType==TOKEN_TYPE_KEYWORD_NONE){}			//none done
		else{												//float,int & id
	   		appendToString(&adding, getStrData(tAttribute)[i]);
		}
	}
    unsigned int i = 0;
    while (i < getStrLen(adding)){
		appendToString(output,getStrData(adding)[i]);
		i++;
	}
	//clearDynamicString(adding);
	appendToString(output,' ');
}
int generatePostfix(tGlobalVars global_var,tStackTElem E,tDynamicString* output,tTStack pushdown){
	int check=numOfTokensUntilEmptyToken(pushdown);

	if (check==3){
		tStackTElem temp,temp2;
		for (int i = 3; i > 0; --i)	{ //put last 3 tokens to output in correct order
			temp= stackGet (pushdown,i);									//poping last 3 items from stack 
			if (is_operator(tokenToIndex(temp)) && getTokenType(temp)!= TOKEN_TYPE_KEYWORD_PASS){
				if (i==2){
					temp2=temp;												//saving operator for later (has to be in 2nd place)
				}
				else {
					fprintf(stderr,"operator %d found in unexpected place (in function generatePostfix)\n",getTokenType(temp));//getStrData(getTokenAttribute(temp)));
				}
			}
			else if(is_constant_or_id(tokenToIndex(temp))){
				if (i==1 || i==3){
					if (tokenToIndex(temp)!= tokenToIndex(E)){
						appendToOutput(global_var,output, temp);			//appending constants/IDs
					}
				}
				else {
					fprintf(stderr,"operand %s found in unexpected place (in function generatePostfix)\n",getStrData(getTokenAttribute(temp)));
				}
			}
		}
		appendToOutput(global_var,output, temp2);							//appending operator
		stackTPop ( pushdown);												//delete 1st operand
		stackTPop ( pushdown);												//delete 1st operator
		stackTPop ( pushdown);												//delete 2nd operand
		stackTPop ( pushdown);												//delete empty barrier
		stackTPush ( pushdown,E );											//push nonterminal
		return 0;
	}
	else if(check==1){
		appendToOutput(global_var,output, stackTPop (pushdown ));
		stackTPush ( pushdown,E );											//push nonterminal
	}
	else {
		return ERR_SYNTACTIC;
	}
	return 0;
}
void deleteParentheses(tTStack s){
	tStackTElem temp;
	stackTPop ( s );		//delete RPAR
	temp= stackTPop ( s );	//temp save E
	stackTPop ( s );		//delete LPAR
	stackTPush ( s, temp );	//return E
}
int rulesPrecedence(tGlobalVars global_var,tStackTElem E,tDynamicString* output,tTStack pushdown){	//calls tokenToIndex,generatePostfix,deleteParentheses
	int res=0;
	if(getTokenType(stackTTop (pushdown))==TOKEN_TYPE_RPAR &&\
	   getTokenType(stackGet(pushdown,3))==TOKEN_TYPE_LPAR){
		//4		E->(E)
		deleteParentheses(pushdown);
		return 0;
	}
	else if(getTokenType(stackGet(pushdown,2))==TOKEN_TYPE_MUL ||\
			getTokenType(stackGet(pushdown,2))==TOKEN_TYPE_DIV ||\
			getTokenType(stackGet(pushdown,2))==TOKEN_TYPE_IDIV){
		//3		E->E*E,  E->E/E,  E->E//E
		res= generatePostfix(global_var,E,output,pushdown);
		if (res!=0)	{
			return res;
		}
		
	}
	else if(getTokenType(stackGet(pushdown,2))==TOKEN_TYPE_ADD ||\
			getTokenType(stackGet(pushdown,2))==TOKEN_TYPE_SUB){
		//2		E->E-E,  E->E+E 
		res= generatePostfix(global_var,E,output,pushdown);
		if (res!=0)	{
			return res;
		}
	}
	else if (is_relation_operator(tokenToIndex(stackGet ( pushdown, 2)))){
		//1 	E-> E==E, E-> E!=E, E-> E<=E, E-> E>=E, E-> E<E, E-> E>E
		res= generatePostfix(global_var,E,output,pushdown);
		if (res!=0)	{
			return res;
		}
	}
	else if(is_constant_or_id(tokenToIndex(stackTTop(pushdown))) && \
			tokenToIndex(stackGet(pushdown,2))== i_eol){
		//if  there is only one int/float/string in expression
		//5	 	E->i
		res= generatePostfix(global_var,E,output,pushdown);
		if (res!=0)	{
			return res;
		}
	}
	else{
		fprintf(stderr, "Neviem vygenerovat pravidlo z tohto bordelu\n 			S laskou, funkcia rulesPrecedence v parserExp.c");
		return ERR_SYNTACTIC;
	}
	return 0;
}
//////////////////////////////////////////////////////		  Analyzing output		////////////////////////////////////////////////////////////

void init_prec_table(int arr[prec_table_size][prec_table_size]) {
	// 0='' 1='<' 2='>' 3='='
	for (int i=0; i<prec_table_size-1; ++i){
		for (int j=0; j<prec_table_size-1; ++j){
			if (is_arithmetic_operator(i)){
				if ((i==i_plus || i==i_minus) && (j==i_mul || j==i_div || j==i_idiv))	{					
					arr[i][j]=1;	
				}
				else if ((is_constant_or_id(j) && j!=i_E)  || j==i_lpar){
					arr[i][j]=1;
				}
				else{
					arr[i][j]=2;
				}
			}
			else if (is_relation_operator(i)){
				if (is_relation_operator(j)){
					arr[i][j]=0;
				}
				else if (j==i_colon || j==i_eol || j==i_rpar){
					arr[i][j]=2;
				}
				else{
					arr[i][j]=1;
				}
			}
			else if (is_constant_or_id(i) || i==i_rpar){
				if ((is_constant_or_id(j) && j!=i_E)  ||  j==i_lpar){
					arr[i][j]=0;
				}
				else{
					arr[i][j]=2;
				}
			}
			else if (i==i_lpar || i==i_eol){					// || i==i_colon){		//nah...
				arr[i][j]=1;
			}
		}
	}
	arr[i_lpar][i_rpar]=3;
	arr[i_lpar][i_eol]=0;
	arr[i_lpar][i_colon]=0;

	arr[i_eol][i_rpar]=0;			//this eol is a barrier on pushdown

	arr[i_eol][i_eol]=2;
	arr[i_eol][i_colon]=2;
	arr[i_colon][i_colon]=0;	
}
tStackTElem firstOperatorToken(tTStack pushdown){	//calls tokenToIndex
	int i=1;
	tStackTElem temp;
	if(stackTEmpty(pushdown)==0){
		do{
			temp= stackGet (pushdown,i);	
			/*if(tokenToIndex(temp)==i_eol && tokenToIndex(stackTTop(pushdown))!=i_eol){	
				//last token in stack is not operator
				return NULL;
			}*/
			i++;
		}while(!is_operator(tokenToIndex(temp)) || getTokenType(temp)==TOKEN_TYPE_KEYWORD_PASS);
					//^find operator 								//^ignore E
	}
	return temp;
}
int isValidExpToken(tGlobalVars global_var, tStackTElem* b,int *bIndex){
	if (is_operator(tokenToIndex(global_var->token)) || (is_constant_or_id(tokenToIndex(global_var->token))))	{
		if (tokenToIndex(global_var->token)==i_id){
			if(HTvar_get_item(global_var->HTlocalVar, getStrData(getTokenAttribute(global_var->token))) == NULL &&\
			   HTvar_get_item(global_var->HTglobalVar, getStrData(getTokenAttribute(global_var->token))) == NULL ){
				fprintf(stderr, "nedfinovana premenna %s\n",getStrData(getTokenAttribute(global_var->token)) );
				return ERR_SEMANTIC_NONDEFINED;								//id is not defined
			}
		}
		setTokenType(*b, getTokenType(global_var->token));
		setTokenAttribute(*b, getTokenAttribute(global_var->token));
		*bIndex=tokenToIndex(*b);
		return 0;
	}
	else{
		return ERR_SYNTACTIC;							//input token should not be in expression
	}
}
int parse_exp(tGlobalVars global_var){				//calls init_prec_table,firstOperatorToken,isValidExpToken,rulesPrecedence
	fprintf(stderr, "*****************PARSEREXP****************\n" );
	tDynamicString output=initDynamicString(3000);

	int prec_table[prec_table_size][prec_table_size];
	init_prec_table(prec_table);						//3* general?

	tTStack pushdown;
	stackTInit ( &pushdown );

	tStackTElem eolStackBarrier=initToken();
	setTokenType(eolStackBarrier, TOKEN_TYPE_EOL);
	stackTPush(pushdown, eolStackBarrier);				// barrier on the stack;

	tStackTElem E=initToken();
	setTokenType(E, TOKEN_TYPE_KEYWORD_PASS);

	tStackTElem dummyStackBarrier=initToken();
	setTokenType(dummyStackBarrier, TOKEN_TYPE_EMPTY);	//functions as '<' on the pushdown (to avoid confusion with logic operator less than) 

	tStackTElem	a;
	int aIndex;
	int bIndex;

	bool success=false;

	while(success==false){

		a = firstOperatorToken(pushdown);				//the topmost operator on the stack
		if(getTokenType(a)==TOKEN_TYPE_KEYWORD_PASS){
			aIndex=i_c_none;
		}
		else{
			aIndex=tokenToIndex(a);
		}
				
		tStackTElem b;
		b=initToken();
		int bCheck=isValidExpToken(global_var, &b, &bIndex);
		if (bCheck != 0){
			return bCheck;
		}
		int temp_err;		
		switch (prec_table[aIndex][bIndex]) {
			//0='' 1='<' 2='>' 3='='
			case(3) : 									//=
				fprintf(stderr, "case [%d][%d] precedencna tabulka vyplula %d(=)\n",aIndex,bIndex, prec_table[aIndex][bIndex]);
				stackTPush(pushdown,b);
				temp_err=getNextToken(global_var->token,  global_var->scannerIndentStack, global_var->input);
				if(temp_err != 0){
					return temp_err;
				}					
				break;
			case(1) : 									//<
				fprintf(stderr, "case [%d][%d] precedencna tabulka vyplula %d(<)\n",aIndex,bIndex, prec_table[aIndex][bIndex]);
				if(tokenToIndex(b)==i_lpar || tokenToIndex(b)==i_rpar ){
				} 
				else if(is_operator(tokenToIndex(b)) && tokenToIndex(b)!=i_eol && tokenToIndex(b)!=i_colon){
					tToken temp;					
					if(is_constant_or_id(tokenToIndex(stackTTop(pushdown)))){
						temp= stackTPop (pushdown);				//temp save i/E

						stackTPush(pushdown,dummyStackBarrier);	
						stackTPush(pushdown,temp);				//replace i with <i on the pushdown
					}
					else {
						fprintf(stderr, "neco neslo podle planu- pokus dat '<' barieru pred token, kt. nieje const/id/E\n");
					}
				}
				stackTPush(pushdown,b); 
				temp_err=getNextToken(global_var->token,  global_var->scannerIndentStack, global_var->input);
				if(temp_err != 0){
					return temp_err;
				}
				break;
			case(2) :									//>
				fprintf(stderr, "case [%d][%d] precedencna tabulka vyplula %d(>)\n",aIndex,bIndex, prec_table[aIndex][bIndex]);
				temp_err=rulesPrecedence(global_var,E,&output,pushdown);
				if (temp_err==0){
					/*
					<y is the pushdown top string and r: A → y ∈ P \
					then replace <y with A & write r to output)
					*/
				}
				else{
					//deleteTStack(pushdown);
					return temp_err;
				}
				break;
			case(0):									//blank
				fprintf(stderr, "case [%d][%d] precedencna tabulka vyplula %d(' ')\n",aIndex,bIndex, prec_table[aIndex][bIndex]);
				//deleteTStack(pushdown);
				return ERR_SYNTACTIC;
			default:
				fprintf(stderr, "precedencna tabulka vyplula blbost\n");
				break;
		}
		if  ((is_constant_or_id(tokenToIndex(stackTTop(pushdown))) && \
			  tokenToIndex(stackGet(pushdown,2))== i_eol )  &&\
			 (tokenToIndex(b)==i_eol || tokenToIndex(b)==i_colon)){				//only const or E on the stack, (eol or ':') ; 
			success=true;
			fprintf(stderr, "----------------------------------------success parserExp\n" );
		}
	}
	//free(dummyStackBarrier);
	//free(eolStackBarrier);

	if (is_constant_or_id(tokenToIndex(stackTTop(pushdown)))){ 	//check if pushdown has only const/E
		//deleteTStack(pushdown);
		free(E);
	}
	else{
		return ERR_SYNTACTIC;
	}

	instExpressionFromPostfix(&(global_var->generator), getStrData(output));
	return 0;
}

void printTStack(tTStack pushdown){
	fprintf(stderr, "stack:\n" );
	int i=1;
	while(tokenToIndex(stackGet(pushdown,i))!=i_eol){
		fprintf(stderr, "      %d:\n",getTokenType(stackGet(pushdown,i)) );
		i++;
	}
}

void print_table(int arr[prec_table_size][prec_table_size]){
    for (int i = 0; i < prec_table_size-1; i++) {
        for (int j = 0; j < prec_table_size; j++) {
			// 0='' 1='<' 2='>' 3='='
			if(i==prec_table_size-2){
				if (j<9){
            		printf("%d ",j);
				}
				else
					printf("%d",j);
			} 
			else if(j==prec_table_size-1){
            	printf("%d ",i);
			}
			else if(arr[i][j]==0){
            	printf("  ");
			}
			else if (arr[i][j]==1){
            	printf("< ");
			}
			else if (arr[i][j]==2){
            	printf("> ");
			}
			else if (arr[i][j]==3){
            	printf("= ");
			}
			else{
            	printf("Error not initialised");
        	}
        }
        printf("\n");
    }
}

#undef prec_table_size
	#undef 	i_plus
	#undef 	i_minus
	#undef 	i_mul
	#undef 	i_div
	#undef 	i_idiv
		
	#undef	i_equal
	#undef	i_notequal
	#undef	i_less
	#undef	i_less_equal
	#undef	i_greater
	#undef	i_greater_equal

	#undef	i_c_none
	#undef	i_c_int
	#undef	i_c_float
	#undef	i_c_string
	#undef	i_id

	#undef	i_lpar
	#undef	i_rpar
	#undef	i_eol
	#undef	i_colon
	#undef	i_E	

#undef is_relation_operator
#undef is_arithmetic_operator
#undef is_constant_or_id
#undef is_another_operator
#undef is_operator