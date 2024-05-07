const int mod = 998244353;
int d;

int multiply(int a, int b){
    if (b == 0) return 0;
    if (b == 1) return a % mod;
    int cur = multiply(a, b / 2);
    cur = (cur + cur) % mod;
    if (b % 2 == 1) return (cur + a) % mod;
    else return cur;
}

int power(int a, int b){
    if (b == 0) return 1;
    int cur = power(a, b / 2);
    cur = multiply(cur, cur);
    if (b % 2 == 1) return multiply(cur, a);
    else return cur;
}

const int maxlen = 2097152;
int temp[maxlen], a[maxlen], b[maxlen], c[maxlen];

int MemMove(int dst[], int dst_pos, int src[], int len){
    for (int i = 0; i < len; i = i + 1) {
        dst[dst_pos + i] = src[i];
    }
    return len - 1;
}

int fft(int arr[], int begin_pos, int n, int w){
    if (n == 1) return 1;
    for (int i = 0; i < n; i = i + 1) {
        if (i % 2 == 0) temp[i / 2] = arr[i + begin_pos];
        else temp[n / 2 + i / 2] = arr[i + begin_pos];
        i = i + 1;
    }

    MemMove(arr, begin_pos, temp, n);
    fft(arr, begin_pos, n / 2, multiply(w, w));
    fft(arr, begin_pos + n / 2, n / 2, multiply(w, w));
    int wn = 1;
    for (int i = 0; i < n / 2; i = i + 1){
        int x = arr[begin_pos + i];
        int y = arr[begin_pos + i + n / 2];
        arr[begin_pos + i] = (x + multiply(wn, y)) % mod;
        arr[begin_pos + i + n / 2] = (x - multiply(wn, y) + mod) % mod;
        wn = multiply(wn, w);
    }
    return 0;
}

int main(){
    int n = getarray(a);
    int m = getarray(b);
    starttime();
    for (d = 1; d < n + m - 1; d = d * 2);
    fft(a, 0, d, power(3, (mod - 1) / d));
    fft(b, 0, d, power(3, (mod - 1) / d));

    int i = 0;
    while (i < d){
        a[i] = multiply(a[i], b[i]);
        i = i + 1;
    }
    fft(a, 0, d, power(3, mod-1 - (mod-1)/d));
    for (int i = 0; i < d; i = i + 1) {
        a[i] = multiply(a[i], power(d, mod-2));
    }
    stoptime();
    putarray(n + m - 1, a);
    return 0;
}