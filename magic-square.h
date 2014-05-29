#ifndef MAGIC_SQUARE_H_
#define MAGIC_SQUARE_H_

#include <vector>
#include <boost/mpi.hpp>

using namespace std;
namespace mpi = boost::mpi;

typedef vector<int> ms_vector;
typedef vector<vector<int> > ms_matrix;

/**
 * Print a vector
 */
void print_vector(ms_vector vector);

/**
 * Print a matrix
 */
void print_matrix(ms_matrix matrix);

/**
 * Print a list of matrixs
 */
void print_list_matrix(vector<ms_matrix> list);

/**
 * Test if the matrix is a magic_square
 * @param matrix matrix to control
 * @return true, if the matrix is a magic square
 */
bool is_magic_square(ms_matrix matrix);

/**
 * Fill matrix with random selected prime numbers from primes.
 * In this approach the matrix elements are taken at random and
 * repeating primes within the matrix is allowed.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
void fill_random_matrix(ms_vector *primes, ms_matrix *matrix, int seed);

/**
 * Fill matrix with a consecutive strategy: first random and for others positions select consecutive prime numbers
 * In this approach only the central element is taken at random and
 * the others are taken around him.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
void fill_with_consecutive(ms_vector *primes, ms_matrix *matrix,
		int seed_random_num);

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
		int &second_position);

/**
 * Heuristic strategy:
 * In this approach, I select randomly the first column,
 * then I look for the others cells as consequence of this selection.
 *
 * @param primes array of prime numbers
 * @param matrix matrix of prime numbers generated
 */
bool fill_in_heuristic_mode_1(ms_vector *primes, ms_matrix *matrix, int seed);

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
bool fill_in_heuristic_mode_2(ms_vector *primes, ms_matrix *matrix, int seed);

/**
 * find prime numbers in a range between [2,limit]
 *
 * @param limit upper limit
 * @param is_prime return a array[limit+1] with a representation of number (if is_prime[n] == true then n is prime, false otherwise)
 */
void find_prime_numbers(mpi::communicator world, int limit, ms_vector *primes);

/**
 * test first strategy: explorer_strategy() + print generated matrix
 * @strategy
 */
void test_explorer_strategy(mpi::communicator world, int limit);

/**
 * Test consecutive_strategy + view generated matrix
 * @strategy
 */
void test_consecutive_strategy(mpi::communicator world, int limit);

/**
 * Heuristic strategy:
 * Select randomly the first column, then I look for
 * the others cells as consequence of this selection.
 * @strategy
 */
void test_heuristic_strategy_1(mpi::communicator world, int limit);

/**
 * Heuristic strategy 2:
 * Select randomly the first 4 numbers, then I look for
 * the others cells as consequence of this selection.
 * I have 8 equations and 9 variables.
 * But, I need only 5 variables and the others can be
 * calculated from those.
 * @strategy
 */
void test_heuristic_strategy_2(mpi::communicator world, int limit);

#endif /* MAGIC_SQUARE_H_ */
