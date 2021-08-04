#include <stdint.h>
#include <stdio.h>
#include "stdlib/stdinReadInt.h"
#include "stdlib/getch.h"

ValeInt stdlib_stdinReadInt() {
  ValeInt x = 0;
  scanf("%ld", &x);
  return x;
}

ValeStr* stdlib_getch() {
  int c = getch();
  ValeStr* str = ValeStrNew(1);
  str->chars[0] = (char)c;
  return str;
}
