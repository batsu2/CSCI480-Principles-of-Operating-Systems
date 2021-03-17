#  Bryan Butz
.DEFAULT_GOAL := assign6.exe

assign6.o: assign6.cc
	g++ -c assign6.cc -Wall -std=c++11

assign6.exe: assign6.o
	g++ -o assign6.exe assign6.o -Wall -std=c++11

clean:
	rm *.o assign6.exe
