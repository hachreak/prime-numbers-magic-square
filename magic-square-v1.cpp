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
//#include <iostream>
#include <math.h>
#include <vector>
#include <random>
#include <algorithm>
#include <boost/mpi.hpp>
#include <iostream>
#include <cstdlib>
#include <boost/serialization/vector.hpp>

namespace mpi = boost::mpi;

//#include <mpi.h>
//#include <time.h>

// http://en.wikipedia.org/wiki/Sieve_of_Atkin

//#define limit 1000000

using namespace std;

typedef vector<int> ms_vector;
typedef vector<vector<int> > ms_matrix;

/**
 * Print a vector
 */
void print_vector(ms_vector vector) {
	for (ms_vector::iterator i = vector.begin(); i != vector.end(); i++) {
		std::cout << *i << "\t";
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
void is_prime2primes(bool *is_prime, int limit, ms_vector *primes) {
//#   pragma omp parallel for default(none) shared(limit, is_prime, primes)
	for (int n = 0; n <= limit; n++) {
		if (is_prime[n]) {
//#           pragma omp critical
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
void find_prime_numbers(int limit, ms_vector *primes) {
	int sqrt_limit = ceil(sqrt(limit));
	bool is_prime[limit + 1];

	// init vector
#   pragma omp parallel for default(none) shared(limit, is_prime)
	for (int i = 0; i < (limit + 1); i++) {
		is_prime[i] = false;
	}
	is_prime[2] = true;
	is_prime[3] = true;

//#   pragma omp parallel for default(none) shared(sqrt_limit, limit, is_prime)
	for (int x = 1; x <= sqrt_limit; x++) {
		// TODO can I parallelize????
#       pragma omp parallel for default(none) shared(sqrt_limit, limit, is_prime, x)
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

#   pragma omp parallel for default(none) shared(sqrt_limit, is_prime, limit)
	for (int n = 5; n <= sqrt_limit; n++) {
		if (is_prime[n]) {
			int k = n * n;
			for (int i = k; i <= limit; i += k) {
				is_prime[i] = false;
			}
		}
	}

	// convert the structure in a array with inside only the prime numbers
	is_prime2primes(is_prime, limit, primes);
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

	// TODO parallelize with MPI???
	for (int i = 0; i < matrix->size(); i++) {
#       pragma omp parallel for default(none) shared(matrix, primes, i)
		for (int j = 0; j < matrix->size(); j++) {
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
void fill_with_consecutive(ms_vector *primes, ms_matrix *matrix, int seed_random_num) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand() + seed_random_num);
	int seed = (int) (rand() % primes->size());

#pragma omp parallel sections
{
#   pragma omp section
	{
		(*matrix)[0][0] = (*primes)[seed - 1];
		(*matrix)[0][1] = (*primes)[seed + 1];
		(*matrix)[0][2] = (*primes)[seed - 2];
		(*matrix)[1][0] = (*primes)[seed + 2];
	}

#   pragma omp section
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
#               pragma omp parallel for default(none) shared(f, size, not2consider, ret, primes, sum, first, first_position, second, second_position, i)
				for (int j = 0; j < size; j++) {
					ms_vector::iterator found_sec;
//#                   pragma omp critical
					found_sec = find(not2consider.begin(), not2consider.end(),
							j);
					// if the index of prime number is not ones to avoid
#                   pragma omp flush(ret)
					if (!ret && found_sec == not2consider.end()) {
						int s = (*primes)[j];
//#                           pragma omp critical
						if (sum == (f + s)) {
							// found!
#                           pragma omp flush(ret)
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
	for (int i = 0; i < matrix->size(); i++) {
#       pragma omp flush(ret)
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
#               pragma omp flush(ret)
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
#   pragma omp section
	{
		// compute
		(*matrix)[2][2] = sum - (*matrix)[0][0] - (*matrix)[1][1];
		// test if they are prime numbers
		if (find(primes->begin(), primes->end(), (*matrix)[2][2])
				== primes->end())
			ret = false;
	}

#   pragma omp section
	{
		// compute
		(*matrix)[2][1] = sum - (*matrix)[0][1] - (*matrix)[1][1];
		// test if they are prime numbers
		if (find(primes->begin(), primes->end(), (*matrix)[2][1])
				== primes->end())
			ret = false;
	}

#   pragma omp section
	{
		// compute
		(*matrix)[1][2] = sum - (*matrix)[0][2] - (*matrix)[2][2];
		if (find(primes->begin(), primes->end(), (*matrix)[1][2])
				== primes->end())
			ret = false;
	}

#   pragma omp section
	{
		// compute
		(*matrix)[1][0] = sum - (*matrix)[1][1] - (*matrix)[1][2];
		if (find(primes->begin(), primes->end(), (*matrix)[1][0])
				== primes->end())
			ret = false;
	}

#   pragma omp section
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
#   pragma omp parallel for default(none) shared(matrix, ret)
	for (int i = 0; i < matrix->size(); i++) {
#       pragma omp flush(ret)
		if (ret) {
			for (int j = 0; j < matrix->size(); j++) {
				if ((*matrix)[i][j] < 1) {
#                   pragma omp flush(ret)
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
#   pragma omp parallel for default(none) shared(ret, matrix, sum)
		for (int i = 0; i < 3; i++) {
#           pragma omp flush(ret)
			if (ret) {
				// test col
				if (sum
						!= ((*matrix)[i][0] + (*matrix)[i][1] + (*matrix)[i][2])) {
					// TODO can I use atomic???
#                   pragma omp flush(ret)
					ret = false;
				}
				// test row
				if (sum
						!= ((*matrix)[0][i] + (*matrix)[1][i] + (*matrix)[2][i])) {
					// TODO can I use atomic???
#                   pragma omp flush(ret)
					ret = false;
				}
			}
		}
	}

	return ret;
}

/**
 * In this approach the matrix elements are taken at random and 
 * repeating primes within the matrix is allowed.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
bool explorer_strategy(ms_vector *primes, ms_matrix *matrix, int seed) {
	fill_random_matrix(primes, matrix, seed);
	return is_magic_square(matrix);
}

/**
 * In this approach only the central element is taken at random and
 * the others are taken around him.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
bool consecutive_strategy(ms_vector *primes, ms_matrix *matrix, int seed) {
	fill_with_consecutive(primes, matrix, seed);
	return is_magic_square(matrix);
}

/**
 * Heuristic strategy:
 * In this approach, I select randomly the first column,
 * then I look for the others cells as consequence of this selection.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
bool heuristic_strategy_1(ms_vector *primes, ms_matrix *matrix, int seed) {
	if (fill_in_heuristic_mode_1(primes, matrix, seed))
		return is_magic_square(matrix);
	return false;
}

/**
 * Heuristic strategy:
 * In this approach, I select randomly the first 4 numbers, then I look for
 * the others cells as consequence of this selection.
 * I have 8 equations and 9 variables.
 * But, I need only 5 variables and the others can be calculated from those.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
bool heuristic_strategy_2(ms_vector *primes, ms_matrix *matrix, int seed) {
	if (fill_in_heuristic_mode_2(primes, matrix, seed))
		return is_magic_square(matrix);
	return false;
}
