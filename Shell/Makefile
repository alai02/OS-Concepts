
all: msh

msh: my_shell.o lex.yy.o
	gcc -o msh my_shell.o lex.yy.o -lfl

my_shell.o: my_shell.c
	gcc -Wall -g -c my_shell.c

lex.yy.o: lex.yy.c
	gcc -Wall -g -c lex.yy.c -Wno-unused-function

lex.yy.c: lex.c
	flex lex.c

clean:
	rm -f *.o lex.yy.c msh
	
