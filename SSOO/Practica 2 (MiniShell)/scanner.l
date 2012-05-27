%{
/*-
 * scanner.l
 * Copyright (c) 1993-1999, Francisco Rosales <frosal@fi.upm.es>
 *
 * Minishell "lex" source
 * Describes valid input lexic
 *
 * DO NOT MODIFY THIS FILE
 */

#include <unistd.h>     /* isatty, write */
#include <string.h>     /* strdup */
#include "y.tab.h"

static int onc = 1;
static int bat = 0;
#define echo() (bat||(onc&&onc--&&(bat=!isatty(0))))&&write(2,yytext,yyleng)

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) { \
	int c = getchar(); \
	result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
}
%}

%%
[ \t]+		{ echo(); }
[|<>&\n]	{ echo(); return *yytext; }
[^ \t|<>&\n]+	{ echo(); yylval.txt = strdup(yytext); return TXT; }
%%
