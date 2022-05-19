/*
 * Created on Sat Oct 26 2019
 *
 * Copyright (c) 2019 Kristina Hostacna, Juraj Sloboda
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file parser.h
 * @author Kristina Hostacna
 * @brief 
 */

#ifndef _PARSER_H_
#define _PARSER_H_

	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>
	#include "error.h"
	#include "codeGenerator.h"
	#include "scanner.h"
	#include "global_var.h"
	#include "stackToken.h"
	

	#define SYNTAX_OK     	0

	#define MAX_HTSIZE 		101

	//////////////////////////////////    General    //////////////////////////////////////////////////////////////


/**
* @brief Syntax control of ifj19
*
* @param global_var global structure for everything
* @return Returns 0 on success or number of error on fail (depends on the type of error)
*/
	int parse(tGlobalVars global_var);
	
/**
* @brief Parser for expressions
*
* @param global_var global structure for everything
* @return Returns 0 on success or number of error on fail (depends on the type of error)
*/
	int parse_exp(tGlobalVars global_var);

#endif
