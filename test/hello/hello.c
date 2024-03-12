#include "sysy.h"

int x, y = 10;

const int N = 10;

int fib(int n) {
  if (n == 1) return 1;
  else if (n == 2) return 1;
  else return fib(n - 1) + fib(n - 2);
}

int main() {
  return fib(N);
}