#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include "StrChain.h"
long is_file(ValeStr* path) {
    struct stat path_stat;
    stat(path->chars, &path_stat);
    return S_ISREG(path_stat.st_mode);    
}

long is_dir(ValeStr* path) {
    return !is_file(path);
}

long exists(ValeStr* path) {
    if(is_dir(path)) {
        DIR* dir = opendir(path->chars);
        long retval = dir ? 1 : 0;
        closedir(dir);
        return retval;
    }else{
        FILE* file = fopen(path->chars, "r");
        long retval = file ? 1 : 0; 
        fclose(file);
        return retval;
    }
}
#include <stdint.h>
#include <assert.h>

ValeStr* ValeStrNew(int64_t length);

// Aborts on failure, beware!
ValeStr* readFileAsString(ValeStr* filenameVStr) {
  char* filename = filenameVStr->chars;

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror(filename);
    exit(1);
  }

  fseek(fp, 0L, SEEK_END);
  long lSize = ftell(fp);
  rewind(fp);

  /* allocate memory for entire content */
  char *buffer = malloc(lSize);
  if (!buffer) {
    fclose(fp);
    fputs("memory alloc fails", stderr);
    exit(1);
  }

  /* copy the file into the buffer */
  if (1 != fread(buffer, lSize, sizeof(char), fp)) {
    fclose(fp);
    free(buffer);
    fputs("Failed to read file", stderr);
    exit(1);
  }

  ValeStr* result = ValeStrNew(lSize);
  strncpy(result->chars, buffer, lSize);

  fclose(fp);
  free(buffer);

  return result;
}

void writeStringToFile(ValeStr* filenameVStr, ValeStr* contentsVStr) {
  char *filename = filenameVStr->chars;
  char* contents = contentsVStr->chars;
  int contentsLen = contentsVStr->length;

  //printf("contents len: %d\n", contentsLen);

  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror(filename);
    exit(1);
  }

  if (contentsLen > 0) {
    if (1 != fwrite(contents, contentsLen, 1, fp)) {
      fclose(fp);
      fputs("Failed to write file", stderr);
      exit(1);
    }
  }

  fclose(fp);
}


