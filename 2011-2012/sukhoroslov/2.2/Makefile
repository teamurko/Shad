all:
	g++ -Wall -o single_life single_life.cpp
	mpicc -Wall -o multi_life multi_life.c
	g++ -Wall -o gen gen.cpp

clean:
	rm single_life gen multi_life input.txt multi.out single.out
