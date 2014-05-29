FLAGS=-Wall -std=c++11 -fopenmp -lboost_mpi -lboost_serialization
CC=mpic++

all:  bin/main bin/tests

bin/tests: bin/magic-square.o bin/tests.o
	${CC} ${FLAGS} bin/magic-square.o bin/tests.o -o bin/tests

bin/main: bin/magic-square.o bin/main.o
	${CC} ${FLAGS} bin/magic-square.o bin/main.o -o bin/main

bin/tests.o: tests.cpp
	${CC} ${FLAGS} -c tests.cpp -o bin/tests.o

bin/main.o: main.cpp
	${CC} ${FLAGS} -c main.cpp -o bin/main.o

bin/magic-square.o: magic-square.cpp magic-square.h
	${CC} ${FLAGS} -c magic-square.cpp -o bin/magic-square.o 

clean:
	rm -f bin/*
