all: my_prog

my_prog: msuprak2.o
		g++ -o my_prog msuprak2.o

msuprak1.o: msuprak2.cpp
		g++ -c msuprak2.cpp
		
clean:
		rm -rf *.o my_prog