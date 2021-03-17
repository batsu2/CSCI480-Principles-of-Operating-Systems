#  Bryan Butz
.DEFAULT_GOAL := Assign4.exe

assign4.o: assign4.cc
	g++ -c assign4.cc -Wall -std=c++11

Assign4.exe: assign4.o
	g++ -o Assign4.exe assign4.o -Wall -std=c++11

clean:
	rm *.o Assign4.exe
