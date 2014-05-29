/**
 * Copyright (C) 2014 Leonardo Rossi <leonardo.rossi@studenti.unipr.it>
 *
 * This source code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this source code; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <omp.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <boost/serialization/vector.hpp>
#include "magic-square.h"

// http://en.wikipedia.org/wiki/Sieve_of_Atkin

/**
 * Print a vector
 */
void print_vector(ms_vector vector) {
	for (ms_vector::iterator i = vector.begin(); i != vector.end(); i++) {
		std::cout << *i << "\n";
	}
	cout << endl;
}

/**
 * Print a matrix
 */
void print_matrix(ms_matrix matrix) {
	for (ms_matrix::iterator i = matrix.begin(); i != matrix.end(); i++) {
		print_vector(*i);
	}
}

/**
 * Print a list of matrixs
 */
void print_list_matrix(vector<ms_matrix> list) {
	// print all generated matrix
	for (vector<ms_matrix>::iterator i = list.begin(); i != list.end(); i++) {
		if (is_magic_square(&*i)) {
			cout << "Found a magic square!\n";
		}
		print_matrix(*i);
		cout << endl;
	}
}

/**
 * Convert the prime numbers representation:
 *   from: if is_prime[n] == true then n is prime, false otherwise
 *   to  : list of prime numbers
 *
 * @param is_prime the array in input
 * @param limit the size of array is_prime
 * @param primes the output array
 */
void is_prime2primes(vector<bool> is_prime, int limit, ms_vector *primes) {
	// get all prime numbers preserving the order
	for (int n = 0; n <= limit; n++) {
		if (is_prime[n]) {
			primes->push_back(n);
		}
	}
}

/**
 * find prime numbers in a range between [2,limit]
 * 
 * @param limit upper limit
 * @param is_prime return a array[limit+1] with a representation of number (if is_prime[n] == true then n is prime, false otherwise)
 */
void find_prime_numbers(mpi::communicator world, int limit, ms_vector *primes) {
	int sqrt_limit = ceil(sqrt(limit));
	vector<bool> is_prime(limit + 1, false);
	vector<vector<bool> > matrix_is_prime(world.size());

	//if (world.rank() == 0) {
		is_prime[2] = true;
		is_prime[3] = true;
	//}

	int size = world.size();
	// if the number of process > sqrt_limit
	if (size > sqrt_limit)
		// simulate to have sqrt_limit processes
		size = sqrt_limit;
	// compute how many numbers scan for each process
	int howmuch = sqrt_limit / size;
	// compute where the process start to look
	int start = 1 + (howmuch * world.rank());
	// compute where the process stop to look
	int stop = howmuch * (world.rank() + 1);
	// if stop is out of limit, set stop as limit
	if (stop > limit)
		stop = limit;
//cout<<"start "<<start<<" stop "<<stop<<endl;
	// execute algorithm
	for (int x = start; x <= stop; x++) {
//#		pragma omp parallel for default(none) shared(sqrt_limit, limit, is_prime, x)
		for (int y = 1; y <= sqrt_limit; y++) {
			int n = 4 * x * x + y * y;

			if (n <= limit && ((n % 12) == 1 || (n % 12) == 5))
				is_prime[n] = !is_prime[n];

			n = 3 * x * x + y * y;

			if (n <= limit && (n % 12) == 7)
				is_prime[n] = !is_prime[n];

			n = 3 * x * x - y * y;

			if (x > y && n <= limit && (n % 12) == 11)
				is_prime[n] = !is_prime[n];
		}
	}

	// gather: receive all generated matrix
	mpi::gather(world, is_prime, matrix_is_prime, 0);

	if (world.rank() == 0) {

		// take the last update
		for (unsigned int i = 1; i < matrix_is_prime.size(); i++) {
//#			pragma omp parallel for default(none) shared(matrix_is_prime, limit, i)
			for (int j = 1; j <= limit; j++) {
				if (matrix_is_prime[i - 1][j])
					matrix_is_prime[i][j] = !matrix_is_prime[i][j];
			}
		}

		// remove the others no prime numbers
//#		pragma omp parallel for default(none) shared(sqrt_limit, matrix_is_prime, limit)
		for (int n = 5; n <= sqrt_limit; n++) {
			if (matrix_is_prime[matrix_is_prime.size() - 1][n]) {
				int k = n * n;
				for (int i = k; i <= limit; i += k) {
					matrix_is_prime[matrix_is_prime.size() - 1][i] = false;
				}
			}
		}

		// put number 2 and 3
		if(!matrix_is_prime[matrix_is_prime.size() - 1][2]){
			primes->push_back(2);
			primes->push_back(3);
		}
		// convert the structure in a array with inside only the prime numbers
		is_prime2primes(matrix_is_prime[matrix_is_prime.size() - 1], limit,
				primes);
	}
}

