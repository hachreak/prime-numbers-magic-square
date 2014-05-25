FLAGS=-Wall -std=c++11 -fopenmp -lboost_mpi -lboost_serialization
CC=mpic++

all:  main tests

tests: magic-square.o tests.o
	${CC} ${FLAGS} bin/magic-square.o bin/tests.o -o bin/tests

main: magic-square.o main.o
	${CC} ${FLAGS} bin/magic-square.o bin/main.o -o bin/main

tests.o: tests.cpp
	${CC} ${FLAGS} -c tests.cpp -o bin/tests.o

main.o: main.cpp
	${CC} ${FLAGS} -c main.cpp -o bin/main.o

magic-square.o: magic-square.cpp magic-square.h
	${CC} ${FLAGS} -c magic-square.cpp -o bin/magic-square.o 

clean:
	rm -f bin/*
