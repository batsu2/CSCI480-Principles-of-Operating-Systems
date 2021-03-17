#  Bryan Butz
.DEFAULT_GOAL := Assign1.exe

shuffle.o: Assign1.cc
	g++ -c Assign1.cc -Wall -std=c++11

Assign1.exe: Assign1.o
	g++ -o Assign1.exe Assign1.o -Wall -std=c++11

clean:
	rm *.o Assign1.exe
