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

// http://en.wikipedia.org/wiki/Sieve_of_Atkin

//#define limit 1000000

using namespace std;

void find_prime_numbers(int limit, bool *is_prime){
  int sqrt_limit = ceil(sqrt(limit));

  for(int i=0; i<(limit+1); i++){
    is_prime[i] = false;
  }
  is_prime[2] = true;
  is_prime[3] = true;

  for(int x=1; x<=sqrt_limit; x++){
    for(int y=1; y<=sqrt_limit; y++){
      int n = 4*x*x + y*y;

      if(n <= limit && ((n % 12) == 1 || (n % 12) == 5)) is_prime[n] = ! is_prime[n];
   
      n = 3*x*x + y*y;

      if(n <= limit && (n % 12) == 7) is_prime[n] = ! is_prime[n];

      n = 3*x*x - y*y;

      if(x > y && n <= limit && (n % 12) == 11) is_prime[n] = ! is_prime[n];
    }
  }

  for(int n=5; n<=sqrt_limit; n++){
    if(is_prime[n]){
      int k = n*n;
      for(int i=k; i<=limit; i+=k){
          is_prime[i] = false;
      }
    }
  }

  return;
}

int main(int argc, char *argv[]){
  int limit = 100000;
  bool is_prime[limit + 1];
  find_prime_numbers(limit, is_prime);
  for(int n=0; n<=limit; n++){
    if(is_prime[n]) cout << n << "\n";
  }
  return 0;
}
