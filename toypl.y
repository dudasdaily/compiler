%{
#include <stdio.h>
#include <string.h>
#include "codegenerate.h"
#include "node.h"
#include "toypl.tab.h"
Node * rootNode;

int yyerror();
int yylex();
void traverse(Node * nodeP); 

%}

%union {
	double dval;
	char * sval;
	struct _node * nodeP; // 노드의 주소
}

%token  TPROGRAM TPROC TRETURNS TINT TIF TFOR TCALL TBEGIN TMAIN TFUNC TVAR TLONG TWHILE TTO TRETURN TEND
%token  TASS TAND TLT TGT TPLUS TMUL TOR TLE TGE TMINUS TDIV TNOT TNE
%token  <sval> TWORD
%token <dval> TNUMBER

// IF (조건) THEN  IF (조건) THEN 문장  ELSE 문장 같은 Shift/Reduce Conflict를 해결하기 위해
// 기본적으로 Conflit 발생 시 Bison은 shift를 선택한다
%nonassoc TTHEN
%nonassoc TELSE

// 논터미널 타입 선언
%type <nodeP> Fact Term Expr Name FuncName ProcName FuncCall ExprList Var Number ParamList
%type <nodeP> Cond Rel
%type <nodeP> Stmt AsgnStmt CompStmt RtrnStmt IfStmt CallStmt ForStmt WhileStmt StmtList
%type <nodeP> Program VarDecl DclList Decl VarList Type
%type <nodeP> SubPgmList SubPgm ProcDecl FuncDecl FormParam

%start  Program
%%
Stmt        : AsgnStmt  { $$ = $1; }
            | IfStmt    { $$ = $1; }
            | WhileStmt { $$ = $1; }
            | ForStmt   { $$ = $1; }
            | CallStmt  { $$ = $1; } 
            | RtrnStmt  { $$ = $1; }
            | CompStmt  { $$ = $1; }
            ;

AsgnStmt    : Var TASS Expr { $$ = makeNode("ASSIGN", NULL, $1);
                $1->bro = $3;
            }
            ;

IfStmt      : TIF '(' Cond ')' TTHEN Stmt { $$ = makeNode("IF", NULL, $3);
                $3->bro = $6;
            }
            | TIF '(' Cond ')' TTHEN Stmt TELSE Stmt {
                $$ = makeNode("IFELSE", NULL, $3);
                $3->bro = $6;
                $6->bro = $8;
            }
            ;

WhileStmt   : TWHILE '(' Cond ')' Stmt { $$ = makeNode("WHILE", NULL, $3);
                $3->bro = $5;
            }
            ;

ForStmt     : TFOR '(' Var TASS Expr TTO Expr ')' Stmt { $$ = makeNode("FOR", NULL, $3);
                $3->bro = $5;
                $5->bro = $7;
                $7->bro = $9;
            }
            ;

CallStmt    : TCALL ProcName '(' ParamList ')' { $$ = makeNode("CALL", NULL, $2);
                $2->bro = $4;
            }
            ;

RtrnStmt    : TRETURN '(' Expr ')' { $$ = makeNode("RETURN", NULL, $3); }
            ;

CompStmt    : TBEGIN StmtList TEND { $$ = makeNode("COMP", NULL, $2); }
            ;

StmtList    : Stmt ';' StmtList {
                $$ = $1;
                $1->bro = $3;
            }
            | Stmt { $$ = $1; }
            ;

Cond        : Cond TAND Rel { $$ = makeNode("AND", NULL, $1);
                $1->bro = $3;
            }
            | Cond TOR Rel { $$ = makeNode("OR", NULL, $1);
                $1->bro = $3;
            }
            | TNOT Rel { $$ = makeNode("NOT", NULL, $2); }
            | Rel { $$ = $1; }
            ;

Rel         : Expr TLT Expr { $$ = makeNode("LT", NULL, $1);
                $1->bro = $3;
            }
            | Expr TLE Expr { $$ = makeNode("LE", NULL, $1);
                $1->bro = $3;
            }
            | Expr TGT Expr { $$ = makeNode("GT", NULL, $1);
                $1->bro = $3;
            }
            | Expr TGE Expr { $$ = makeNode("GE", NULL, $1);
                $1->bro = $3;
            }
            | Expr TASS Expr { $$ = makeNode("EQ", NULL, $1);
                $1->bro = $3;
            }
            | Expr TNE Expr { $$ = makeNode("NE", NULL, $1);
                $1->bro = $3;
            }
            ;

Expr        : Expr TPLUS Term   {
                $$ = makeNode("PLUS", NULL, $1);
                $1->bro = $3;
            }
            | Expr TMINUS Term  {
                $$ = makeNode("MINUS", NULL, $1);
                $1->bro = $3; 
            }
            | Term              { $$ = $1; }
            ;

Term        : Term TMUL Fact    {
                $$ = makeNode("MUL", NULL, $1);
                $1->bro = $3;
                }
            | Term TDIV Fact    { 
                $$ = makeNode("DIV", NULL, $1);
                $1->bro = $3;
                }
            | Fact              { $$ = $1; }
            ;

