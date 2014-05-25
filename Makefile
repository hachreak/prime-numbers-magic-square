FLAGS=-Wall -std=c++11 -fopenmp -lboost_mpi -lboost_serialization
CC=mpic++

all:  main test

test: magic-square.o test.o
	${CC} main-square.o test.o -o bin/test

main: magic-square.o main.o
	${CC} main-square.o main.o -o bin/ms

test.o: test.cpp
	${CC} ${FLAGS} magic-square.o test.cpp -o bin/test.o

main.o: main.cpp
	${CC} ${FLAGS} main.cpp -o bin/main.o

magic-square.o: magic-square.cpp magic-square.h
	${CC} ${FLAGS} -c magic-square.cpp -o bin/magic-square.o 

clean:
	rm -f bin/*.o bin/*~
