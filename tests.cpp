
/**
 * find first 100 prime numbers: print array is_prime
 */
void test_01() {
	//
	int limit = 100;
	bool is_prime[limit + 1];
	find_prime_numbers(limit, is_prime);
	for (int n = 0; n <= limit; n++) {
		if (is_prime[n])
			cout << n << "\n";
	}
}

/**
 * transform is_prime to primes
 */
void test_02() {
	// 
	int limit = 100;
	bool is_prime[limit + 1];
	find_prime_numbers(limit, is_prime);

	std::vector<int> primes;
	is_prime2primes(is_prime, limit, &primes);

	print_vector(primes);
}

/**
 * get random matrix of prime numbers
 */
void test_03() {
	// 
	int limit = 100;
	bool is_prime[limit + 1];
	find_prime_numbers(limit, is_prime);

	std::vector<int> primes;
	is_prime2primes(is_prime, limit, &primes);

	int length = 3;
	vector < vector<int> > matrix(length, std::vector<int>(length));
	fill_random_matrix(&primes, &matrix);
	print_matrix (matrix);
	vector < vector<int> > matrix2(length, std::vector<int>(length));
	fill_random_matrix(&primes, &matrix2);
	print_matrix (matrix2);
}

/**
 * test function is_magic_square()
 */
void test_04() {
	int length = 3;
	vector < vector<int> > matrix(length, std::vector<int>(length));

	matrix[0][0] = 7;
	matrix[0][1] = 61;
	matrix[0][2] = 43;
	matrix[1][0] = 73;
	matrix[1][1] = 37;
	matrix[1][2] = 1;
	matrix[2][0] = 31;
	matrix[2][1] = 13;
	matrix[2][2] = 67;

	cout << (is_magic_square(&matrix) ? "yes" : "no") << endl;

	matrix[2][2] = 4;

	cout << (is_magic_square(&matrix) ? "yes" : "no") << endl;
}

/**
 * test first strategy: explorer_strategy()
 */
void test_05() {
	int limit = 100;
	bool is_prime[limit + 1];
	find_prime_numbers(limit, is_prime);

	std::vector<int> primes;
	is_prime2primes(is_prime, limit, &primes);
	
	vector<vector<int> > *matrix; 

	for(int i=0; i<100000000; i++){
		int length = 3;
		vector<vector<int> > matrix(length, std::vector<int>(length));
		if(explorer_strategy(&primes, &matrix)){
			print_matrix(matrix);
		}
	}
}
