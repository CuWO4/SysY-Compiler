int buf[2][100];

// sort [l, r)
void merge_sort(int low, int high)
{
    if (low + 1 >= high) { return; }

    int mid = (low + high) / 2;
    merge_sort(low, mid);
    merge_sort(mid, high);

    int i = low, j = mid, k = low;
    while (i < mid && j < high) {
        if (buf[0][i] < buf[0][j]) {
            buf[1][k] = buf[0][i];
            i = i + 1;
        } else {
            buf[1][k] = buf[0][j];
            j = j + 1;
        }
        k = k + 1;
    }
    while (i < mid) {
        buf[1][k] = buf[0][i];
        i = i + 1;
        k = k + 1;
    }
    while (j < high) {
        buf[1][k] = buf[0][j];
        j = j + 1;
        k = k + 1;
    }

    while (low < high) {
        buf[0][low] = buf[1][low];
        low = low + 1;
    }
}

int main()
{
    int n = getarray(buf[0]);
    merge_sort(0, n);
    putarray(n, buf[0]);
    return 0;
}
