#  Bryan Butz
.DEFAULT_GOAL := assign7.exe

assign7.o: assign7.cc
	g++ -c assign7.cc -Wall -std=c++11

assign7.exe: assign7.o
	g++ -o assign7.exe assign7.o -Wall -std=c++11

clean:
	rm *.o assign7.exe
