#ifndef STRCHAIN_HEADER_VALE
#define STRCHAIN_HEADER_VALE

typedef long ValeInt;

typedef struct ValeStr {
    unsigned long length;
    char chars[0]; 
} ValeStr;

typedef struct StrChain {
  unsigned long length;
  ValeStr* elements[0];
} StrChain;
#define ValeReleaseMessage(msg) (free(*((void**)(msg) - 2)))

#endif