/**
 * Fill matrix with random selected prime numbers from primes.
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
void fill_random_matrix(ms_vector *primes, ms_matrix *matrix, int seed) {
	// init random generator
	// TODO fix random generator! T_T
//	std::default_random_engine generator;
//	std::uniform_real_distribution<double> distribution(0.0, primes->size());
	srand(time(NULL) + rand() + seed);

	// fill the matrix
#	pragma omp parallel for default(none) shared(matrix, primes)
	for (unsigned int i = 0; i < matrix->size(); i++) {
		for (unsigned int j = 0; j < matrix->size(); j++) {
//			int x = (int)distribution(generator);
			int x = (int) (rand() % primes->size());
			(*matrix)[i][j] = (*primes)[x];
		}
	}
}

/**
 * Fill matrix with a consecutive strategy: first random and for others positions select consecutive prime numbers
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
void fill_with_consecutive(ms_vector *primes, ms_matrix *matrix,
		int seed_random_num) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand() + seed_random_num);
	int seed = (int) (rand() % primes->size());

#pragma omp parallel sections
	{
#		pragma omp section
		{
			(*matrix)[0][0] = (*primes)[seed - 1];
			(*matrix)[0][1] = (*primes)[seed + 1];
			(*matrix)[0][2] = (*primes)[seed - 2];
			(*matrix)[1][0] = (*primes)[seed + 2];
		}

#		pragma omp section
		{
			(*matrix)[1][1] = (*primes)[seed];
			(*matrix)[1][2] = (*primes)[seed - 3];
			(*matrix)[2][0] = (*primes)[seed + 3];
			(*matrix)[2][1] = (*primes)[seed - 4];
			(*matrix)[2][2] = (*primes)[seed + 4];
		}
	}
}

/**
 * Look for a couple of prime number which the sum is equal to 'sum'
 * @param primes vector of prime numbers
 * @param sum the constrain: sum == first + second
 * @param first the first prime number found
 * @param second the second prime number found
 * @return true, if founded a couple of prime number that satisfy the condition
 */
bool look_for_couple_prime_with_condition(ms_vector *primes, int sum,
		ms_vector not2consider, int &first, int &first_position, int &second,
		int &second_position) {
	int size = primes->size();
	bool ret = false;
	for (int i = 0; i < size; i++) {
		if (!ret) {
			ms_vector::iterator found_first = find(not2consider.begin(),
					not2consider.end(), i);
			// if the index of prime number is not ones to avoid
			if (found_first == not2consider.end()) {
				int f = (*primes)[i];
#				pragma omp parallel for default(none) shared(f, size, not2consider, ret, primes, sum, first, first_position, second, second_position, i)
				for (int j = 0; j < size; j++) {
					ms_vector::iterator found_sec;
					found_sec = find(not2consider.begin(), not2consider.end(),
							j);
					// if the index of prime number is not ones to avoid
#				pragma omp flush(ret)
					if (!ret && found_sec == not2consider.end()) {
						int s = (*primes)[j];
						if (sum == (f + s)) {
							// found!
#							pragma omp flush(ret)
							ret = true;
							first = f;
							first_position = i;
							second = s;
							second_position = j;
						}
					}
				}
			}
		}
	}
	return ret;
}

