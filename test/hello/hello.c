#include "sysy.h"

int x, y = 10;

const int N = 10;

int fib(int n) {
  if (n == 1) return 1;
  else if (n == 2) return 1;
  else return fib(n - 1) + fib(n - 2);
}

int foo(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, int x10) {
    return x10;
}

int main() {
  foo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  return fib(N);
}