Fact        : Var               { $$ = $1; }
            | Number            { $$ = $1; }
            | FuncCall          { $$ = $1; }
            | TMINUS Fact       { $$ = makeNode("NEG", NULL, $2); }
            | '(' Expr ')'      { $$ = $2; }
            ;

FuncCall    : FuncName '(' ParamList ')' {
                $$ = makeNode("FUNC_CALL", NULL, $1);
                $1->bro = $3;
            }
            ;

ParamList   : ExprList          { $$ = $1; }
            |                   { $$ = NULL; }
            ;

ExprList    : ExprList ',' Expr {
                Node * ptr = $1;
                while (ptr->bro != NULL) ptr = ptr->bro;
                ptr->bro = $3;
                $$ = $1;
            }
            | Expr              { $$ = $1; }
            ;

Program     : TPROGRAM Name ';' SubPgmList TMAIN VarDecl CompStmt '.' { 
                rootNode = makeNode("PROGRAM", NULL, NULL);
                Node * ptr = NULL;

                // 1. SubPgmList 처리
                if ($4 != NULL) {
                    rootNode->son = $4; // 첫 번째 자식으로 등록
                    ptr = $4;
                }

                // 2. VarDecl 처리
                if ($6 != NULL) {
                    if (ptr == NULL) {
                        rootNode->son = $6; // 앞선 자식이 없으면 얘가 첫째
                    } else {
                        ptr->bro = $6;      // 앞선 자식의 형제로 연결
                    }
                    ptr = $6; // 포인터 이동
                }

                // 3. CompStmt 처리 (문법상 항상 존재하므로 NULL 체크 불필요하지만 안전하게 연결)
                if (ptr == NULL) {
                    rootNode->son = $7;
                } else {
                    ptr->bro = $7;
                }
            }
            ;

VarDecl     : TVAR DclList ';' { $$ = makeNode("VARDECL", NULL, $2); }
            | { $$ = NULL; }
            ;

DclList     : DclList ';' Decl { $$ = makeNode("DCLLIST", $1, NULL);
                $1->bro = $3;
            }
            | Decl { $$ = $1; }
            ;

Decl        : VarList ':' Type { $$ = makeNode("DECL", NULL, $1);
                $1->bro = $3;
            }
            ;

VarList     : VarList ',' Var {
                $$ = makeNode("VARLIST", $1, NULL);
                $1->bro = $3;
            }
            | Var { $$ = $1; }
            ;

Type        : TINT { $$ = makeNode("INT", NULL, NULL);}
            | TLONG { $$ = makeNode("LONG", NULL, NULL);}
            ;

Var         : Name { $$ = $1; }
            ;

SubPgmList  : SubPgmList SubPgm {
                if ($1 == NULL) {
                    $$ = $2;
                } else {
                    Node * ptr = $1;
                    while (ptr->bro != NULL) ptr = ptr->bro;
                    ptr->bro = $2;
                    $$ = $1;
                }
            }
            | { $$ = NULL; }
            ;

SubPgm      : ProcDecl { $$ = $1; }
            | FuncDecl { $$ = $1; }
            ;

ProcDecl    : TPROC ProcName '(' FormParam ')' VarDecl CompStmt {
                $$ = makeNode("PROC", NULL, $2);
                $2->bro = $4;
                $4->bro = $6;
                $6->bro = $7;
            }
            ;

FuncDecl    : TFUNC FuncName '(' FormParam ')' TRETURNS '(' Type ')' VarDecl CompStmt {
                $$ = makeNode("FUNC", NULL, $2);
                $2->bro = $8;
                $8->bro = $4;
                $4->bro = $10;
                $10->bro = $11;
            }
            ;

FormParam   : DclList { $$ = $1; }
            | { $$ = NULL; }
            ;

Name        : TWORD {
                $$ = makeNode("NAME", NULL, NULL);
                $$->value.sv = $1;
            }
            ;

FuncName    : TWORD {
                $$ = makeNode("NAME", NULL, NULL);
                $$->value.sv = $1;
            }
            ;

ProcName    : TWORD {
                $$ = makeNode("NAME", NULL, NULL);
                $$->value.sv = $1;
            }
            ;

Number      : TNUMBER {
                $$ = makeNode("NUMBER", NULL, NULL);
                $$->value.dv = $1;
            }
            ;

%%
int yyerror() {
    printf("syntax error\n");
}

void traverse(Node * nodeP) {
	// while (nodeP!=NULL) {
	// 	printf("%s\n", nodeP->kind);
	// 	traverse(nodeP->son);
	// 	nodeP=nodeP->bro;
	// }

    // if (nodeP->son)
    //     traverse(nodeP->son);
        
    // if (nodeP->bro)
    //     traverse(nodeP->bro);

    // printf("%s\n", nodeP->kind);

    if (nodeP == NULL)
        return;

    Node* son = nodeP->son;
    Node* bro = nodeP->bro;

    printf("%s\n", nodeP->kind);
    traverse(son);
    traverse(bro);
}

int main() {
    yyparse();
    traverse(rootNode);

    // code(rootNode);

    return 0;
}