/**
 * Fill matrix with a heuristic strategy: first random and for others positions as result of scanned prime numbers vector
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
bool fill_in_heuristic_mode_1(ms_vector *primes, ms_matrix *matrix, int seed) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand() + seed);

	// select randomly first 3 prime numbers
	int seed_00 = (int) (rand() % primes->size());
	int seed_01 = (int) (rand() % primes->size());
	int seed_02 = (int) (rand() % primes->size());

	// save first column
	(*matrix)[0][0] = (*primes)[seed_00];
	(*matrix)[0][1] = (*primes)[seed_01];
	(*matrix)[0][2] = (*primes)[seed_02];

	// compute the sum
	int sum = (*matrix)[0][0] + (*matrix)[0][1] + (*matrix)[0][2];

	vector<int> not2consider;
	// list of index of prime numbers not to be considered in search
	not2consider.push_back(seed_00);
	not2consider.push_back(seed_01);
	not2consider.push_back(seed_02);

	int first, first_position, second, second_position;

	bool ret = true;
	for (unsigned int i = 0; i < matrix->size(); i++) {
#		pragma omp flush(ret)
		if (ret) {
			// test i-th column
			if (look_for_couple_prime_with_condition(primes,
					sum - (*matrix)[0][i], not2consider, first, first_position,
					second, second_position)) {
				// save position of founded prime numbers
				not2consider.push_back(first_position);
				not2consider.push_back(second_position);

				// save new prime numbers
				int n1, n2;
				if ((i % 2) == 0) {
					n1 = first;
					n2 = second;
				} else {
					n1 = second;
					n2 = first;
				}
				(*matrix)[1][i] = n1;
				(*matrix)[2][i] = n2;
			} else {
#				pragma omp flush(ret)
				ret = false;
			}
		}
	}

	return ret;
}

/**
 * Fill matrix with a heuristic strategy: first 5 random and then compute the others.
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
bool fill_in_heuristic_mode_2(ms_vector *primes, ms_matrix *matrix, int seed) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand() + seed);

	// select randomly  prime number
	int seed_00 = (int) (rand() % primes->size());
	// save in matrix
	(*matrix)[0][0] = (*primes)[seed_00];

	// select randomly  prime number
	int seed_01 = (int) (rand() % primes->size());
	// save in matrix
	(*matrix)[0][1] = (*primes)[seed_01];

	// select randomly  prime number
	int seed_02 = (int) (rand() % primes->size());
	// save in matrix
	(*matrix)[0][2] = (*primes)[seed_02];

	// select randomly  prime number
	int seed_11 = (int) (rand() % primes->size());
	// save in matrix
	(*matrix)[1][1] = (*primes)[seed_11];

	// compute the sum
	int sum = (*matrix)[0][0] + (*matrix)[0][1] + (*matrix)[0][2];

	bool ret = true;

#pragma omp parallel sections
	{
#		pragma omp section
		{
			// compute
			(*matrix)[2][2] = sum - (*matrix)[0][0] - (*matrix)[1][1];
			// test if they are prime numbers
			if (find(primes->begin(), primes->end(), (*matrix)[2][2])
					== primes->end())
				ret = false;
		}

#		pragma omp section
		{
			// compute
			(*matrix)[2][1] = sum - (*matrix)[0][1] - (*matrix)[1][1];
			// test if they are prime numbers
			if (find(primes->begin(), primes->end(), (*matrix)[2][1])
					== primes->end())
				ret = false;
		}

#		pragma omp section
		{
			// compute
			(*matrix)[1][2] = sum - (*matrix)[0][2] - (*matrix)[2][2];
			if (find(primes->begin(), primes->end(), (*matrix)[1][2])
					== primes->end())
				ret = false;
		}

#		pragma omp section
		{
			// compute
			(*matrix)[1][0] = sum - (*matrix)[1][1] - (*matrix)[1][2];
			if (find(primes->begin(), primes->end(), (*matrix)[1][0])
					== primes->end())
				ret = false;
		}

#		pragma omp section
		{
			// compute
			(*matrix)[2][0] = sum - (*matrix)[2][1] - (*matrix)[2][2];
			if (find(primes->begin(), primes->end(), (*matrix)[1][0])
					== primes->end())
				ret = false;
		}
	}
	return ret;
}

/**
 * Test if the matrix is a magic_square
 * @param matrix matrix to control
 * @return true, if the matrix is a magic square
 */
