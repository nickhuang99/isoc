export LANG=en_US.UTF-8
CC=/home/nick/opt/gcc-10.2.0/bin/gcc
CPP=/home/nick/opt/gcc-10.2.0/bin/g++

all: isoc.exe

isoc.exe: bnf.exe lexer.exe
	${CPP} -std=c++20 -I. -g -O0 -DYYMAXDEPTH=160000 isoc.c lexer.c -o isoc.exe
		
bnf.exe: grammar.txt bnf.cpp
	${CPP} -std=c++20 -g -O0 -I. -static-libstdc++ bnf.cpp -o bnf.exe	
	./bnf.exe
	bison -d -t -v -g --html=isoc.html --report-file=isoc.output isoc.y -o isoc.c
	
lexer.exe: lexer.l isoc.h
	flex -t lexer.l > lexer.c
	${CPP} -std=c++20 -I. -DLEXER_TEST lexer.c -o lexer.exe 

clean:
	rm -f *.exe isoc.* lexer.c
			
