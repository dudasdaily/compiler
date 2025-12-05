%{
#include "exp.tab.h"
%}
%%
[0-9]+			|
[0-9]+\.[0-9]*		{
				yylval.dval=atof(yytext);
				return(TNUMBER);
			}
[a-zA-Z][a-zA-Z0-9]*	{
				yylval.sval=strdup(yytext);
				return(TNAME);
			}
[ \t\n]		;
\+		return('+');
\-		return('-');
\*		return('*');
\/		return('/');
\(		return('(');
\)		return(')');
\,		return(',');
.		{ printf("'%c': illegal character\n", yytext[0]); 
		  exit(-1); }