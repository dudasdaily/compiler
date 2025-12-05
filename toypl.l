%{
#include "toypl.tab.h"
%}
%x          CMT
word        [a-zA-Z]+[a-zA-Z0-9]*
number      -?([1-9]+[0-9]*)|0

%%
"/*"        BEGIN CMT;
<CMT>"*/"\n BEGIN 0;
<CMT>"*/"   BEGIN 0;
<CMT>.      ;
<CMT>\n     ;

program     return TPROGRAM;
proc        return TPROC;
returns     return TRETURNS;
int         return TINT;
if          return TIF;
else        return TELSE;
for         return TFOR;
call        return TCALL;
begin       return TBEGIN;
main        return TMAIN;
func        return TFUNC;
var         return TVAR;
long        return TLONG;
then        return TTHEN;
while       return TWHILE;
to          return TTO;
return      return TRETURN;
end         return TEND;

"="         return TASS;
"&"         return TAND;
"<"         return TLT;
">"         return TGT;
"+"         return TPLUS;
"*"         return TMUL;
"|"         return TOR;
"<="        return TLE;
">="        return TGE;
"-"         return TMINUS;
"/"         return TDIV;
"!"         return TNOT;
"!="        return TNE;

";"         return ';';
":"         return ':';
","         return ',';
"."         return '.';

{word}      {
                yylval.sval = strdup(yytext);
                return TWORD;
            }

{number}    {
                yylval.dval = atof(yytext);
                return TNUMBER;
            }

[ \t\n]     ;

.           { return yytext[0]; }

%%
int yywrap(void) {
    return 1;
}