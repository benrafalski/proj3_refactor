project3: project3.o lexer.o inputbuf.o parser.o stack.o tree.o execute.o helperfuns.o typechecker.o
	g++ -std=c++11 -o project3 project3.o lexer.o inputbuf.o parser.o stack.o tree.o execute.o helperfuns.o typechecker.o

project3.o: project3.cc
	g++ -c -std=c++11 project3.cc

lexer.o: lexer.h lexer.cc
	g++ -c -std=c++11 lexer.cc

inputbuf.o: inputbuf.h inputbuf.cc
	g++ -c -std=c++11 inputbuf.cc

parser.o: parser.h parser.cc
	g++ -c -std=c++11 parser.cc

tree.o: tree.h tree.cc
	g++ -c -std=c++11 tree.cc

stack.o: stack.h stack.cc
	g++ -c -std=c++11 stack.cc

execute.o: execute.h execute.cc
	g++ -c -std=c++11 execute.cc

typechecker.o: typechecker.h typechecker.cc
	g++ -c -std=c++11 typechecker.cc

helperfuns.o: helperfuns.h helperfuns.cc
	g++ -c -std=c++11 helperfuns.cc

clean:
	rm lexer.o inputbuf.o parser.o project3 stack.o project3.o tree.o execute.o helperfuns.o typechecker.o