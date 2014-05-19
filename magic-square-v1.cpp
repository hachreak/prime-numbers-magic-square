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
