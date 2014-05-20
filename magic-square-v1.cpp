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
#include <iostream>
#include <math.h>
#include <vector>
#include <random>
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
	for (ms_vector::iterator i = vector.begin(); i != vector.end();
			i++) {
		std::cout << *i << "\t";
	}
	cout << endl;
}

/**
 * Print a matrix
 */
void print_matrix(ms_matrix matrix) {
	for (ms_matrix::iterator i = matrix.begin();
			i != matrix.end(); i++) {
		print_vector(*i);
	}
}

/**
 * find prime numbers in a range between [2,limit]
 * 
 * @param limit upper limit
 * @param is_prime return a array[limit+1] with a representation of number (if is_prime[n] == true then n is prime, false otherwise)
 */
void find_prime_numbers(int limit, bool *is_prime) {
	int sqrt_limit = ceil(sqrt(limit));

	for (int i = 0; i < (limit + 1); i++) {
		is_prime[i] = false;
	}
	is_prime[2] = true;
	is_prime[3] = true;

	for (int x = 1; x <= sqrt_limit; x++) {
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

	for (int n = 5; n <= sqrt_limit; n++) {
		if (is_prime[n]) {
			int k = n * n;
			for (int i = k; i <= limit; i += k) {
				is_prime[i] = false;
			}
		}
	}

	return;
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
	for (int n = 0; n <= limit; n++) {
		if (is_prime[n]) {
			primes->push_back(n);
		}
	}
}

void fill_random_matrix(ms_vector *primes,
		ms_matrix *matrix) {
	// init random generator
	// TODO fix random generator! T_T
//	std::default_random_engine generator;
//	std::uniform_real_distribution<double> distribution(0.0, primes->size());
	srand(time(NULL) + rand());

	for (int i = 0; i < matrix->size(); i++) {
		for (int j = 0; j < matrix->size(); j++) {
//			int x = (int)distribution(generator);
			int x = (int) (rand() % primes->size());
			(*matrix)[i][j] = (*primes)[x];
		}
	}
}

void fill_with_consecutive(ms_vector *primes,
		ms_matrix *matrix) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand());
	int x = (int) (rand() % primes->size());

	(*matrix)[0][0] = (*primes)[x-1];
	(*matrix)[0][1] = (*primes)[x+1];
	(*matrix)[0][2] = (*primes)[x-2];
	(*matrix)[1][0] = (*primes)[x+2];
	(*matrix)[1][1] = (*primes)[x];
	(*matrix)[1][2] = (*primes)[x-3];
	(*matrix)[2][0] = (*primes)[x+3];
	(*matrix)[2][1] = (*primes)[x-4];
	(*matrix)[2][2] = (*primes)[x+4];
}

bool is_magic_square(ms_matrix *matrix) {
	// test diagonal
	int tot = (*matrix)[0][0] + (*matrix)[1][1] + (*matrix)[2][2];
	if (tot != ((*matrix)[2][0] + (*matrix)[1][1] + (*matrix)[0][2]))
		return false;

	// test rows and cols
	for (int i = 0; i < 3; i++) {
		// test col
		if (tot != ((*matrix)[i][0] + (*matrix)[i][1] + (*matrix)[i][2]))
			return false;
		// test row
		if (tot != ((*matrix)[0][i] + (*matrix)[1][i] + (*matrix)[2][i]))
			return false;
	}

	return true;
}

/**
 * In this approach the matrix elements are taken at random and 
 * repeating primes within the matrix is allowed.
 *
 * @param primes array of prime numbers
 */
bool explorer_strategy(ms_vector *primes, ms_matrix *matrix) {
	fill_random_matrix(primes, matrix);
	return is_magic_square(matrix);
}

/**
 * In this approach the matrix elements are taken at random and
 * repeating primes within the matrix is allowed.
 *
 * @param primes array of prime numbers
 */
bool consecutive_strategy(ms_vector *primes, ms_matrix *matrix) {
	fill_with_consecutive(primes, matrix);
	return is_magic_square(matrix);
}
