int main() {
  int i, j, sum;
  for (int i = 0, j = 1; i < 10; i = i + 1, j = j + 2) {
    if (i % 2 == 0) continue;
    if (j >= 20) break;
    sum = sum + i + j;
  }
  return sum;
}