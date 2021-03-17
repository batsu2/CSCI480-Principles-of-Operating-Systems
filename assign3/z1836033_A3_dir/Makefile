#  Bryan Butz
.DEFAULT_GOAL := z1836033_A3.exe

assign3.o: assign3.cc
	g++ -c assign3.cc -Wall -std=c++11

z1836033_A3.exe: assign3.o
	g++ -o z1836033_A3.exe assign3.o -Wall -std=c++11

clean:
	rm *.o z1836033_A3.exe
