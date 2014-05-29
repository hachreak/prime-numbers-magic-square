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

// temporary debug
#include "magic-square.h"
#include <assert.h>

/**
 * find first 100 prime numbers: print array is_prime
 */
//void test_01() {
//	//
//	int limit = 100;
//
//	find_prime_numbers(world, limit, is_prime);
//	//cout<<sizeof(is_prime);exit(0);
//	for (int n = 0; n <= limit; n++) {
//		if (is_prime[n])
//			cout << n << "\n";
//	}
//	exit(0);
//}
/**
 * transform is_prime to primes
 */
void test_02(mpi::communicator world, int limit) {
	if (world.rank() == 0) {
		cout << "Find prime numbers.... [2," << limit << "]\n";
	}

	ms_vector primes;
	find_prime_numbers(world, limit, &primes);

	if (world.rank() == 0) {
		print_vector(primes);
		cout << "\n";
	}
	world.barrier();
}

/**
 * get random matrix of prime numbers
 */
void test_03(mpi::communicator world) {
	if (world.rank() == 0) {
		cout << "Fill a matrix with random prime numbers...\n";
	}

	int limit = 100;
	ms_vector primes;
	find_prime_numbers(world, limit, &primes);

	if (world.rank() == 0) {
		int length = 3;

		ms_matrix matrix(length, ms_vector(length));
		fill_random_matrix(&primes, &matrix, 3);
		ms_matrix matrix2(length, ms_vector(length));
		fill_random_matrix(&primes, &matrix2, 6);

		print_matrix(matrix);
		cout << "\n";
		print_matrix(matrix2);
	}
	world.barrier();
}

/**
 * test function is_magic_square()
 */
void test_04(mpi::communicator world) {
	if (world.rank() == 0) {
		cout << "Test if identify correctly a magic square....\n";
		int length = 3;
		ms_matrix matrix(length, ms_vector(length));

		matrix[0][0] = 7;
		matrix[0][1] = 61;
		matrix[0][2] = 43;
		matrix[1][0] = 73;
		matrix[1][1] = 37;
		matrix[1][2] = 1;
		matrix[2][0] = 31;
		matrix[2][1] = 13;
		matrix[2][2] = 67;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(is_magic_square(matrix));

		matrix[2][2] = 4;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(!is_magic_square(matrix));

		matrix[2][2] = 67;
		matrix[1][1] = 38;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(!is_magic_square(matrix));

		matrix[1][1] = 36;
		matrix[1][2] = 0;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(!is_magic_square(matrix));

		matrix[0][0] = 39419;
		matrix[0][1] = 66499;
		matrix[0][2] = 69403;
		matrix[1][0] = 35911;
		matrix[1][1] = 99961;
		matrix[1][2] = 6011;
		matrix[2][0] = 99991;
		matrix[2][1] = 8861;
		matrix[2][2] = 99907;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(!is_magic_square(matrix));

		matrix[0][0] = 33931;
		matrix[0][1] = 49999;
		matrix[0][2] = 11527;
		matrix[1][0] = 7;
		matrix[1][1] = 45439;
		matrix[1][2] = 61;
		matrix[2][0] = 61519;
		matrix[2][1] = 19;
		matrix[2][2] = 83869;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(!is_magic_square(matrix));

		matrix[0][0] = 33931;
		matrix[0][1] = 7;
		matrix[0][2] = 61519;
		matrix[1][0] = 49999;
		matrix[1][1] = 45439;
		matrix[1][2] = 19;
		matrix[1][0] = 11527;
		matrix[1][1] = 61;
		matrix[1][2] = 83869;

		print_matrix(matrix);
		cout << (is_magic_square(matrix) ? "yes" : "no") << endl;
		assert(!is_magic_square(matrix));

	}
	world.barrier();
}

/**
 * test first strategy: explorer_strategy() + print generated matrix
 * @strategy
 */
