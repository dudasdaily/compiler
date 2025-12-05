all: expParser

toypl.tab.c toypl.tab.h:	toypl.y
	bison -d toypl.y

lex.yy.c:	toypl.l toypl.tab.h
	flex toypl.l

expParser: lex.yy.c toypl.tab.c toypl.tab.h
	gcc -o expParser lex.yy.c toypl.tab.c node.c -lfl

clean:
	rm expParser toypl.tab.c lex.yy.c toypl.tab.h