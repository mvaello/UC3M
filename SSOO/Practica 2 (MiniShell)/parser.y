%{
/*-
 * parse.y
 * Copyright (c) 1993-1999, Francisco Rosales <frosal@fi.upm.es>
 *
 * Minishell "yacc" source
 * Describes valid input grammar
 * Exports "obtain_order" input interface function
 *
 * DO NOT MODIFY THIS FILE
 */

static char *** argvv;
static int argvc;
static int argc;
static char * filev[3];
static int bg;
static void pipeline();
static void command(char *arg);
%}

%start minis
%token TXT
%right '|'
%union { char * txt; }
%type <txt> TXT

%%
minis	:			{ return(0); }
	| line fin		{ return(argvc+1); }
	| error fin		{ yyerrok; return(-1); }
	;

line	:
	| pipel redir backg
	;

pipel	: comma			{ pipeline(); }
	| pipel '|' comma	{ pipeline(); }
	;

comma	: TXT			{ command($1); }
	| comma TXT		{ command($2); }
	;

redir	:
	| '<' TXT redir		{ if(filev[0]) YYERROR; filev[0] = $2; }
	| '>' TXT redir		{ if(filev[1]) YYERROR; filev[1] = $2; }
	| '>' '&' TXT redir	{ if(filev[2]) YYERROR; filev[2] = $3; }
	;

backg	:
	| '&'			{ bg = 1; }
	;

fin	: '\n'
	;
%%

#include <stddef.h>			/* NULL */
#include <stdlib.h>			/* malloc */

#define zfree(p) { if(p) { free(p); p = NULL; } }
#define zfreevv(ppp) { if(ppp) { freevv(ppp); ppp = NULL; } }

static void freevv(char ***ppp)
{
	char **pp;
	for (; ppp && *ppp; ppp++)
		for (pp = *ppp; pp && *pp; pp++)
			zfree(*pp);
}

static void pipeline()
{
	argvc++;
}

#define argv argvv[argvc]

static void command(char *arg)
{
	if (!argvv) {
		argvc = 0;
		argvv = calloc(1, sizeof(char **));
	}
	if (!argv) {
		argvv = realloc(argvv, (argvc + 2) * sizeof(char **));
		argvv[argvc + 1] = NULL;
		argc = 0;
		argv = calloc(1, sizeof(char *));
	}
	argv = realloc(argv, (argc + 2) * sizeof(char *));
	argv[argc++] = arg;
	argv[argc] = NULL;
}

/* 
 * Input interface function
 * Returns:
 *   -1, at syntax error
 *    0, at end of file on input
 *   >0, number of commands in the pipeline + 1
 */
int obtain_order(char ****argvvp, char *filep[3], int *bgp)
/* argvvp is reference of a NULL terminated array of argvs */
/* filep is reference to an array of 3 char* */
/* bgp is reference to an integer */
{
	int ret;

	zfreevv(argvv);
	argvc = 0;
	zfree(filev[0]);
	zfree(filev[1]);
	zfree(filev[2]);
	bg = 0;

	ret = yyparse();

	*argvvp = argvv;
	filep[0] = filev[0];
	filep[1] = filev[1];
	filep[2] = filev[2];
	*bgp = bg;

	return ret;
}