void test_05(mpi::communicator world, int limit) {
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
	if (is_magic_square(matrix)) {
		cout << "Found a magic square!\n";
		print_matrix(matrix);
	}

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

/**
 * generate a consecutive matrix and view
 */
void test_06(mpi::communicator world, int limit) {
	if (world.rank() == 0) {
		cout << "Generate a consecutive matrix and view...\n";
	}

	ms_vector primes;
	find_prime_numbers(world, limit, &primes);

	if (world.rank() == 0) {

		int length = 3;
		ms_matrix matrix(length, ms_vector(length));

		fill_with_consecutive(&primes, &matrix, 6);
		print_matrix(matrix);
	}
}

/**
 * Test consecutive_strategy + view generated matrix
 * @strategy
 */
void test_07(mpi::communicator world, int limit) {
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
	if (is_magic_square(matrix)) {
		cout << "Found a magic square!\n";
		print_matrix(matrix);
	}

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

/**
 * test if exists a another trio of prime numbers that satisfy the restriction: the sum.
 * 1) select randomly first 3 prime numbers and a+b+c = sum
 * 2) look if exists other 3 prime numbers that x+y+z = sum = a+b+c
 */
void test_08(mpi::communicator world, int limit) {
	ms_vector primes;
	// vector to collecting all generated messages
	vector<string> list;

	if (world.rank() == 0) {
		cout
				<< "Test if found a trio of prime numbers that satisfy the condition A+B+C=SUM, where you know A and SUM...\n";
	}

	find_prime_numbers(world, limit, &primes);

	// send to all the prime numbers
	mpi::broadcast(world, primes, 0);

	ms_matrix matrix(3, ms_vector(3));

	srand(time(NULL) + rand() + world.rank());

	// select randomly first 3 prime numbers
	int seed_00 = (int) (rand() % primes.size());
	int seed_01 = (int) (rand() % primes.size());
	int seed_02 = (int) (rand() % primes.size());

	// save first column
	matrix[0][0] = primes[seed_00];
	matrix[0][1] = primes[seed_01];
	matrix[0][2] = primes[seed_02];

	// compute the sum
	int sum = matrix[0][0] + matrix[0][1] + matrix[0][2];

	ostringstream convert;

	convert << matrix[0][0] << "\t+ " << matrix[0][1] << "\t+ " << matrix[0][2]
			<< "\t = " << sum << " (prime numbers selected) " << endl;

	vector<int> not2consider;
	// list of index of prime numbers not to be considered in search
	not2consider.push_back(seed_00);
	not2consider.push_back(seed_01);
	not2consider.push_back(seed_02);

	int first, first_position, second, second_position;

	if (look_for_couple_prime_with_condition(&primes, sum - matrix[0][0],
			not2consider, first, first_position, second, second_position)) {

		convert << first << "\t + " << second << "\t + " << matrix[0][0]
				<< "\t = " << (matrix[0][0] + first + second)
				<< "\t position: (" << first_position << "," << second_position
				<< ")" << endl;

		assert(
				(first + second + matrix[0][0])
						== (matrix[0][0] + matrix[0][1] + matrix[0][2]));
	} else {
		convert << "nothing.." << endl;
	}

	string msg = convert.str();
	// receive all generated messages
	mpi::gather(world, msg, list, 0);

	if (world.rank() == 0) {
		for (vector<string>::iterator i = list.begin(); i != list.end(); i++) {
			std::cout << *i << "\t";
		}
		cout << endl;
	}
}

/**
 * Heuristic strategy:
 * Select randomly the first column, then I look for
 * the others cells as consequence of this selection.
 * @strategy
 */
void test_09(mpi::communicator world, int limit) {
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

	if (fill_in_heuristic_mode_1(&primes, &matrix, rank) && is_magic_square(matrix)) {
		cout << "Found a magic square!\n";
		print_matrix(matrix);
	}

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

/**
 * Test of broadcasting a vector
 */
void test_10(mpi::communicator world) {
	ms_vector primes;
	int limit = 100;

	if (world.rank() == 0) {
		cout << "Test broadcasting...\n";
	}

	// generate primes numbers
	find_prime_numbers(world, limit, &primes);

	if (world.rank() == 0) {

		print_vector(primes);
	}

	// vector to collecting all generated matrix
	vector<ms_matrix> list;

	// send to all the prime numbers
	mpi::broadcast(world, primes, 0);
	//MPI_Bcast(&primes.data(), primes.size(), MPI_INT, 0, MPI_COMM_WORLD);

	if (world.rank() == 1) {
		print_vector(primes);
	}
}

/**
 * Test fill a matrix with heuristic strategy
 */
void test_11(mpi::communicator world, int limit) {
	if (world.rank() == 0) {
		cout << "Test fill in heuristic strategy v2...\n";
	}

	ms_vector primes;
	find_prime_numbers(world, limit, &primes);

	if (world.rank() == 0) {
		ms_matrix matrix(3, ms_vector(3));

		fill_in_heuristic_mode_2(&primes, &matrix, world.rank());

		print_matrix(matrix);
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
void test_12(mpi::communicator world, int limit) {
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

	if (fill_in_heuristic_mode_2(&primes, &matrix, rank)) {
		cout << "Found a magic square!\n";
		print_matrix(matrix);
	}

	// receive all generated matrix
	mpi::gather(world, matrix, list, 0);

	if (rank == 0) {
		// print all generated matrix
		cout << "Print all generated matrix:\n";
		print_list_matrix(list);
	}
}

int main(int argc, char *argv[]) {
	mpi::environment env(argc, argv);
	mpi::communicator world;

	int limit = 100000;

	test_02(world, 100);
	test_02(world, 1000);
	test_02(world, 10000);

	test_03(world);
	test_04(world);
	test_05(world, limit);
	test_06(world, limit);
	test_07(world, limit);
	test_08(world, limit);
	test_09(world, limit);
	test_10(world);
	test_11(world, limit);
	test_12(world, limit);

	return 0;
}
