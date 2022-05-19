/*
 * Created on Sun Oct 27 2019
 *
 * Copyright (c) 2019 Kristina Hostacna
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file codeGenerator.h
 * @author Kristina Hostacna
 * @brief 
 */

#ifndef _CODEGENERATOR_H_
#define _CODEGENERATOR_H_

#include <stdbool.h>

#include "dynamicString.h"
//#include "symtable.h"

#define INST_QUEUE_CHUNK_SIZE 16

	//////////////////////////////////    Code generator    //////////////////////////////////////////////////////////////
	
	struct codeGenerator;
	typedef struct codeGenerator* tCodeGenerator;
	

	struct instructionQueue;
	typedef struct instructionQueue* tInstQueue;

	tCodeGenerator codeGeneratorInit();
	void generateInstructions(tCodeGenerator *generator);


	#pragma region instructions

	#pragma region variables
	/**
	 * @brief Declares a variable
	 * 
	 * @param generator 
	 * @param scope Scope of the variable, LF, GF, or TF
	 * @param varName Name of the variable
	 */
	void instDecVar(tCodeGenerator *generator, char* scope, char* varName);

	/**
	 * @brief assigns to a variable. Will also work if type is a scope and value a name of a variable
	 * 
	 * @param generator 
	 * @param scope Scope of the variable, LF, GF, or TF
	 * @param varName Name of the variable
	 * @param type type of the passed value
	 * @param value the value to be assigned
	 */
	void instAssign(tCodeGenerator *generator, char* scope, char* varName, char* type, char* value);

	/**
	 * @brief Pops to a variable from data stack
	 * 
	 * @param generator 
	 * @param scope Scope of the variable, LF, GF, or TF
	 * @param varName Name of the variable
	 */
	void instAssignFromStack(tCodeGenerator *generator, char* scope, char* varName);

	void instDecVarAndAssign(tCodeGenerator *generator, char* scope, char* varName, char* type, char* value);

	#pragma endregion

	#pragma if-else
	
	/**
	 * @brief Should be called at the start of the if part of an if-else block, but AFTER the the condition was evaluated (and pushed to data stack)
	 * 
	 * @param generator 
	 */
	void instIf(tCodeGenerator *generator);

	/**
	 * @brief Called at the start of the else part of an if-else block
	 * 
	 * @param generator 
	 */
	void instElse(tCodeGenerator *generator);

	/**
	 * @brief Should be called at the end of an if-else block
	 * 
	 * @param generator 
	 */
	void instIfElseEnd(tCodeGenerator *generator);

	#pragma endregion

	#pragma region while

	/**
	 * @brief Should be called at the start of while but AFTER the condition was evaluated for the first time (and pushed to stack)
	 * 
	 * @param generator 
	 */
	void instWhileStart(tCodeGenerator *generator);

	/**
	 * @brief Should be called at the end of while block, but AFTER the condition was evaluated again, (and pushed to stack);
	 * 
	 * @param generator 
	 */
	void instWhileEnd(tCodeGenerator *generator);
	#pragma endregion

	#pragma region expressions

	void instExpressionFromPostfix(tCodeGenerator *generator, char* postfix);

	void instPush(tCodeGenerator *generator, char* prefix, char* value);
	#pragma endregion

	#pragma region functions

	/**
	 * @brief Should be called at the start of every function definition.
	 * 
	 * @param generator 
	 * @param name Name of the function
	 */
	void instFunctionDefStart(tCodeGenerator *generator, char* name);

	/**
	 * @brief Called for each parameter of the function being generated
	 * 
	 * @param generator 
	 * @param paramName Name of the parameter
	 * @param paramNum Index of the parameter, strating at 1
	 */
	void instFunctionDefParam(tCodeGenerator *generator, char* paramName, size_t paramNum);

	/**
	 * @brief Generates a return. Can be called multiple times per function. Use this function when assigning return value manually.
	 * 
	 * @param generator 
	 * @param returnValue the value/variable that should be moved to %retvalue, ie to be returned by the function. Can be empty.
	 */
	void instFunctionDefReturn(tCodeGenerator *generator, char* returnValue);

	/**
	 * @brief Generates return. Can be called multiple times per function. Uses top of data stack as return value.
	 * 
	 * @param generator 
	 */
	void instFunctionDefReturnFromStack(tCodeGenerator *generator);

	/**
	 * @brief Needs to be called at the end of function definition
	 * 
	 * @param generator 
	 */
	void instFunctionDefEnd(tCodeGenerator *generator);

	/**
	 * @brief Needs to be called at the start of calling a function
	 * 
	 * @param generator 
	 */
	void instCallFunctionStart(tCodeGenerator *generator, char* name);

	/**
	 * @brief Called to pass each parameter of the function being called
	 * 
	 * @param generator 
	 * @param prefix Type of constant of scope of symbol
	 * @param paramValue Value to pass (or name of symbol)
	 * @param paramNum Index of the parameter, strating at 1
	 */
	void instCallFunctionParam(tCodeGenerator *generator, char* prefix, char *paramValue, size_t paramNum);

	/**
	 * @brief Needs to be called after passing all parameters when calling a function
	 * 
	 * @param generator 
	 * @param name Name of the function to be called
	 */
	void instCallFunctionEnd(tCodeGenerator *generator, char* name);

	void instGetReturnVal(tCodeGenerator *generator, char* scope, char* name);

	#pragma endregion
	
	#pragma endregion

	unsigned GetOrderOfInteger(int a);
#endif
