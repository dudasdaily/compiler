%{
#include <stdio.h>
#include <string.h>
#include "node.h"

int yylex(void);
void yyerror(char const *s);

Node * rootNode;
%}
// 파서가 처리하는 토큰과 논터미널 심볼이 가질 수 있는 데이터 타입들의 집합
%union {
	double dval;
	char * sval;
	struct _node * nodeP; // 노드의 주소
}
%token <dval> TNUMBER // 토큰에 대한 타입 선언
%token <sval> TNAME // 토큰에 대한 타입 선언

%type <nodeP> Fact // Fact라는 논터미널 심볼이 nodeP라는 타입을 가지고, nodeP는 %union에 정의되어 있다
%type <nodeP> Term // 논터미널 심볼에 대한 타입 선언
%type <nodeP> Exp
%type <nodeP> FuncCall
%type <nodeP> ParamList
%type <nodeP> ExprList
%%
Goal	: Exp			{
				  	rootNode = $1;
				}  
	| Rel
	;
Rel	: Exp '<' Exp
	;
Exp 	: Exp '+' Term 		{
					$$=makeNode("PLUS", NULL, $1);
					$1->bro = $3;
				}
	| Exp '-' Term		{
					$$=makeNode("MINUS", NULL, $1);
					$1->bro = $3;
				}
	| Term	               	{	$$ = $1;  }
	;
 
Term 	: Term '*' Fact		{
					$$=makeNode("TIMES", NULL, $1);
					$1->bro = $3;
				}
	| Term '/' Fact		{
					$$=makeNode("DIV", NULL, $1);
					$1->bro = $3;
				}
	| Fact			{ 	$$ = $1; }	
	;

Fact 	: TNAME			{ 
					printf("Name:%s\n", $1); 
					$$=makeNode("NAME", NULL, NULL);
					$$->value.sv = $1;
				}
	| TNUMBER		{ 
					printf("Number:%f\n", $1); 
					$$=makeNode("NUMBER", NULL, NULL);
					$$->value.dv = $1;
				}
	| FuncCall		{ $$=$1;}	
	| '-'Fact		{ 
					$$=makeNode("NEG", NULL, $2);
				}
	| '('Exp')'		{ $$=$2;}
	;
FuncCall : TNAME'('ParamList')' {$$=NULL;}
	;
ParamList: ExprList		{ $$=NULL; }
	|			{ $$=NULL; }
	;
ExprList: ExprList','Exp	{ $$=NULL; }
	| Exp			{ $$=NULL; }
	;
%%

void traverse(Node * nodeP); 

int yylex(void);
void yyerror(char const *s);

int main(void) {
	yyparse();

//  code to print AST
	traverse(rootNode);
	return 0;
}

void yyerror(char const *s) {
   fprintf(stderr, "Error: %s\n", s);
}

void traverse(Node * nodeP) {
	while (nodeP!=NULL) {
		printf("%s\n", nodeP->kind);
		traverse(nodeP->son);
		nodeP=nodeP->bro;
	}
}
