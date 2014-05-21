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
#include <algorithm>
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

/**
 * Fill matrix with random selected prime numbers from primes.
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
void fill_random_matrix(ms_vector *primes, ms_matrix *matrix) {
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

/**
 * Fill matrix with a consecutive strategy: first random and for others positions select consecutive prime numbers
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
void fill_with_consecutive(ms_vector *primes, ms_matrix *matrix) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand());
	int seed = (int) (rand() % primes->size());

	(*matrix)[0][0] = (*primes)[seed - 1];
	(*matrix)[0][1] = (*primes)[seed + 1];
	(*matrix)[0][2] = (*primes)[seed - 2];
	(*matrix)[1][0] = (*primes)[seed + 2];
	(*matrix)[1][1] = (*primes)[seed];
	(*matrix)[1][2] = (*primes)[seed - 3];
	(*matrix)[2][0] = (*primes)[seed + 3];
	(*matrix)[2][1] = (*primes)[seed - 4];
	(*matrix)[2][2] = (*primes)[seed + 4];
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
		vector<int> not2consider, int &first, int &first_position, int &second,
		int &second_position) {
	int size = primes->size();
	//int middle = (int) (size / 2);
	for (int i = 0; i < size; i++) {
		// if the index of prime number is not ones to avoid
		if (find(not2consider.begin(), not2consider.end(), i)
				== not2consider.end()) {
			int f = (*primes)[i];
			for (int j = 0; j < size; j++) {
				// if the index of prime number is not ones to avoid
				if (find(not2consider.begin(), not2consider.end(), j)
						== not2consider.end()) {
					int s = (*primes)[j];
					if (sum == (f + s)) {
						// found!
						first = f;
						first_position = i;
						second = s;
						second_position = j;
						return true;
					}
				}
			}
		}
	}
	return false;
}

/**
 * Fill matrix with a heuristic strategy: first random and for others positions as result of scanned prime numbers vector
 * @param primes vector of prime numbers
 * @param matrix matrix to fill
 */
bool fill_in_heuristic_mode(ms_vector *primes, ms_matrix *matrix) {
	// TODO fix random generator! T_T
	srand(time(NULL) + rand());

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

	for(int i=0; i<matrix->size(); i++){
		// test i-th column
		if (look_for_couple_prime_with_condition(primes, sum - (*matrix)[0][i],
				not2consider, first, first_position, second, second_position)) {
			// save position of founded prime numbers
			not2consider.push_back(first_position);
			not2consider.push_back(second_position);
			// save new prime numbers
			(*matrix)[1][i] = first;
			(*matrix)[2][i] = second;
		} else {
			return false;
		}
	}

	return true;
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
 * @param matrix matrix of prime numbers generated
 */
bool explorer_strategy(ms_vector *primes, ms_matrix *matrix) {
	fill_random_matrix(primes, matrix);
	return is_magic_square(matrix);
}

/**
 * In this approach only the central element is taken at random and
 * the others are taken around him.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
bool consecutive_strategy(ms_vector *primes, ms_matrix *matrix) {
	fill_with_consecutive(primes, matrix);
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
bool heuristic_strategy(ms_vector *primes, ms_matrix *matrix) {
	if(fill_in_heuristic_mode(primes, matrix))
		return is_magic_square(matrix);
	return false;
}
