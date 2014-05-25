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
 * Test if the matrix is a magic_square
 * @param matrix matrix to control
 * @return true, if the matrix is a magic square
 */
bool is_magic_square(ms_matrix *matrix);

/**
 * Print a list of matrixs
 */
void print_list_matrix(vector<ms_matrix> list);

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
