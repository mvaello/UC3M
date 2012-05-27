/*-
 * y.c
 * Copyright (c) 1993-1999, Francisco Rosales <frosal@fi.upm.es>
 *
 * Minishell "C" source 
 * Exports basic lex functions
 *
 * DO NOT MODIFY THIS FILE
 */

#include <stdio.h>

void yyerror(char *s)
{
	fprintf(stderr, "%s\n", s);
}

#undef yywrap

int yywrap()
{
	return 1;
}
