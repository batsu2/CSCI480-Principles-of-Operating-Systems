#  Bryan Butz
.DEFAULT_GOAL := Assign2.exe

assign2.o: assign2.cc
	g++ -c assign2.cc -Wall -std=c++11

Assign2.exe: assign2.o
	g++ -o Assign2.exe assign2.o -Wall -std=c++11

clean:
	rm *.o Assign2.exe
