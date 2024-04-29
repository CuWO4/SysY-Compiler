int main() {
  const int n = 5;
  int arr[n] = {1, 4, 5, 2, 3};
  
  int i;
  int j;
  i =0; 
  while(i < n-1){
  // Last i elements are already in place
      j = 0;
      while(j < n-i-1){
          if (arr[j] > arr[j+1]) {
              // swap(& arr[j],& arr[j+1]); 
              int tmp;
              tmp = arr[j+1];
              arr[j+1] = arr[j];
              arr[j] = tmp;
          }
          j = j + 1;
      }
      i = i + 1;
  }
  return 0;
}