bool is_magic_square(ms_matrix *matrix) {
	bool ret = true;

	// test if all numbers are different from 0
#	pragma omp parallel for default(none) shared(matrix, ret)
	for (unsigned int i = 0; i < matrix->size(); i++) {
#		pragma omp flush(ret)
		if (ret) {
			for (unsigned int j = 0; j < matrix->size(); j++) {
				if ((*matrix)[i][j] < 1) {
#					pragma omp flush(ret)
					ret = false;
				}
			}
		}
	}

	if (!ret)
		return false;

	// test diagonal
	int sum = (*matrix)[0][0] + (*matrix)[1][1] + (*matrix)[2][2];
	if (sum != ((*matrix)[2][0] + (*matrix)[1][1] + (*matrix)[0][2]))
		ret = false;

	if (ret) {
		// test rows and cols
#		pragma omp parallel for default(none) shared(ret, matrix, sum)
		for (int i = 0; i < 3; i++) {
#			pragma omp flush(ret)
			if (ret) {
				// test col
				if (sum
						!= ((*matrix)[i][0] + (*matrix)[i][1] + (*matrix)[i][2])) {
#					pragma omp flush(ret)
					ret = false;
				}
				// test row
				if (sum
						!= ((*matrix)[0][i] + (*matrix)[1][i] + (*matrix)[2][i])) {
#					pragma omp flush(ret)
					ret = false;
				}
			}
		}
	}

	return ret;
}

/**
 * test first strategy: explorer_strategy() + print generated matrix
 * @strategy
 */
void test_explorer_strategy(mpi::communicator world, int limit) {
	// primes number data structure
	ms_vector primes;
	// vector to collecting all generated matrix
	vector<ms_matrix> list;
	// my rank
	int rank = world.rank();

	if (rank == 0) {
		cout << "Test the Explorer strategy...\n";
	}

	// generate primes numbers
	find_prime_numbers(world, limit, &primes);

	// send to all the prime numbers
	mpi::broadcast(world, primes, 0);

	int length = 3;
	ms_matrix matrix(length, ms_vector(length));

	fill_random_matrix(&primes, &matrix, rank);

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

/**
 * Test consecutive_strategy + view generated matrix
 * @strategy
 */
void test_consecutive_strategy(mpi::communicator world, int limit) {
	// primes number data structure
	ms_vector primes;
	// vector to collecting all generated matrix
	vector<ms_matrix> list;
	// my rank
	int rank = world.rank();

	if (rank == 0) {
		cout << "Test the Consecutive strategy...\n";
	}

	// generate primes numbers
	find_prime_numbers(world, limit, &primes);

	// send to all the prime numbers
	mpi::broadcast(world, primes, 0);

	int length = 3;
	ms_matrix matrix(length, ms_vector(length));

	fill_with_consecutive(&primes, &matrix, rank);

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

/**
 * Heuristic strategy:
 * Select randomly the first column, then I look for
 * the others cells as consequence of this selection.
 * @strategy
 */
void test_heuristic_strategy_1(mpi::communicator world, int limit) {
	// primes number data structure
	ms_vector primes;
	// vector to collecting all generated matrix
	vector<ms_matrix> list;
	// my rank
	int rank = world.rank();

	if (rank == 0) {
		cout << "Test the heuristic strategy...\n";
	}

	// generate primes numbers
	find_prime_numbers(world, limit, &primes);

	// send to all the prime numbers
	mpi::broadcast(world, primes, 0);

	int length = 3;
	ms_matrix matrix(length, ms_vector(length));

	fill_in_heuristic_mode_1(&primes, &matrix, rank);

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

/**
 * Heuristic strategy 2:
 * Select randomly the first 4 numbers, then I look for
 * the others cells as consequence of this selection.
 * I have 8 equations and 9 variables.
 * But, I need only 5 variables and the others can be
 * calculated from those.
 * @strategy
 */
void test_heuristic_strategy_2(mpi::communicator world, int limit) {
	// primes number data structure
	ms_vector primes;
	// vector to collecting all generated matrix
	vector<ms_matrix> list;
	// my rank
	int rank = world.rank();

	if (rank == 0) {
		cout << "Test the heuristic strategy 2...\n";
	}

	// generate primes numbers
	find_prime_numbers(world, limit, &primes);

	// send to all the prime numbers
	mpi::broadcast(world, primes, 0);

	int length = 3;
	ms_matrix matrix(length, ms_vector(length));

	fill_in_heuristic_mode_2(&primes, &matrix, rank);

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}
