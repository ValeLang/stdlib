#include "list.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include "stdlib/StrChain.h"

long stdlib_exists(ValeStr* path);
ValeStr* ValeStrNew(int64_t length);

long vale_is_file_internal(ValeStr* path) {
    struct stat path_stat;
    stat(path->chars, &path_stat);
    return S_ISREG(path_stat.st_mode);    
}

long stdlib_is_file(ValeStr* path) {
    return stdlib_exists(path) && vale_is_file_internal(path);
}

long stdlib_is_dir(ValeStr* path) {
    return !stdlib_is_file(path);
}

long stdlib_makeDirectory(ValeStr* path) {
    if(!stdlib_exists(path)) {
        return mkdir(path->chars, 0700);
    }
    return 1;
}

long stdlib_exists(ValeStr* path) {
    if(!vale_is_file_internal(path)) {
        DIR* dir = opendir(path->chars);
        long retval = dir ? 1 : 0;
        if(retval) { closedir(dir); }
        return retval;
    }else{
        FILE* file = fopen(path->chars, "r");
        long retval = file ? 1 : 0; 
        if(retval) { fclose(file); }
        return retval;
    }
}

stdlib_StrChain* stdlib_iterdir(ValeStr* path) {
    vale_queue* entries = vale_queue_empty(); 
    if(stdlib_is_file(path)) {
        perror("is a file not a path");
        exit(0);
    }
    DIR* d;
    struct dirent *dir;
    d = opendir(path->chars);
    if(d) {
        while((dir = readdir(d)) != NULL){
            int64_t length = strlen(dir->d_name);
            ValeStr* path_name = ValeStrNew(length);
            strcpy(path_name->chars, dir->d_name);
            vale_queue_push(entries, path_name); 
        }
        closedir(d); 
    }else{
        printf("cannot open directory: %s\n", path->chars);
        stdlib_StrChain* retval = malloc(sizeof(long));
        retval->length = 0;
        return retval;
    }
    long length = entries->length;
    stdlib_StrChain* retval = (stdlib_StrChain*)vale_queue_to_array(entries); 
    vale_queue_destroy(entries);
    return retval;
}
#include <stdint.h>
#include <assert.h>

// Aborts on failure, beware!
ValeStr* stdlib_readFileAsString(ValeStr* filenameVStr) {
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
    fputs("Failed to read file: ", stderr);
    fputs(filename, stderr);
    fputs("\n", stderr);
    exit(1);
  }

  ValeStr* result = ValeStrNew(lSize);
  strncpy(result->chars, buffer, lSize);

  fclose(fp);
  free(buffer);

  return result;
}

void stdlib_writeStringToFile(ValeStr* filenameVStr, ValeStr* contentsVStr) {
  char *filename = filenameVStr->chars;
  char* contents = contentsVStr->chars;
  int contentsLen = contentsVStr->length;

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


