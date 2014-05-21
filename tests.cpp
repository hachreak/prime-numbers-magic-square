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
#include "magic-square-v1.cpp"

/**
 * find first 100 prime numbers: print array is_prime
 */
//void test_01() {
//	//
//	int limit = 100;
//
//	find_prime_numbers(limit, is_prime);
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
void test_02() {
	int limit = 100;
	cout<<"Find prime numbers.... [2,"<<limit<<"]\n";
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	print_vector(primes);
	cout<<"\n";
}

/**
 * get random matrix of prime numbers
 */
void test_03() {
	cout<<"Fill a matrix with random prime numbers...\n";
	int limit = 100;
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	int length = 3;

	ms_matrix matrix(length, ms_vector(length));
	fill_random_matrix(&primes, &matrix);
	print_matrix(matrix);

	cout<<"\n";
	ms_matrix matrix2(length, ms_vector(length));
	fill_random_matrix(&primes, &matrix2);
	print_matrix(matrix2);
}

/**
 * test function is_magic_square()
 */
void test_04() {
	cout<< "Test if identify correctly a magic square....\n";
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
	cout << (is_magic_square(&matrix) ? "yes" : "no") << endl;

	matrix[2][2] = 4;

	print_matrix(matrix);
	cout << (is_magic_square(&matrix) ? "yes" : "no") << endl;
}

/**
 * test first strategy: explorer_strategy() + print generated matrix
 * @strategy
 */
void test_05() {
	cout<<"Explorer strategy...\n";
	int limit = 100000;
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	vector<ms_matrix> list;

	for (int i = 0; i < 10; i++) {
		int length = 3;
		ms_matrix matrix(length, ms_vector(length));

		if (explorer_strategy(&primes, &matrix)) {
			cout << "Found a magic square!\n";
			print_matrix(matrix);
		}

		list.push_back(matrix);
	}

	// print all generated matrix
	cout << "print matrix:\n";
	print_list_matrix(list);
}

/**
 * generate a consecutive matrix and view
 */
void test_06() {
	cout<<"Generate a consecutive matrix and view...\n";
	int limit = 100000;
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	int length = 3;
	ms_matrix matrix(length, ms_vector(length));

	fill_with_consecutive(&primes, &matrix);
	print_matrix(matrix);
}

/**
 * Test consecutive_strategy + view generated matrix
 * @strategy
 */
void test_07() {
	cout<<"Test consecutive strategy...\n";
	int limit = 100000;
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	vector<ms_matrix> list;

	for (int i = 0; i < 10; i++) {
		int length = 3;
		ms_matrix matrix(length, ms_vector(length));

		if (consecutive_strategy(&primes, &matrix)) {
			cout << "Found a magic square!\n";
			print_matrix(matrix);
		}
		list.push_back(matrix);
	}

	// print all generated matrix
	cout << "Print all generated matrix:\n";
	print_list_matrix(list);
}

/**
 * test if exists a another trio of prime numbers that satisfy the restriction: the sum.
 * 1) select randomly first 3 prime numbers and a+b+c = sum
 * 2) look if exists other 3 prime numbers that x+y+z = sum = a+b+c
 */
void test_08() {
	cout<<"Test if found a trio of prime numbers that satisfy the condition A+B+C=SUM, where you know A and SUM...\n";
	int limit = 100000;
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	ms_matrix matrix(3, ms_vector(3));

	for (int i = 0; i < 10; i++) {
		srand(time(NULL) + rand());

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

		cout << matrix[0][0] << "\t+ " << matrix[0][1] << "\t+ " << matrix[0][2]
				<< "\t = " << sum << " (prime numbers selected) " << endl;

		vector<int> not2consider;
		// list of index of prime numbers not to be considered in search
		not2consider.push_back(seed_00);
		not2consider.push_back(seed_01);
		not2consider.push_back(seed_02);

		int first, first_position, second, second_position;

		if (look_for_couple_prime_with_condition(&primes, sum - matrix[0][0],
				not2consider, first, first_position, second, second_position)) {
			cout << first << "\t + " << second << "\t + " << matrix[0][0]
					<< "\t = " << (matrix[0][0] + first + second)
					<< "\t position: (" << first_position << ","
					<< second_position << ")" << endl;
		}
	}

}

/**
 * Heuristic strategy:
 * Select randomly the first column, then I look for
 * the others cells as consequence of this selection.
 * @strategy
 */
void test_09() {
	cout<<"Test the heuristic strategy...\n";
	int limit = 100000;
	ms_vector primes;
	find_prime_numbers(limit, &primes);

	vector<ms_matrix> list;

	for (int i = 0; i < 10; i++) {
		int length = 3;
		ms_matrix matrix(length, ms_vector(length));

		if (heuristic_strategy(&primes, &matrix)) {
			cout << "Found a magic square!\n";
			print_matrix(matrix);
		}

		list.push_back(matrix);
	}

	// print all generated matrix
	cout << "Print all generated matrix:\n";
	print_list_matrix(list);
}

int main(int argc, char *argv[]) {
//s	test_01();
	test_02();
	test_03();
	test_04();
	test_05();
	test_06();
	test_07();
	test_08();
	test_09();
//	int length = 3;
//	ms_matrix matrix(length, ms_vector(length));
//	print_matrix(matrix);
	return 0;
}
