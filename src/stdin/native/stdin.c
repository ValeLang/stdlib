#include <stdint.h>
#include <stdio.h>

int64_t stdlib_stdinReadInt() {
  int64_t x = 0;
  scanf("%lld", &x);
  return x;
}
