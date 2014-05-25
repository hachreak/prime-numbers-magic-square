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

#include <boost/mpi.hpp>
#include "magic-square.h"

namespace mpi = boost::mpi;

int main(int argc, char *argv[]) {
	mpi::environment env(argc, argv);
	mpi::communicator world;

	int limit = 100000;
	int strategy = 4;
	
	if(strategy == 1){
		test_explorer_strategy(world, limit);
	}else if(strategy == 2){
		test_consecutive_strategy(world, limit);
	}else if(strategy == 3){
		test_heuristic_strategy_1(world, limit);
	}else if(strategy == 4){
		test_heuristic_strategy_2(world, limit);
	}
	

	return 0;
}
