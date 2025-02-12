#include <stdio.h>
#include "memory_control.h"
// Function to print the free list visually
int main(void) {
  // Initialize the global memory pool
  init_memory_pool();
  print_free_list();
  // Allocate space for 10 integers
  int* int_p = (int*)_calloc(10 , sizeof(int));
  if (int_p == NULL) {
    printf("Allocation failed.\n");
    return 1;
  }
  // Print the values
  for (int i = 0; i < 10; i++) {
    printf("int_p[%d] = %d\n", i, int_p[i]);
  }
  // Use the allocated memory
  for (int i = 0; i < 10; i++) {
    int_p[i] = i * 2; // Fill with some values
  }
  int*realloc_ptr =  _realloc(int_p,20*sizeof(int));
  int* int_p2 = (int*)_malloc(500 * sizeof(int));
  print_free_list();
  // Print the values
  for (int i = 0; i < 10; i++) {
    printf("int_p[%d] = %d\n", i, int_p[i]);
  }

  // Free the allocated memory
  _free(int_p);
  print_free_list();
  printf("Memory freed.\n");
  printf ("%d\n",sizeof (int));
  _free(int_p2);
  print_free_list();
  return 0;
}
