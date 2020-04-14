OPTIONS = -Wall -Werror

all: my_prog
	
my_prog: msuprak2.o
		g++ -o my_prog msuprak2.o

msuprak1.o: msuprak2.cpp
		g++ $(OPTIONS) -c msuprak2.cpp

run:	my_prog
		./my_prog

clean:
		rm -rf *.o my_prog