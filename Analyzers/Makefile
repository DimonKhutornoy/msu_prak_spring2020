all: my_prog
	
my_prog: LexicalAnalyzer.o
		g++ -o my_prog LexicalAnalyzer.o

LexicalAnalyzer.o: LexicalAnalyzer.cpp
		g++ -c LexicalAnalyzer.cpp

run:	my_prog
		./my_prog
clean:
		rm -rf *.o my_prog
		rm -rf *.exe my_prog