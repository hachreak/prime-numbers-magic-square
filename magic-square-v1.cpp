#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

// http://en.wikipedia.org/wiki/Sieve_of_Atkin

#define limit 1000000

using namespace std;

int main(int argc, char *argv[]){
  long sqrt_limit = ceil(sqrt(limit));
  bool is_prime[limit + 1];

  for(long i=0; i<(limit+1); i++){
    is_prime[i] = false;
  }
  is_prime[2] = true;
  is_prime[3] = true;

  for(long x=1; x<=sqrt_limit; x++){
    for(long y=1; y<=sqrt_limit; y++){
      long n = 4*x*x + y*y;

      if(n <= limit && ((n % 12) == 1 || (n % 12) == 5)) is_prime[n] = ! is_prime[n];
   
      n = 3*x*x + y*y;

      if(n <= limit && (n % 12) == 7) is_prime[n] = ! is_prime[n];

      n = 3*x*x - y*y;

      if(x > y && n <= limit && (n % 12) == 11) is_prime[n] = ! is_prime[n];
    }
  }

//  long z = 1;
  for(long n=5; n<=sqrt_limit; n++){
    if(is_prime[n]){
      long k = n*n;
//      while(k <= limit){
      for(int i=k; i<=limit; i+=k){
          is_prime[i] = false;
      }
//        z++;
//        k = z*n*n;
    }
  }

  //cout << "2\n3\n";
  for(long n=0; n<=limit; n++){
    if(is_prime[n]) cout << n << "\n";
  }
//  cout << endl;
  return 0;
}
