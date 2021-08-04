#include <stdint.h>
#include <stdio.h>
#include "stdlib/stdinReadInt.h"
#include "stdlib/getch.h"

ValeInt stdlib_stdinReadInt() {
  ValeInt x = 0;
  scanf("%ld", &x);
  return x;
}

ValeInt stdlib_getch() {
  return getch();
}
