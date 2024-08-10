#include <stdio.h>
#include <stdbool.h>

// void bubble_sort(int arr[], int n) {
void bubble_sort(int* arr, int n) {
  bool swapped;
  for (int i = 0; i < n-1; i++) {
    swapped = false;
    for (int j = 0; j < n-i-1; j++) {
      if (arr[j] > arr[j+1]) {
	// Swap arr[j] and arr[j+1]
	int temp = arr[j];
	arr[j] = arr[j+1];
	arr[j+1] = temp;
	swapped = true;
      }
    }
    // If no elements were swapped, the array is already sorted
    if (!swapped)
      break;
  }
}

// void print_array(int arr[], int n) {
void print_array(int* arr, int n) {
  for (int i = 0; i < n; i++) {
    printf("%ld ", arr[i]);
  }
  printf("\n");
}

int main() {
  int arr[] = {64, 34, 25, 12, 22, 11, 90};
  int n = sizeof(arr)/sizeof(arr[0]);
    
  printf("Original array: \n");
  print_array(arr, n);
    
  bubble_sort(arr, n);
    
  printf("Sorted array: \n");
  print_array(arr, n);
    
  return 0;
}
