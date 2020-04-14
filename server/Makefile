all: my_prog
	
my_prog: server.o
		g++ -o my_prog server.o

server.o: server.cpp
		g++ -c server.cpp

run:	my_prog
		./my_prog
clean:
		rm -rf *.o my_prog
		rm -rf *.exe my_prog