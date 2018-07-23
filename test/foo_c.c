#include <stdio.h>
#include <string.h>

int int_deref(int *a) {
  return *a;
}

int string_length(char * c, int x, int y) {
  printf("c: %s, x: %d, y: %d\n", c, x, y);
  return 13;
}
