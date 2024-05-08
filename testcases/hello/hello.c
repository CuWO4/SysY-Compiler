/*






*/

int fib_arr[10] = { 0 };

int fib(int n) {
    if (n <= 1) return n;

    if (fib_arr[n] != 0) {
        return fib_arr[n];
    }
    else {
        return fib_arr[n] = fib(n - 1) + fib(n - 2);
    }
}

int main() {
    int n = getint();
    putint(fib(n));
    return 0;
}