#  Bryan Butz
.DEFAULT_GOAL := Assign5.exe

assign5.o: assign5.cc
	g++ -c assign5.cc -Wall -std=c++11

Assign5.exe: assign5.o
	g++ -pthread -o Assign5.exe assign5.o -Wall -std=c++11

clean:
	rm *.o Assign5.exe
