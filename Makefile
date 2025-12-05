all: expParser

exp.tab.c exp.tab.h:	exp.y
	bison -d exp.y

lex.yy.c:	exp.l exp.tab.h
	flex exp.l

expParser: lex.yy.c exp.tab.c exp.tab.h
	gcc -o expParser lex.yy.c exp.tab.c node.c -ll

clean:
	rm expParser exp.tab.c lex.yy.c exp.tab.h